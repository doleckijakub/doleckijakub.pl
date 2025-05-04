const ctx = document.getElementById('usageChart').getContext('2d');

const data = {
    labels: [],
    datasets: [
    {
        label: 'CPU Usage (%)',
        data: [],
        borderColor: 'blue',
        fill: true,
        tension: 0.1
    },
    {
        label: 'Memory Usage (%)',
        data: [],
        borderColor: 'red',
        fill: true,
        tension: 0.1
    }
    ]
};

const config = {
    type: 'line',
    data: data,
    options: {
    responsive: true,
    animation: false,
    scales: {
        y: {
        min: 0,
        max: 100,
        title: {
            display: true,
            text: 'Usage (%)'
        }
        },
        x: {
        title: {
            display: true,
            text: 'Time'
        }
        }
    }
    }
};

const usageChart = new Chart(ctx, config);

async function fetchDataAndUpdateChart() {
    try {
    const [cpuRes, memUsageRes, memTotalRes] = await Promise.all([
        fetch('/api/cpu_usage'),
        fetch('/api/mem_usage_kb'),
        fetch('/api/mem_total_kb')
    ]);

    const cpuUsage = await cpuRes.text();
    const memUsageKb = parseFloat(await memUsageRes.text());
    const memTotalKb = parseFloat(await memTotalRes.text());

    const memUsagePercent = (memUsageKb / memTotalKb) * 100;

    const timeLabel = new Date().toLocaleTimeString();

    if (data.labels.length > 60) {
        data.labels.shift();
        data.datasets[0].data.shift();
        data.datasets[1].data.shift();
    }

    data.labels.push(timeLabel);
    data.datasets[0].data.push(parseFloat(cpuUsage));
    data.datasets[1].data.push(memUsagePercent);

    usageChart.update();
    } catch (err) {
    console.error('Error fetching usage data:', err);
    }
}

setInterval(fetchDataAndUpdateChart, 1000);