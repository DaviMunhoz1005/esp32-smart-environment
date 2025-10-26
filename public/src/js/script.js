const ws = new WebSocket(`ws://${globalThis.location.hostname}:3000`);

const chartConfig = (label, color) => ({
    type: "line",
    data: {
        labels: [],
        datasets: [{
            label,
            data: [],
            borderColor: color,
            borderWidth: 2,
            fill: false,
            tension: 0.2,
        }]
    },
    options: {
        scales: {
            y: { beginAtZero: true },
            x: { display: false }
        }
    }
});

const chartTemp = new Chart(document.getElementById("chartTemp"), chartConfig("Temperatura", "rgb(59,130,246)"));
const chartHum  = new Chart(document.getElementById("chartHum"),  chartConfig("Umidade", "rgb(34,197,94)"));
const chartLum  = new Chart(document.getElementById("chartLum"),  chartConfig("Luminosidade", "rgb(250,204,21)"));

const estadoAtual = document.getElementById("estadoAtual");

ws.onopen = () => console.log("✅ Conectado ao WebSocket!");
ws.onerror = err => console.error("Erro no WebSocket:", err);

ws.onmessage = (event) => {
    try {
        const data = JSON.parse(event.data);
        if (!data.temperature || !data.humidity) return;

        const now = new Date().toLocaleTimeString();

        const getColor = (param) => {
            if (data.danger.includes(param)) return "rgb(220,38,38)";
            if (data.alert.includes(param)) return "rgb(234,179,8)";
            return "rgb(34,197,94)";
        };

        updateChart(chartTemp, now, Number.parseFloat(data.temperature), getColor("temperature"));
        updateChart(chartHum, now, Number.parseFloat(data.humidity), getColor("humidity"));
        updateChart(chartLum, now, Number.parseFloat(data.luminosity), getColor("luminosity"));

        estadoAtual.textContent = data.state || "—";

        if (data.state === "OK") estadoAtual.className = "text-3xl font-bold text-green-400";
        else if (data.state === "ALERTA") estadoAtual.className = "text-3xl font-bold text-yellow-400 animate-pulse";
        else if (data.state === "PERIGO") estadoAtual.className = "text-3xl font-bold text-red-500 animate-bounce";

    } catch (e) {
        console.error("Erro ao processar mensagem:", e);
    }
};

function updateChart(chart, label, value, color) {
    const data = chart.data;
    data.labels.push(label);
    data.datasets[0].data.push(value);

    data.datasets[0].borderColor = color;

    if (data.labels.length > 10) {
        data.labels.shift();
        data.datasets[0].data.shift();
    }

    chart.update();
}
