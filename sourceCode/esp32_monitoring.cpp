#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>

#include <cstring>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "18.234.110.71";
const int mqtt_port = 1883;
const char* mqtt_topic_publish = "esp32/ambiente/dados";

WiFiClient espClient;
PubSubClient client(espClient);

#define LED_DANGER 32
#define LED_ALERT 33
#define LED_OK 25

#define PIEZO 27
unsigned long lastPiezoTime = 0;
bool piezoOn = false;

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

String getStateName(State state) {
  switch (state) {
    case STATE_OK: return "OK";
    case STATE_ALERT: return "ALERTA";
    case STATE_DANGER: return "PERIGO";
    default: return "DESCONHECIDO";
  }
}

State state;

char parameterInAlert[3][15];
int totalWordsAlert = 0;
char parameterInDanger[3][15];
int totalWordsDanger = 0;

void setup_wifi() {
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Tentando conectar MQTT...");
    if (client.connect("ESP32_Placa")) {
      Serial.println("Conectado!");
      client.subscribe("esp32/ambiente/buzzer");
      client.subscribe("esp32/ambiente/led_ok");
      client.subscribe("esp32/ambiente/led_alert");
      client.subscribe("esp32/ambiente/led_danger");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 3s");
      delay(3000);
    }
  }
}

void publicarDados() {
  String payload = "{";
  payload += "\"status\": " + String((int)state) + ",";
  payload += "\"state\": \"" + getStateName(state) + "\",";
  payload += "\"temperature\": " + String(temperature, 2) + ",";
  payload += "\"humidity\": " + String(humidity, 2) + ",";
  payload += "\"luminosity\": " + String(luminosityMapped) + ",";
  payload += "\"alert\": [";
  for (int i = 0; i < totalWordsAlert; i++){
    payload += "\"" + String(parameterInAlert[i]) + "\"";
    if (i < totalWordsAlert - 1) payload += ",";
  }
  payload += "],";
  payload += "\"danger\": [";
  for (int i = 0; i < totalWordsDanger; i++){
    payload += "\"" + String(parameterInDanger[i]) + "\"";
    if (i < totalWordsDanger - 1) payload += ",";
  }
  payload += "]";
  payload += "}";

  if (client.publish(mqtt_topic_publish, payload.c_str())){
    Serial.print("Publicado com sucesso: ");
    Serial.println(payload);
  } else {
    Serial.println("Falha ao publicar no MQTT.");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();

  Serial.print("Comando recebido no tópico [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (String(topic) == "esp32/ambiente/buzzer") {
    if (message == "ON") {
      Serial.println("🔊 Tocando buzina via MQTT...");
      playPiezo(2000);
    }
  }

  else if (String(topic) == "esp32/ambiente/led_ok") {
    digitalWrite(LED_OK, message == "ON" ? HIGH : LOW);
  }

  else if (String(topic) == "esp32/ambiente/led_alert") {
    digitalWrite(LED_ALERT, message == "ON" ? HIGH : LOW);
  }

  else if (String(topic) == "esp32/ambiente/led_danger") {
    digitalWrite(LED_DANGER, message == "ON" ? HIGH : LOW);
  }
}

void addWord(char arr[][15], int &total, const char* parameter) {
  if (total >= 3) {
    Serial.println("Erro: array cheio");
  }
  if (strlen(parameter) >= 15) {
    Serial.println("Erro: palavra muito longa");
  }
  strcpy(arr[total], parameter);
  total++;
}

bool containsWord(char arr[][15], int total, const char* parameter) {
  for (int i = 0; i < total; i++) {
    if (strcmp(arr[i], parameter) == 0) return true;
  }
  return false;
}

void removeWord(char arr[][15], int &total, const char* parameter) {
  for (int i = 0; i < total; i++) {
    if (strcmp(arr[i], parameter) == 0) {
      for (int j = i; j < total-1; j++) {
        strcpy(arr[j], arr[j+1]);
      }
      total--;
      arr[total][0] = '\0'; // opcional
    }
  }
}

void clearArray(char arr[][15], int &total) {
  for (int i = 0; i < total; i++) {
    arr[i][0] = '\0';  
  }
  total = 0;           
}

void addWordSmart(char targetArr[][15], int &targetTotal,
                  char otherArr[][15], int &otherTotal,
                  const char* parameter) {

  if (containsWord(targetArr, targetTotal, parameter)) {
    Serial.println("Palavra já existe no array alvo. Não será adicionada.");
  }
  if (containsWord(otherArr, otherTotal, parameter)) {
    removeWord(otherArr, otherTotal, parameter);
    Serial.println("Palavra removida do outro array.");
  }
  addWord(targetArr, targetTotal, parameter);
}

bool removeWordSmart(char targetArr[][15], int &targetTotal,
                    char otherArr[][15], int &otherTotal,
                    const char* parameter, bool isDanger) {
  
  if (isDanger) {
    addWordSmart(targetArr, targetTotal,
             otherArr, otherTotal,
             parameter);
    return true;
  }
  else if (containsWord(targetArr, targetTotal, parameter)) {
    removeWord(targetArr, targetTotal, parameter);
  }
  return false;
}

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

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  checkState();
}

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  checkState();

  switch(state) {
    case STATE_OK:
      turnLEDsOnAndOff(0, 0, 1);
      noTone(PIEZO);
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
}

void checkState() {
  getMappedLuminosity();
  getTemperature();
  getHumidity();

  clearArray(parameterInAlert, totalWordsAlert); 
  clearArray(parameterInDanger, totalWordsDanger);

  bool isAlert = isInAlert();
  bool isDanger = isInDanger();

  if (isAlert) { 
    state = STATE_ALERT;
  } 
  if (isDanger) { 
    state = STATE_DANGER;
  } else if (!isAlert && !isDanger) { 
    state = STATE_OK;
    displayOKMessages();
  }

  delay(1000);
  publicarDados();
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

  if (removeWordSmart(parameterInDanger, totalWordsDanger,
      parameterInAlert, totalWordsAlert,
      "luminosity", (luminosityMapped > 66))) {

    title = "Luz ALTA";
    value = "Luz = " + String(luminosityMapped) + "%";
  } else if (removeWordSmart(parameterInDanger, totalWordsDanger,
      parameterInAlert, totalWordsAlert,
      "temperature", (temperature < 8))) {
        
    title = "Temp. BAIXA";
    value = "Temp. = " + String(temperature, 1) + "C";
  } else if (removeWordSmart(parameterInDanger, totalWordsDanger,
      parameterInAlert, totalWordsAlert,
      "temperature", (temperature > 20))) {

    title = "Temp. ALTA";
    value = "Temp. = " + String(temperature, 1) + "C";
  } else if (removeWordSmart(parameterInDanger, totalWordsDanger,
      parameterInAlert, totalWordsAlert,
      "humidity", (humidity < 50))) {

    title = "Umidade BAIXA";
    value = "Umi. = " + String(humidity, 1) + "%";
  } else if (removeWordSmart(parameterInDanger, totalWordsDanger,
      parameterInAlert, totalWordsAlert,
      "humidity", (humidity > 75))) {

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

  if (removeWordSmart(parameterInAlert, totalWordsAlert,
      parameterInDanger, totalWordsDanger,
      "luminosity", (luminosityMapped > 33 && luminosityMapped <= 66))) {

    title = "Luz MODERADA";
    value = "Luz = " + String(luminosityMapped) + "%";
  } else if (removeWordSmart(parameterInAlert, totalWordsAlert,
      parameterInDanger, totalWordsDanger,
      "temperature", (temperature >= 8 && temperature <= 10))) {

    title = "Temp. MOD. BAIXA";
    value = "Temp. = " + String(temperature, 1) + "C";
  } else if (removeWordSmart(parameterInAlert, totalWordsAlert,
      parameterInDanger, totalWordsDanger,
      "temperature", (temperature >= 18 && temperature <= 20))) {

    title = "Temp. MOD. ALTA";
    value = "Temp. = " + String(temperature, 1) + "C";
  } else if (removeWordSmart(parameterInAlert, totalWordsAlert,
      parameterInDanger, totalWordsDanger,
      "humidity", (humidity >= 50 && humidity <= 60))) {

    title = "Umi. MOD. BAIXA";
    value = "Umi. = " + String(humidity, 1) + "%";
  } else if (removeWordSmart(parameterInAlert, totalWordsAlert,
      parameterInDanger, totalWordsDanger,
      "humidity", (humidity >= 70 && humidity <= 75))) {

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
  unsigned long now = millis();

  if (!piezoOn && now - lastPiezoTime >= 5000) {
    tone(PIEZO, 1500);
    piezoOn = true;
    lastPiezoTime = now;
  }
  else if (piezoOn && now - lastPiezoTime >= 2000) {
    noTone(PIEZO);
    piezoOn = false;
    lastPiezoTime = now;
  }
}

void soundDanger() {
  unsigned long now = millis();

  if (!piezoOn && now - lastPiezoTime >= 300) {
    tone(PIEZO, 1500);
    piezoOn = true;
    lastPiezoTime = now;
  }
  else if (piezoOn && now - lastPiezoTime >= 300) {
    noTone(PIEZO);
    piezoOn = false;
    lastPiezoTime = now;
  }
}

void playPiezo(int milliseconds) {
  tone(PIEZO, 1500);
  delay(milliseconds);         
  noTone(PIEZO);    
}