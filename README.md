# 🌡️ Sistema de Monitoramento Ambiental com ESP32

Projeto desenvolvido para monitorar **temperatura, umidade e luminosidade** em tempo real utilizando o **ESP32**, exibindo os valores em um display **LCD I2C** e acionando **LEDs** e **buzzer piezo** conforme o estado do ambiente (OK, ALERTA ou PERIGO).

---

## 🧠 Objetivo

Criar um sistema capaz de:

- Detectar variações ambientais críticas.
- Exibir leituras em um display LCD.
- Acionar alarmes visuais (LEDs) e sonoros (Piezo).
- Demonstrar integração entre sensores e atuadores no ESP32.

---

## ⚙️ Componentes utilizados

| Componente | Função | Pinos utilizados |
|-------------|--------|------------------|
| ESP32 | Microcontrolador principal | — |
| DHT22 | Sensor de temperatura e umidade | GPIO 16 |
| LDR + resistor (10kΩ) | Sensor de luminosidade | GPIO 34 |
| LCD I2C (16x2) | Exibição das medições | SDA/SCL (I2C) |
| LED Verde | Indica ambiente OK | GPIO 25 |
| LED Amarelo | Indica alerta | GPIO 33 |
| LED Vermelho | Indica perigo | GPIO 32 |
| Buzzer Piezo | Alerta sonoro | GPIO 35 |

---

## 🧩 Ligações elétricas (esquemático)

<div align="center">
  
<img src="https://cdn.statically.io/gh/DaviMunhoz1005/esp32-smart-environment/main/images/system_schematic.png" alt="System Schematic" width="600">

</div>

## 💻 Simulação Wokwi

Para testar o projeto sem precisar da VM ou hardware físico, você pode usar a simulação no **Wokwi**:  
[Link da Simulação Wokwi](https://wokwi.com/projects/444923253527449601)

---

## 🧠 Função dos Componentes

| Componente | Função |
|-------------|--------|
| **ESP32** | Unidade principal. Faz leitura dos sensores, controla LEDs, buzina e envia dados via Wi-Fi/MQTT. |
| **DHT22** | Mede a temperatura (°C) e umidade relativa do ar (%). |
| **LDR** | Detecta a luminosidade do ambiente e converte em percentual. |
| **LCD I2C 16x2** | Mostra os valores lidos e mensagens de status (“OK”, “ALERTA”, “PERIGO”). |
| **Buzzer (Piezo)** | Emite sons de alerta quando há risco ambiental. |
| **LEDs** | Indicadores visuais do estado do ambiente. |

---

## ⚙️ Recursos Necessários

### 🧩 Hardware (Simulação no Wokwi)
- ESP32 Dev Module  
- Sensor DHT22  
- LDR com resistor de pull-down  
- Buzzer  
- LEDs (vermelho, amarelo, verde)  
- Display LCD I2C  
- Protoboard + Jumpers  

### 💡 Hardware (Físico)
- Mesmo da simulação  
- Cabo USB para alimentação e programação  
- Fonte de 5V (opcional para operação autônoma)

### 💻 Software / Ferramentas

- **Arduino IDE** (ou PlatformIO)
- **Bibliotecas**:
  - `WiFi.h`
  - `PubSubClient.h`
  - `DHT.h`
  - `DHT_U.h`
  - `LiquidCrystal_I2C.h`
- **Broker MQTT** (ex: Mosquitto, HiveMQ ou FIWARE)
- **MQTT Explorer** ou **mosquitto_pub/sub** (para testes)

---

## 📡 Configuração MQTT

| Parâmetro | Valor |
|------------|--------|
| **Broker** | `{PUBLIC_IP_VM}` |
| **Porta** | `1883` |
| **Tópico de publicação** | `esp32/ambiente/dados` |
| **Tópicos de comando** | `esp32/ambiente/buzzer`, `esp32/ambiente/led_ok`, `esp32/ambiente/led_alert`, `esp32/ambiente/led_danger` |

### 🔄 Exemplo de Mensagem Publicada

```json
{
  "status": 3,
  "state": "DANGER",
  "temperature": 22,
  "humidity": 60.00,
  "luminosity": 90,
  "alert": ["temperature"],
  "danger": ["luminosity"]
}

```

---

## 💻 Código-fonte

O código principal está no arquivo:
[👉 Visualizar código no GitHub](sourceCode/esp32_monitoring.cpp)

---

## 📚 Principais bibliotecas

```cpp
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
```

Instale-as na IDE do Arduino via Gerenciador de Bibliotecas.

---

## 🌐 Frontend do Sistema de Monitoramento

O frontend permite visualizar temperatura, umidade, luminosidade e estado do ambiente em tempo real diretamente no navegador, utilizando WebSocket para receber os dados do ESP32 via Node.js/MQTT.

## 🖥️ Funcionalidades principais

- Receber JSON publicado pelo ESP32 (`esp32/ambiente/dados`) através do servidor Node.js.
- Atualizar valores de temperatura, umidade e luminosidade na tela em tempo real.
- Exibir estado atual (OK, ALERTA ou PERIGO) com cores e ícones.
- Histórico visual rápido de alertas e perigo (opcional, dependendo do frontend).
- Interface limpa e responsiva para monitoramento remoto.

## 💻 Tecnologias utilizadas

- HTML/CSS/JS – Estrutura e estilização.
- WeSocket API – Conexão em tempo real entre Node.js e navegador.
- Chart.js (opcional) – Para gráficos de histórico em tempo real.
- Node.js + express + ws + mqtt – Backend que faz ponte MQTT → WebSocket.

## 🔗 Estrutura simplificada do frontend

<div align="center">
  
<img src="https://cdn.statically.io/gh/DaviMunhoz1005/esp32-smart-environment/main/images/frontend_charts.png" alt="Frontend Charts" width="600">

</div>

## 🔧 Integração com o Node.js

- O Node.js assina o tópico MQTT do ESP32 (esp32/ambiente/dados) e repassa o JSON via WebSocket para todos os clientes conectados.
- Se o frontend estiver aberto no navegador, ele atualiza os dados em tempo real, sem precisar recarregar a página.

<div align="center">
  
<img src="https://cdn.statically.io/gh/DaviMunhoz1005/esp32-smart-environment/main/images/terminal_integrate.png" alt="Terminal Integrate" width="600">

</div>

## 🔄 Observações importantes

- Certifique-se que broker MQTT, Node.js e ESP32 estão na mesma rede ou acessíveis via IP público.
- O frontend se conecta ao WebSocket usando o endereço do servidor Node.js, normalmente ws://<IP_DO_SERVIDOR>:3000.
- O ESP32 publica JSON com campos: `status`, `state`, `temperature`, `humidity`, `luminosity`, `alert` e `danger`.

---

## 🚦 Lógica de funcionamento

| Estado | Condições | Ações |
|-------------|--------|------------------|
| OK (LED Verde) | LDR < 33, Temp 10–18°C, Umid 60–70% | Exibe mensagens de status OK no LCD |
| ALERTA (LED Amarelo) | Valores próximos ao limite | Liga buzzer 3s e alerta no LCD |
| PERIGO (LED Vermelho) | Valores fora da faixa padrão | Buzzer constante e alerta no LCD |

---

## 🔁 Fluxo de execução

1. Leitura dos sensores (DHT e LDR)
2. Conversão dos valores (map)
3. Verificação do estado (OK, ALERTA, PERIGO)
4. Ação correspondente:
    - LEDs acesos
    - LCD atualiza mensagem
    - Piezo emite som, se necessário

---

## 🗂️ Estrutura do repositório

```bash
📁 esp32-smart-environment
│
├── 📄 README.md
├── 📄.gitignore
├── 📁 public
│   ├── 📁 src
│   │   └── 📁 js
│   │       └── script.js
│   └── index.html
├── 📁 sourceCode
│   └── esp32_monitoring.cpp
│   📁 server
│   ├── server.js
│   ├── package.json
│   └── package-lock.json
├── 📁 images
│   ├── terminal_integrate.png
│   ├── system_schematic.png
│   └── frontend_charts.png
└── 📁 configs
    └── mymqtt_config.txt
```

---

## 👥 Integrantes do Grupo

| [<img loading="lazy" src="https://github.com/DaviMunhoz1005.png" width=115><br><sub>Davi Marques</sub>](https://github.com/DaviMunhoz1005) |  [<img loading="lazy" src="https://github.com/Gabsgc01.png" width=115><br><sub>Gabriel Ciriaco</sub>](https://github.com/Gabsgc01) | [<img loading="lazy" src="https://github.com/MariFranca.png" width=115><br><sub>Mariana Souza</sub>](https://github.com/MariFranca) | [<img loading="lazy" src="https://github.com/larissashiba.png" width=115><br><sub>Larissa Shiba</sub>](https://github.com/larissashiba) | [<img loading="lazy" src="https://github.com/Mafraaa.png" width=115><br><sub>Vinicius Mafra</sub>](https://github.com/Mafraaa) | 
| :---: | :---: | :---: | :---: | :---: |
