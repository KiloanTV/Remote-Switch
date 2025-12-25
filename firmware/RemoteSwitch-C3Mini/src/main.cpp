#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

// for C3 Mini
#define PC1_LED 5
#define PC2_LED 4
#define PC1_POWER 6
#define PC1_RESET 7
#define PC2_POWER 11
#define PC2_RESET 12

WebServer server(80);

void pressButton(int pin, int duration = 200) {
  digitalWrite(pin, HIGH);
  delay(duration);
  digitalWrite(pin, LOW);
}

void handleRoot() {
  bool pc1On = digitalRead(PC1_LED);
  bool pc2On = digitalRead(PC2_LED);
  
  String html = "<html><head><title>PC Remote Switch</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial;margin:20px;}";
  html += ".pc{border:2px solid #333;padding:15px;margin:10px 0;border-radius:8px;}";
  html += ".on{background:#d4edda;}.off{background:#f8d7da;}";
  html += "button{padding:10px 20px;margin:5px;font-size:16px;cursor:pointer;}</style></head><body>";
  
  html += "<h1>PC Remote Switch</h1>";
  
  html += "<div class='pc " + String(pc1On ? "on" : "off") + "'>";
  html += "<h2>PC 1 - " + String(pc1On ? "ON" : "OFF") + "</h2>";
  html += "<button onclick=\"location.href='/pc1/power'\">Power</button>";
  html += "<button onclick=\"location.href='/pc1/reset'\">Reset</button></div>";
  
  html += "<div class='pc " + String(pc2On ? "on" : "off") + "'>";
  html += "<h2>PC 2 - " + String(pc2On ? "ON" : "OFF") + "</h2>";
  html += "<button onclick=\"location.href='/pc2/power'\">Power</button>";
  html += "<button onclick=\"location.href='/pc2/reset'\">Reset</button></div>";
  
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  
  // Configure pins
  pinMode(PC1_LED, INPUT);
  pinMode(PC2_LED, INPUT);
  pinMode(PC1_POWER, OUTPUT);
  pinMode(PC1_RESET, OUTPUT);
  pinMode(PC2_POWER, OUTPUT);
  pinMode(PC2_RESET, OUTPUT);
  
  digitalWrite(PC1_POWER, LOW);
  digitalWrite(PC1_RESET, LOW);
  digitalWrite(PC2_POWER, LOW);
  digitalWrite(PC2_RESET, LOW);
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
  
  // Setup web routes
  server.on("/", handleRoot);
  server.on("/pc1/power", []() {
    pressButton(PC1_POWER);
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/pc1/reset", []() {
    pressButton(PC1_RESET);
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/pc2/power", []() {
    pressButton(PC2_POWER);
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/pc2/reset", []() {
    pressButton(PC2_RESET);
    server.sendHeader("Location", "/");
    server.send(303);
  });
  
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}