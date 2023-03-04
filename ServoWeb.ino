#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// Access point credentials
const char* ap_ssid = "ESP8266_AP";
const char* ap_password = "password";

// Web server
ESP8266WebServer server(80);

// Servo
Servo myservo;
int servo_pos = 0;
const int servo_pin = D1;

void handleRoot() {
  String html = "<h1>Hello from ESP8266!</h1>";
  html += "<p>Servo position: " + String(servo_pos) + "</p>";
  html += "<form method='post' action='/position'>";
  html += "<label for='pos'>Position (0-180):</label>";
  html += "<input type='number' id='pos' name='pos' min='0' max='180' value='" + String(servo_pos) + "'>";
  html += "<input type='submit' value='Submit'>";
  html += "</form>";
  server.send(200, "text/html", html);
}

void handlePosition() {
  if (server.method() == HTTP_POST) {
    String pos_param = server.arg("pos");
    if (pos_param != "") {
      int pos = pos_param.toInt();
      if (pos >= 0 && pos <= 180) {
        servo_pos = pos;
        myservo.write(servo_pos);
        server.send(200, "text/plain", "OK");
        return;
      }
    }
    server.send(400, "text/plain", "Bad Request");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void setup() {
  Serial.begin(9600);

  // Start access point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Access point started");
  Serial.print("SSID: ");
  Serial.println(ap_ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Start web server
  server.on("/", handleRoot);
  server.on("/position", handlePosition);
  server.begin();

  // Initialize servo
  myservo.attach(servo_pin);
  myservo.write(servo_pos);
  Serial.println("Servo initialized");
}

void loop() {
  server.handleClient();
}
