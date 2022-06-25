#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

const char *ssid = "ssid"; // Ubah SSID dengan SSID wifi anda
const char *password = "password"; // Ubah password dengan password wifi anda
const char *mqtt_server = "152.69.200.232";
WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x3F, 16, 2);
DHT dht(D5, DHT11);
unsigned long previousDHT = 0, previousSend = 0;
char buff[33];
float t, h;
byte tempChar[] = {0x0E, 0x0A, 0x0A, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E};
byte humidChar[] = {0x00, 0x04, 0x0A, 0x11, 0x1F, 0x1F, 0x0E, 0x00};

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("  Connecting to ");
    lcd.setCursor(0, 1);
    lcd.print(ssid);
    delay(250);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   CONNECTED!   ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP().toString());
  delay(2000);
  lcd.clear();
}

void reconnect() {
  while (!client.connected()) {
    lcd.setCursor(0, 0);
    lcd.print("Connecting MQTT");
    if (client.connect("kelompok6")) {
      lcd.setCursor(0, 1);
      lcd.print("   CONNECTED!   ");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("     FAILED!    ");
    }
    delay(500);
    lcd.clear();
  }
}

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, tempChar);
  lcd.createChar(2, humidChar);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop()) {
    client.connect("kelompok6");
  }
  unsigned long currentDHT = millis();
  if (currentDHT - previousDHT >= 2000) {
    previousDHT = currentDHT;
    h = dht.readHumidity();
    t = dht.readTemperature();
  }
  lcd.setCursor(0, 0);
  lcd.write(1);
  dtostrf(t, 2, 1, buff);
  lcd.setCursor(2, 0);
  lcd.print(buff);
  lcd.setCursor(6, 0);
  lcd.print("\xDF" "C");
  lcd.setCursor(0, 1);
  lcd.write(2);
  dtostrf(h, 2, 1, buff);
  lcd.setCursor(2, 1);
  lcd.print(buff);
  lcd.setCursor(6, 1);
  lcd.print("%");

  unsigned long currentSend = millis();
  if (currentSend - previousSend >= 1000) {
    previousSend = currentSend;
    char tempSuhu[8];
    dtostrf(t, 1, 2, tempSuhu);
    client.publish("kel6/suhu", tempSuhu);

    char tempHumidity[8];
    dtostrf(h, 1, 2, tempHumidity);
    client.publish("kel6/kelembapan", tempHumidity);
  }
}