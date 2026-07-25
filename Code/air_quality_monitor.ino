#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "secrets.h"

// ===== PIN DEFINITIONS =====
#define DHTPIN D4
#define DHTTYPE DHT11
#define MQ135 A0
#define STATUS_LED D0

// ===== SENSOR OBJECTS =====
DHT dht(DHTPIN, DHTTYPE);
PulseOximeter pox;

// ===== SENSOR READINGS =====
float heartRate = 0;
float spo2 = 0;
float temperature = 0;
int gasValue = 0;

// ===== THINGSPEAK CONFIG =====
const char* server = "api.thingspeak.com";
WiFiClient client;

// ===== DATA BUFFER =====
struct SensorReading {
  float temperature;
  int gasValue;
  float heartRate;
  float spo2;
};
SensorReading buffer[50];
int bufferIndex = 0;

// ===== INITIALIZATION =====
void setup() {
  Serial.begin(115200);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  
  Serial.println("\n\nInitializing Air Quality Monitor...");
  
  // Initialize DHT11
  dht.begin();
  Serial.println("✓ DHT11 initialized");
  
  // Initialize I2C for MAX30100
  Wire.begin(D2, D1);
  Serial.println("✓ I2C initialized");
  
  // Initialize MAX30100
  if (!pox.begin()) {
    Serial.println("✗ MAX30100 FAILED - Check connections!");
  } else {
    Serial.println("✓ MAX30100 initialized");
  }
  
  // Connect to WiFi (with timeout)
  connectToWiFi();
  
  Serial.println("\n===== SYSTEM READY =====\n");
}

// ===== WIFI CONNECTION WITH TIMEOUT =====
void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int maxAttempts = 20;
  int attempts = 0;
  
  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(STATUS_LED, HIGH);
  } else {
    Serial.println("\n✗ WiFi connection timeout");
    Serial.println("Device will work in local mode (no cloud upload)");
    digitalWrite(STATUS_LED, LOW);
  }
}

// ===== ENSURE WIFI CONNECTED =====
bool ensureWiFiConnected() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  
  Serial.println("WiFi disconnected. Attempting reconnection...");
  int delayTimes[] = {5000, 10000, 20000};
  
  for (int i = 0; i < 3; i++) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
      delay(500);
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("✓ WiFi reconnected!");
      digitalWrite(STATUS_LED, HIGH);
      return true;
    }
    
    if (i < 2) {
      Serial.print("Retry in ");
      Serial.print(delayTimes[i] / 1000);
      Serial.println("s...");
      delay(delayTimes[i]);
    }
  }
  
  Serial.println("✗ WiFi reconnection failed");
  digitalWrite(STATUS_LED, LOW);
  return false;
}

// ===== VALIDATE SENSOR READING =====
bool isValidReading(float value) {
  if (isnan(value)) {
    return false;
  }
  
  if (value < -50 || value > 60) {
    return false;
  }
  
  return true;
}

// ===== SMOOTHED GAS SENSOR READING =====
int getSmoothedGasValue() {
  int readings[5];
  
  for (int i = 0; i < 5; i++) {
    readings[i] = analogRead(MQ135);
    delay(50);
  }
  
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4 - i; j++) {
      if (readings[j] > readings[j + 1]) {
        int temp = readings[j];
        readings[j] = readings[j + 1];
        readings[j + 1] = temp;
      }
    }
  }
  
  return (readings[1] + readings[2] + readings[3]) / 3;
}

// ===== CONNECT WITH TIMEOUT =====
bool connectWithTimeout(const char* server, int port, int timeoutMs = 5000) {
  unsigned long startTime = millis();
  
  while (!client.connect(server, port)) {
    if (millis() - startTime > timeoutMs) {
      Serial.println("✗ Server connection timeout");
      return false;
    }
    delay(100);
  }
  
  return true;
}

// ===== UPLOAD DATA =====
bool uploadData() {
  if (!ensureWiFiConnected()) {
    if (bufferIndex < 50) {
      buffer[bufferIndex].temperature = temperature;
      buffer[bufferIndex].gasValue = gasValue;
      buffer[bufferIndex].heartRate = heartRate;
      buffer[bufferIndex].spo2 = spo2;
      bufferIndex++;
      
      Serial.print("✓ Data buffered (");
      Serial.print(bufferIndex);
      Serial.println(" readings in buffer)");
    }
    return false;
  }
  
  if (!connectWithTimeout(server, 80)) {
    return false;
  }
  
  String url = "/update?api_key=" + String(THINGSPEAK_API) +
               "&field1=" + String(temperature) +
               "&field2=" + String(gasValue) +
               "&field3=" + String(heartRate) +
               "&field4=" + String(spo2);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: api.thingspeak.com\r\n" +
               "Connection: close\r\n\r\n");
  
  client.stop();
  
  Serial.println("✓ Data uploaded to ThingSpeak");
  return true;
}

// ===== UPLOAD BUFFERED DATA =====
void uploadBufferedData() {
  if (bufferIndex == 0 || !ensureWiFiConnected()) {
    return;
  }
  
  Serial.print("Uploading ");
  Serial.print(bufferIndex);
  Serial.println(" buffered readings...");
  
  for (int i = 0; i < bufferIndex; i++) {
    if (!connectWithTimeout(server, 80)) {
      Serial.println("Failed to upload buffered data");
      return;
    }
    
    String url = "/update?api_key=" + String(THINGSPEAK_API) +
                 "&field1=" + String(buffer[i].temperature) +
                 "&field2=" + String(buffer[i].gasValue) +
                 "&field3=" + String(buffer[i].heartRate) +
                 "&field4=" + String(buffer[i].spo2);
    
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: api.thingspeak.com\r\n" +
                 "Connection: close\r\n\r\n");
    
    client.stop();
    delay(100);
  }
  
  Serial.println("✓ Buffered data uploaded");
  bufferIndex = 0;
}

// ===== MAIN LOOP =====
void loop() {
  // ===== READ SENSORS =====
  pox.update();
  heartRate = pox.getHeartRate();
  spo2 = pox.getSpO2();
  
  temperature = dht.readTemperature();
  gasValue = getSmoothedGasValue();
  
  // ===== VALIDATE READINGS =====
  if (!isValidReading(temperature)) {
    Serial.println("✗ Temperature reading invalid");
    temperature = 0;
  }
  
  // ===== DISPLAY DATA =====
  Serial.print("\n--- Sensor Readings ---\n");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
  
  Serial.print("Gas Level: ");
  Serial.println(gasValue);
  
  Serial.print("Heart Rate: ");
  Serial.print(heartRate);
  Serial.println(" bpm");
  
  Serial.print("SpO2: ");
  Serial.print(spo2);
  Serial.println("%");
  
  // ===== UPLOAD TO CLOUD =====
  uploadData();
  
  // ===== TRY TO UPLOAD BUFFERED DATA =====
  if (bufferIndex > 0) {
    uploadBufferedData();
  }
  
  // ===== WAIT FOR NEXT CYCLE =====
  delay(15000);
}
