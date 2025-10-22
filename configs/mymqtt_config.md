# 🧠 Guia de Teste MQTT – ESP32 Monitoramento de Ambiente

Este documento descreve como configurar, conectar e testar os tópicos MQTT usados pelo código do ESP32 que monitora **temperatura**, **umidade** e **luminosidade**, além de permitir o **controle remoto dos LEDs e do buzzer**.

---

## ⚙️ 1. Configuração do Broker MQTT

O ESP32 foi configurado para se conectar ao seguinte broker:

| Parâmetro       | Valor           |
|-----------------|-----------------|
| **Servidor**    | `{PUBLIC_IP_VM}`   |
| **Porta**       | `1883`          |
| **Cliente ID**  | `ESP32_Placa`   |

Certifique-se de que o broker esteja **ativo** e **acessível publicamente**.  
Você pode usar ferramentas como [MQTT Explorer](https://mqtt-explorer.com/), `mosquitto_pub`, `mosquitto_sub` ou o painel do **FIWARE / Node-RED / HiveMQ Web Client** para testar.

---

## 📡 2. Tópicos MQTT Usados

### 🧾 Publicação (dados enviados pelo ESP32)

| Tópico                        | Exemplo de Payload |
|-------------------------------|--------------------|
| `esp32/ambiente/dados`        | `{ "temperatura": 19.5 Cº , "umidade": 62.3% , "luminosidade": 41%, "estado": "OK" }` |

Esses dados são publicados automaticamente a cada ciclo de leitura (2 segundos, conforme `delayMS`).

---

### 🎛️ Assinatura (comandos recebidos pelo ESP32)

O ESP32 escuta os seguintes tópicos e executa ações conforme o payload recebido:

| Tópico | Payload Aceito | Ação Executada |
|--------|----------------|----------------|
| `esp32/ambiente/buzzer` | `ON` | Toca o buzzer por 2 segundos |
| `esp32/ambiente/led_ok` | `ON` / `OFF` | Liga/Desliga o LED Verde |
| `esp32/ambiente/led_alert` | `ON` / `OFF` | Liga/Desliga o LED Amarelo |
| `esp32/ambiente/led_danger` | `ON` / `OFF` | Liga/Desliga o LED Vermelho |

> ⚠️ Observação: os LEDs também são controlados automaticamente pelo sistema de monitoramento.  
> Caso queira impedir que os comandos automáticos sobrescrevam os manuais, será necessário adicionar um **modo manual/automático** no código.

---

## 🧠 4. Diagnóstico de Conexão

Durante a inicialização, o monitor serial exibirá mensagens como:

```bash

Conectando ao WiFi...
WiFi conectado!
IP: 192.168.0.105
Tentando conectar MQTT... Conectado!
Publicado com sucesso: {...}

```

Se aparecer algo como:

```bash

Falhou, rc=-2 Tentando novamente em 3s

```

significa que o ESP32 não conseguiu alcançar o broker MQTT — verifique:

- Endereço IP (mqtt_server)
- Porta (1883)
- Conectividade com a internet/Wi-Fi