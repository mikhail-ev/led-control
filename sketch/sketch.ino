#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

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

String markup = "";

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

void handleRoot() {
  digitalWrite(BUILDIN_LED, LOW);
  server.send(200, "text/html", markup);
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
  Serial.print(redResolved);
  analogWrite(G_LED, greenResolved);
  Serial.print(" , setting green: ");
  Serial.print(greenResolved);
  analogWrite(B_LED, blueResolved);
  Serial.print(" , setting blue: ");
  Serial.println(blueResolved);
}

void blinkTwice(int redValue, int greenValue, int blueValue) {
  blinkOnce(redValue, greenValue, blueValue);
  blinkOnce(redValue, greenValue, blueValue);
}

void blinkOnce(int redValue, int greenValue, int blueValue) {
  int previousRed = red;
  int previousGreen = green;
  int previousBlue = blue;
  setColors(redValue, greenValue, blueValue);
  delay(300);
  setColors(previousRed, previousGreen, previousBlue);
  delay(300);
}

boolean loadMarkup() {
  WiFiClient client;
  HTTPClient http;
  boolean success = false;

  char* url = "http://led-control-app.herokuapp.com/markup?app=";
  strcat(url, app); /* add the extension */

  Serial.print("[HTTP] begin...\n");
  if (http.begin(client, url)) {
    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        markup = http.getString();
        success = true;
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      if (httpCode == HTTP_CODE_INTERNAL_SERVER_ERROR) {
        blinkTwice(255, 0, 0);
      } else {
        blinkOnce(255, 0, 0);
      }
    }

    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
    blinkOnce(255, 0, 0);
  }

  return success;
}

void connectToWiFi() {
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
}

void setup(void) {
  pinMode(2, OUTPUT);
  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);

  digitalWrite(BUILDIN_LED, HIGH);

  Serial.begin(115200);

  setColors(255, 255, 255);

  connectToWiFi();

  blinkTwice(255, 255, 0);

  if (loadMarkup()) {
    blinkTwice(0, 0, 255);
  } else {
    return;
  }

  delay(1000);

  if (MDNS.begin(app)) {
    Serial.println("MDNS responder started");
    blinkTwice(0, 255, 0);
  }

  server.on("/", handleRoot);
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
