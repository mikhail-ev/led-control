#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "WLAN1-797A66"
#define STAPSK  "eJnffBL5n0L0Fq99"
#endif

#define R_LED D1
#define G_LED D2
#define B_LED D3
#define BUILDIN_LED 2

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

void handleUpdate() {
  digitalWrite(BUILDIN_LED, LOW);
  setColors(server.arg("r").toInt(),
            server.arg("g").toInt(),
            server.arg("b").toInt());
  server.send(200, "text/plain", "Colors set!");
  digitalWrite(BUILDIN_LED, HIGH);
}

void handleNotFound() {
  digitalWrite(BUILDIN_LED, LOW);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(BUILDIN_LED, HIGH);
}

int covertResolution(int value) {
  if (value < 0) {
    return 0;
  }
  if (value >= 255) {
    return 1023;
  }
  return (int) round(value / 255.0 * 1023);
}

void setColors(int red, int green, int blue) {
  red = covertResolution(red);
  green = covertResolution(green);
  blue = covertResolution(blue);
  analogWrite(R_LED, red);
  Serial.print("Setting red: ");
  Serial.println(red);
  analogWrite(G_LED, green);
  Serial.print("Setting green: ");
  Serial.println(green);
  analogWrite(B_LED, blue);
  Serial.print("Setting blue: ");
  Serial.println(blue);
}

void setup(void) {
  pinMode(2, OUTPUT);
  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);

  digitalWrite(BUILDIN_LED, HIGH);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/update", handleUpdate);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
