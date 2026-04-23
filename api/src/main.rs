use axum::{Json, Router, extract::Request, middleware::Next, response::Response, routing::get};
use once_cell::sync::Lazy;
use std::time::{Duration, Instant};
use tokio::sync::RwLock;

#[derive(Clone)]
struct Cache<T> {
    data: Option<T>,
    last_update: Option<Instant>,
}

static CACHE: Lazy<RwLock<Cache<serde_json::Value>>> = Lazy::new(|| {
    RwLock::new(Cache {
        data: None,
        last_update: None,
    })
});

const TTL: Duration = Duration::from_mins(5);

async fn fetch_github(token: &str) -> serde_json::Value {
    let query = r#"
    {
      user(login: "doleckijakub") {
        createdAt
        repositories(privacy: PUBLIC) {
          totalCount
        }
        contributionsCollection {
          contributionCalendar {
            totalContributions
            weeks {
              contributionDays {
                date
                contributionCount
              }
            }
          }
        }
      }
    }
    "#;

    let client = reqwest::Client::new();

    client
        .post("https://api.github.com/graphql")
        .bearer_auth(token)
        .header("User-Agent", "axum-app")
        .json(&serde_json::json!({ "query": query }))
        .send()
        .await
        .unwrap()
        .json::<serde_json::Value>()
        .await
        .unwrap()
}

async fn github_stats() -> Json<serde_json::Value> {
    let mut cache = CACHE.write().await;

    let should_refresh = match cache.last_update {
        Some(t) => t.elapsed() > TTL,
        None => true,
    };

    if should_refresh {
        let data = fetch_github(&std::env::var("GITHUB_TOKEN").unwrap()).await;

        cache.data = Some(data.clone());
        cache.last_update = Some(Instant::now());

        return Json(data);
    }

    Json(cache.data.clone().unwrap())
}

async fn log(req: Request, next: Next) -> Response {
    let uri = req.uri().clone();
    let res = next.run(req).await;

    println!("{} {}", uri, res.status().as_u16());

    res
}

#[tokio::main]
async fn main() {
    let app = Router::new()
        .layer(axum::middleware::from_fn(log))
        .route("/github/stats", get(github_stats));

    let listener = tokio::net::TcpListener::bind("0.0.0.0:3001").await.unwrap();

    axum::serve(listener, app).await.unwrap();
}
