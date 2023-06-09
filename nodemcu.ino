#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "";
const char* password = "";
const int udpPort = 5005;
const int serverPort = 80;

int pushUpCount = 0;

WiFiUDP udp;
AsyncWebServer server(serverPort);

void handleRoot(AsyncWebServerRequest *request) {
  String html = "<html><head>";
  html += "<style>";
  html += "body {";
  html += "  font-family: Arial, sans-serif;";
  html += "  text-align: center;";
  html += "}";
  html += "h1 {";
  html += "  color: #333;";
  html += "}";
  html += "</style>";
  html += "</head><body>";
  html += "<h1>Push-Up Count: <span id='count'>" + String(pushUpCount) + "</span></h1>";
  html += "<script>";
  html += "setInterval(fetchCount, 1000);";
  html += "function fetchCount() {";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.onreadystatechange = function() {";
  html += "    if (this.readyState == 4 && this.status == 200) {";
  html += "      document.getElementById('count').textContent = this.responseText;";
  html += "    }";
  html += "  };";
  html += "  xhttp.open('GET', '/pushup_count', true);";
  html += "  xhttp.send();";
  html += "}";
  html += "</script>";
  html += "</body></html>";

  request->send(200, "text/html", html);
}


void handlePushUpCount(AsyncWebServerRequest *request) {
  String countValue = String(pushUpCount);
  request->send(200, "text/plain", countValue);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  udp.begin(udpPort);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/pushup_count", HTTP_GET, handlePushUpCount);

  server.begin();
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    byte buffer[packetSize];
    udp.read(buffer, packetSize);
    buffer[packetSize] = '\0';

    String countValue = String((char*)buffer);
    pushUpCount = countValue.toInt();

    Serial.print("Received push-up count: ");
    Serial.println(pushUpCount);
  }

  // Other code in the loop if needed
}
