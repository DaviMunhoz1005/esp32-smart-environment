#include <Adafruit_Sensor.h>                      
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 

#define LED_DANGER 32
#define LED_ALERT 33
#define LED_OK 25

#define PIEZO 35

#define LDR 34
int luminosity;
int luminosityMapped;
int minValue = 32; // Mudar de acordo com valor mínimo
int maxValue = 4063; // Mudar de acordo com valor Máximo

#define DHTTYPE DHT22  // Mudar de acordo com o sensor 11 ou 22
#define DHTPIN 16                                   
DHT_Unified dht(DHTPIN, DHTTYPE);
float temperature;
float humidity;
sensors_event_t event;
uint32_t delayMS = 2000;

#define col 16 
#define lin 2 
#define ende 0x27 

LiquidCrystal_I2C lcd(ende,col,lin); 
unsigned long lastDisplayTime = 0;
int currentMessageIndex = 0;

enum State {
  STATE_OK,
  STATE_ALERT,
  STATE_DANGER
};

State state;

void setup() {
  Serial.begin(9600);

  pinMode(LED_DANGER, OUTPUT);
  pinMode(LED_ALERT, OUTPUT);
  pinMode(LED_OK, OUTPUT);
  
  pinMode(PIEZO, OUTPUT);
  pinMode(LDR, INPUT);
  dht.begin();   
  lcd.init(); 
  lcd.backlight(); 
  lcd.clear(); 

  checkState();
}

void loop() {
  delay(delayMS);
  switch(state) {
    case STATE_OK:
      turnLEDsOnAndOff(0, 0, 1);
      break;
    case STATE_ALERT:
      turnLEDsOnAndOff(0, 1, 0);
      soundAlert();
      break; 
    case STATE_DANGER:
      turnLEDsOnAndOff(1, 0, 0);
      soundDanger();
      break;
  }
  checkState();
}

void checkState() {
  getMappedLuminosity();
  getTemperature();
  getHumidity();
  if (isInDanger()) { 
    state = STATE_DANGER;
  } else if (isInAlert()) { 
    state = STATE_ALERT;
  } else { 
    state = STATE_OK;
    displayOKMessages();
  }
}

void getMappedLuminosity() {
  luminosity = analogRead(LDR);
  luminosityMapped = map(luminosity, minValue, maxValue, 0, 100); 

  Serial.print("Luminosidade Antes: ");
  Serial.print(luminosity);
  Serial.print(" | Depois: ");
  Serial.println(luminosityMapped);
}

void getTemperature() {
  dht.temperature().getEvent(&event);
  temperature = event.temperature;
  if (isnan(temperature)) {               
    Serial.println("Erro na leitura da Temperatura!");
  }
  else {                                        
    Serial.print("Temperatura: ");              
    Serial.print(temperature);
    Serial.println(" *C");
  }
}

void getHumidity() {
  dht.humidity().getEvent(&event);
  humidity = event.relative_humidity;
  if (isnan(humidity)) {           
    Serial.println("Erro na leitura da Umidade!");
  }
  else { 
    Serial.print("Umidade: ");                  
    Serial.print(humidity);
    Serial.println("%");
  }
}

boolean isInDanger() {
  String title = "";
  String value = "";

  if (luminosityMapped > 66) {
    title = "Luz ALTA";
    value = "Luz = " + String(luminosityMapped) + "%";
  } else if (temperature < 8) {
    title = "Temp. BAIXA";
    value = "Temp. = " + String(temperature, 1) + "C";
  } else if (temperature > 20) {
    title = "Temp. ALTA";
    value = "Temp. = " + String(temperature, 1) + "C";
  } else if (humidity < 50) {
    title = "Umidade BAIXA";
    value = "Umi. = " + String(humidity, 1) + "%";
  } else if (humidity > 75) {
    title = "Umidade ALTA";
    value = "Umi. = " + String(humidity, 1) + "%";
  }

  if (title != "") {
    displayTwoLines(title, value);
    return true;
  }

  return false;
}

boolean isInAlert() {
  String title = "";
  String value = "";

  if (luminosityMapped > 33 && luminosityMapped <= 66) {
    title = "Luz MODERADA";
    value = "Luz = " + String(luminosityMapped) + "%";
  } else if (temperature >= 8 && temperature <= 10) {
    title = "Temp. MOD. BAIXA";
    value = "Temp. = " + String(temperature, 1) + "C";
  } else if (temperature >= 18 && temperature <= 20) {
    title = "Temp. MOD. ALTA";
    value = "Temp. = " + String(temperature, 1) + "C";
  } else if (humidity >= 50 && humidity <= 60) {
    title = "Umi. MOD. BAIXA";
    value = "Umi. = " + String(humidity, 1) + "%";
  } else if (humidity >= 70 && humidity <= 75) {
    title = "Umi. MOD. ALTA";
    value = "Umi. = " + String(humidity, 1) + "%";
  }

  if (title != "") {
    displayTwoLines(title, value);
    return true;
  }

  return false;
}

void turnLEDsOnAndOff(int ledDanger, int ledAlert, int ledOk) {
  digitalWrite(LED_DANGER, ledDanger);
  digitalWrite(LED_ALERT, ledAlert);
  digitalWrite(LED_OK, ledOk);
}

void displayTwoLines(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void displayOKMessages() {
  unsigned long now = millis();
  const unsigned long interval = 5000;

  if (now - lastDisplayTime >= interval) {
    lastDisplayTime = now;

    switch (currentMessageIndex) {
      case 0:
        displayTwoLines("Temperatura OK", "Temp. = " + String(temperature, 1));
        checkState();
        break;
      case 1:
        displayTwoLines("Umidade OK", "Umi. = " + String(humidity, 1) + "%");
        checkState();
        break;
      case 2:
        displayTwoLines("Luz OK", "Luz = " + String(luminosityMapped) + "%");
        checkState();
        break;
    }
    
    currentMessageIndex = (currentMessageIndex + 1) % 3;
  }
}

void soundAlert() {
  playPiezo(3000);
  unsigned int startTime = millis();
  while (millis() - startTime < 5000) {
    checkState();
    if (state != STATE_ALERT) {
      return;
    }
  }
}

void soundDanger() {
  while (state == STATE_DANGER) {
    playPiezo(1000);
    checkState();
  }
}

void playPiezo(int milliseconds) {
  tone(PIEZO, 1500);
  delay(milliseconds);         
  noTone(PIEZO);    
}