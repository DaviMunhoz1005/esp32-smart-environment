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

![Esquemática do Sistema](images/system_schematic.png)

</div>

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

## 🧠 Faixas de referência

| Parâmetro | OK | ALERTA | PERIGO |
|-------------|--------|------------------|------------------|
| Temperatura | 10–18°C | 8–10 ou 18–20°C | <8 ou >20°C |
| Umidade | 60–70% | 50–60 ou 70–75% | <50 ou >75% |
| Luminosidade | <33% | 33–66% | >66% |

---

## 🗂️ Estrutura do repositório

```bash
📁 esp32-smart-environment
│
├── 📄 README.md
├── 📁 sourceCode
│   └── esp32_monitoring.cpp
├── 📁 images
│   └── system_schematic.png
└── 📁 configs
    └── mymqtt_config.txt
```

---

## 👥 Integrantes do Grupo

| [<img loading="lazy" src="https://github.com/DaviMunhoz1005.png" width=115><br><sub>Davi Marques</sub>](https://github.com/DaviMunhoz1005) |  [<img loading="lazy" src="https://github.com/Gabsgc01.png" width=115><br><sub>Gabriel Ciriaco</sub>](https://github.com/Gabsgc01) | [<img loading="lazy" src="https://github.com/MariFranca.png" width=115><br><sub>Mariana Souza</sub>](https://github.com/MariFranca) | [<img loading="lazy" src="https://github.com/larissashiba.png" width=115><br><sub>Larissa Shiba</sub>](https://github.com/larissashiba) | [<img loading="lazy" src="https://github.com/Mafraaa.png" width=115><br><sub>Vinicius Mafra</sub>](https://github.com/Mafraaa) | 
| :---: | :---: | :---: | :---: | :---: |
