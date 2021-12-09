#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#define LED 2
//use pin 16 for blink external LED bulb - more details please check video tutorial
//#define LED 16


const char* ssid     = "MyDevelopmentBoard WiFi";
const char* password = "12345678";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void setup() {
  pinMode(LED, OUTPUT);

  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  server.begin();
  Serial.println("web socket server started");
}

void loop() {
  ws.cleanupClients();
}


void onWebSocketEvent(AsyncWebSocket *server,
                      AsyncWebSocketClient *client,
                      AwsEventType type,
                      void *arg,
                      uint8_t *data,
                      size_t len)
{
  String cmd = "";
  switch (type)
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      ws.text(client->id(), String("connected"));
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      digitalWrite(LED, LOW);
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        Serial.println((char *)data);

        cmd = "";
        for (int i = 0; i < len; i++) {
          cmd = cmd + (char) data[i];
        } //merging payload to single string
        Serial.println(cmd);

        if (cmd == "led1on") {
          digitalWrite(LED, HIGH);
          ws.text(client->id(), String("led1on"));
        } else if (cmd == "led1off") {
          digitalWrite(LED, LOW);
          ws.text(client->id(), String("led1off"));
        }
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;
  }
}
