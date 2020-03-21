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

#define APP_NAME "led-bedroom"

const char* ssid = STASSID;
const char* password = STAPSK;
const char* app = APP_NAME;

int red = 255;
int green = 255;
int blue = 255;

ESP8266WebServer server(80);

void handleStatus() {
  digitalWrite(BUILDIN_LED, LOW);
  char out[128];
  snprintf(out, 128, "{\"red\":%d,\"green\":%d,\"blue\":%d}", red, green, blue);
  Serial.print("Sending status: ");
  Serial.println(out);
  server.send(200, "application/json", out);
  digitalWrite(BUILDIN_LED, HIGH);
}

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
  return (int) round(constrain(value, 0, 255) / 255.0 * 1023);
}

void setColors(int redInput, int greenInput, int blueInput) {
  red = constrain(redInput, 0, 255);
  green = constrain(greenInput, 0, 255);
  blue = constrain(blueInput, 0, 255);
  int redResolved = covertResolution(red);
  int greenResolved = covertResolution(green);
  int blueResolved = covertResolution(blue);
  analogWrite(R_LED, redResolved);
  Serial.print("Setting red: ");
  Serial.println(redResolved);
  analogWrite(G_LED, greenResolved);
  Serial.print("Setting green: ");
  Serial.println(greenResolved);
  analogWrite(B_LED, blueResolved);
  Serial.print("Setting blue: ");
  Serial.println(blueResolved);
}

void blinkTwice(int redValue, int greenValue, int blueValue) {
  int previousRed = red;
  int previousGreen = green;
  int previousBlue = blue;
  setColors(redValue, greenValue, blueValue);
  delay(300);
  setColors(previousRed, previousGreen, previousBlue);
  delay(300);
  setColors(redValue, greenValue, blueValue);
  delay(300);
  setColors(previousRed, previousGreen, previousBlue);
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

  setColors(255, 255, 255);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  blinkTwice(0, 255, 0);

  delay(1000);

  if (MDNS.begin(app)) {
    Serial.println("MDNS responder started");
    blinkTwice(0, 0, 255);
  }

  server.on("/update", handleUpdate);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
