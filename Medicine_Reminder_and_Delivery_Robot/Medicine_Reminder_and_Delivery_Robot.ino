#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include "time.h"

//================== WiFi ==================
const char* ssid = "  ";
const char* password = "  ";

//============== Web Server (Port 80) ======
WebServer server(80);

//============== Bangladesh Time ===========
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 21600;
const int daylightOffset_sec = 0;

//============= Patient Info ===============
String patient = "  ";
String medicine = "  ";

// Alarm Time (ডিফল্ট)
int alarmHour = 16;
int alarmMinute = 34;

//============= LCD ========================
LiquidCrystal_I2C lcd(0x27, 16, 2);

//============= Servo ======================
Servo servo;

//============= Pins =======================
#define SERVO_PIN   18
#define BUZZER_PIN  19
#define RED_LED     25
#define GREEN_LED   26
#define BUTTON_PIN  27

bool alarmDone = false;
bool waitingButton = false;

// --- ওয়েবসাইট থেকে নতুন টাইম রিসিভ করার ফাংশন ---
void handleSetAlarm() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");

  if (server.hasArg("hour") && server.hasArg("min")) {
    alarmHour = server.arg("hour").toInt();
    alarmMinute = server.arg("min").toInt();
    alarmDone = false;

    Serial.printf("New Alarm Set: %02d:%02d\n", alarmHour, alarmMinute);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm Reset To:");
    lcd.setCursor(0, 1);
    char buf[16];
    sprintf(buf, "%02d:%02d", alarmHour, alarmMinute);
    lcd.print(buf);

    // ডিলয় তুলে সরাসরি রেসপন্স দিয়ে দেওয়া হলো যাতে Timeout না হয়
    server.send(200, "text/plain", "Alarm Updated Successfully");
  } else {
    server.send(400, "text/plain", "Bad Request: Missing parameters");
  }
}

// OPTIONS Preflight Handler
void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.send(200, "text/plain", "OK");
}

void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  servo.attach(SERVO_PIN);
  servo.write(0);

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Medicine Robot");
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // CORS Enable & Routes Setup
  server.enableCORS(true);
  server.on("/setAlarm", HTTP_OPTIONS, handleOptions);
  server.on("/setAlarm", HTTP_GET, handleSetAlarm);
  server.on("/setAlarm", HTTP_POST, handleSetAlarm);
  
  server.begin();
  Serial.println("Web Server Ready!");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(3000);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  lcd.clear();
  lcd.print("Time Synced");
  delay(2000);
}

void loop()
{
  server.handleClient();

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    lcd.clear();
    lcd.print("No Time");
    delay(1000);
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("Time:");

  char currentTime[9];
  sprintf(currentTime, "%02d:%02d:%02d",
          timeinfo.tm_hour,
          timeinfo.tm_min,
          timeinfo.tm_sec);

  lcd.setCursor(6, 0);
  lcd.print(currentTime);

  if (timeinfo.tm_hour == alarmHour &&
      timeinfo.tm_min == alarmMinute &&
      !alarmDone)
  {
    alarmDone = true;
    waitingButton = true;

    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);

    servo.write(90);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(patient);
    lcd.setCursor(0, 1);
    lcd.print(medicine);

    delay(3000);

    lcd.clear();
    lcd.print("Take Medicine");
  }

  if (waitingButton && digitalRead(BUTTON_PIN) == LOW)
  {
    delay(50); 

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);

    servo.write(0);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Thank You");
    lcd.setCursor(0, 1);
    lcd.print("Get Well Soon");

    delay(3000);

    digitalWrite(GREEN_LED, LOW);
    waitingButton = false;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waiting...");
  }

  if (timeinfo.tm_hour != alarmHour) {
    alarmDone = false;
  }

  delay(10);
}