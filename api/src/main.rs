use axum::{Router, extract::Request, middleware::Next, response::Response, routing::get};
use chrono::{Datelike, Utc};
use once_cell::sync::Lazy;
use serde::Serialize;
use std::time::{Duration, Instant};
use tokio::sync::RwLock;

#[derive(Serialize, Clone)]
pub struct GitHubStats {
    pub total_contributions: i32,
    pub public_repos: i32,
    pub joined_at: String,

    pub current_streak: i32,
    pub longest_streak: i32,

    pub daily: Vec<DailyContribution>,
}

#[derive(Serialize, Clone)]
pub struct DailyContribution {
    pub date: String,
    pub count: i32,
}

static CACHE: Lazy<RwLock<Option<(GitHubStats, Instant)>>> = Lazy::new(|| RwLock::new(None));

const TTL: Duration = Duration::from_mins(5);

async fn fetch_github_raw(token: &str) -> serde_json::Value {
    let years_query = r#"
    {
      user(login: "doleckijakub") {
        createdAt
        repositories(privacy: PUBLIC) { totalCount }
        contributionsCollection {
          contributionYears
        }
      }
    }
    "#;

    let years_resp: serde_json::Value = reqwest::Client::new()
        .post("https://api.github.com/graphql")
        .bearer_auth(token)
        .header("User-Agent", "axum")
        .json(&serde_json::json!({ "query": years_query }))
        .send()
        .await
        .unwrap()
        .json()
        .await
        .unwrap();

    let user = &years_resp["data"]["user"];
    let years: Vec<i64> = user["contributionsCollection"]["contributionYears"]
        .as_array()
        .unwrap()
        .iter()
        .map(|y| y.as_i64().unwrap())
        .collect();

    let year_fragments: String = years
        .iter()
        .map(|y| {
            let end = if *y == Utc::now().year() as i64 {
                Utc::now().format("%Y-%m-%dT%H:%M:%SZ").to_string()
            } else {
                format!("{y}-12-31T23:59:59Z")
            };
            format!(
                r#"
        y{y}: contributionsCollection(from: "{y}-01-01T00:00:00Z", to: "{end}") {{
          contributionCalendar {{
            weeks {{
              contributionDays {{
                date
                contributionCount
              }}
            }}
          }}
        }}
    "#
            )
        })
        .collect();

    let full_query = format!(
        r#"
    {{
      user(login: "doleckijakub") {{
        createdAt
        repositories(privacy: PUBLIC) {{ totalCount }}
        {year_fragments}
      }}
    }}
    "#
    );

    reqwest::Client::new()
        .post("https://api.github.com/graphql")
        .bearer_auth(token)
        .header("User-Agent", "axum")
        .json(&serde_json::json!({ "query": full_query }))
        .send()
        .await
        .unwrap()
        .json()
        .await
        .unwrap()
}

fn flatten_days(weeks: &serde_json::Value) -> Vec<(String, i32)> {
    let mut days = vec![];

    for week in weeks.as_array().unwrap() {
        for day in week["contributionDays"].as_array().unwrap() {
            days.push((
                day["date"].as_str().unwrap().to_string(),
                day["contributionCount"].as_i64().unwrap() as i32,
            ));
        }
    }

    days
}

fn calculate_streaks(days: &[(String, i32)]) -> (i32, i32) {
    let mut current = 0;
    let mut longest = 0;
    let mut temp = 0;

    for (_, count) in days {
        if *count > 0 {
            temp += 1;
            longest = longest.max(temp);
        } else {
            temp = 0;
        }
    }

    for (_, count) in days.iter().rev() {
        if *count > 0 {
            current += 1;
        } else {
            break;
        }
    }

    (current, longest)
}

fn build_stats(raw: serde_json::Value) -> GitHubStats {
    let user = &raw["data"]["user"];

    let mut all_days: Vec<(String, i32)> = user
        .as_object()
        .unwrap()
        .iter()
        .filter(|(k, _)| k.starts_with('y') && k.len() == 5)
        .flat_map(|(_, v)| {
            let weeks = &v["contributionCalendar"]["weeks"];
            flatten_days(weeks)
        })
        .collect();

    all_days.sort_by(|a, b| a.0.cmp(&b.0));

    let total: i32 = all_days.iter().map(|(_, c)| c).sum();
    let (current_streak, longest_streak) = calculate_streaks(&all_days);

    let cutoff = {
        let d = chrono::Utc::now() - chrono::Duration::days(365);
        d.format("%Y-%m-%d").to_string()
    };
    let daily = all_days
        .into_iter()
        .filter(|(date, _)| date.as_str() >= cutoff.as_str())
        .map(|(date, count)| DailyContribution { date, count })
        .collect();

    GitHubStats {
        total_contributions: total,
        public_repos: user["repositories"]["totalCount"].as_i64().unwrap() as i32,
        joined_at: user["createdAt"].as_str().unwrap().to_string(),
        current_streak,
        longest_streak,
        daily,
    }
}

async fn github_stats() -> axum::Json<GitHubStats> {
    let mut cache = CACHE.write().await;

    if let Some((ref data, time)) = *cache
        && time.elapsed() < TTL
    {
        return axum::Json(data.clone());
    }

    let raw = fetch_github_raw(&std::env::var("GITHUB_TOKEN").unwrap()).await;

    let stats = build_stats(raw);

    *cache = Some((stats.clone(), Instant::now()));

    axum::Json(stats)
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
