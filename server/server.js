const express = require("express");
const http = require("http");
const WebSocket = require("ws");
const mqtt = require("mqtt");
const path = require("path");

const MQTT_HOST = "mqtt://18.234.110.71:1883"; // PUBLIC_IP_VM_BROKER
const MQTT_TOPIC = "esp32/ambiente/dados";

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

app.use(express.static(path.join(__dirname, "../public")));

wss.on("connection", (ws) => {
    console.log("Cliente conectado via WebSocket!");
    ws.send(JSON.stringify({ info: "Aguardando dados do ESP32..." }));
});

const client = mqtt.connect(MQTT_HOST);

client.on("connect", () => {
    console.log("Conectado ao broker MQTT!");
    client.subscribe(MQTT_TOPIC, (err) => {
        if (!err) console.log(`Inscrito no tópico: ${MQTT_TOPIC}`);
    });
});

client.on("message", (topic, message) => {
    const msg = message.toString();
    console.log("Tópico:", topic);
    console.log("Payload:", msg);

    wss.clients.forEach((ws) => {
        if (ws.readyState === WebSocket.OPEN) {
            ws.send(msg);
        }
    });
});

const PORT = 3000;
server.listen(PORT, () => {
    console.log(`🌐 Servidor rodando em http://localhost:${PORT}`);
});
