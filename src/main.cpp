
#include <Arduino.h>
#include <ArduinoJson.h>
#define DEBUG_ESP_PORT
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define ledON digitalWrite(2, LOW)
#define LedOff digitalWrite(2, HIGH)
#define modeClient 2
#define modeServerAP 1
const int mode_esp = modeClient;
// http://www.stroustrup.com/C++11FAQ.html#raw-strings
const char *INDEX_HTML PROGMEM =
#include "index.html"
    ;
ESP8266WebServer serverHTTP(80);

const char *ssid = "wifi kenceng";
const char *password = "";
/*********handler untuk http method *******/
void handleRoot()
{
  int panjangstrr = size_t(INDEX_HTML);
  serverHTTP.setContentLength(panjangstrr);
  serverHTTP.send(200, "text/html", INDEX_HTML);
}
void handleNotFound()
{
  String resp;
  resp = R"({"error":false,"message":"url not found"}")";
  serverHTTP.send(200, "application/json", resp);
}
void handleLed()
{
  String resp;
  if (serverHTTP.hasArg("plain") == false)
  {
    resp = R"({"error":false,"message":"Body tidak ada"}")";
    serverHTTP.send(200, "application/json", resp);
    return;
  }
  String data = serverHTTP.arg("plain");
  //parsing json body
  StaticJsonBuffer<50> JSONBuffer;                     //Memory pool
  JsonObject &bodyJSON = JSONBuffer.parseObject(data); //Parse message
  if (!bodyJSON.success())
  {
    resp = R"({"error":false,"message":"Parsing body failed"}")";
    serverHTTP.send(200, "application/json", resp);
    return;
  }
  char statusLed[20];
  strcpy(statusLed, bodyJSON["led"]);
  if (strcmp(statusLed, "on") == 0)
  {
    resp = R"({"error":false,"message":" command sukses ","command ":"led on"})";
    ledON;
  }
  else if (strcmp(statusLed, "off") == 0)
  {
    resp = R"({"error":false,"message":"command sukses","command":"led off"}")";
    LedOff;
  }
  else
  {
    resp = R"({"error":false,"message":"command failed"}")";
  }
  serverHTTP.send(200, "application/json", resp);
}
/********init setup*********/
void setup()
{

  delay(300);
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  if (mode_esp == modeServerAP)
  {
    WiFi.mode(WIFI_AP);
    boolean result = WiFi.softAP("wifi esp8266");
    if (result == true)
    {
      Serial.println("Ready");
    }
    else
    {
      Serial.println("Failed!");
    }
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    Serial.println();
  }
  else
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println(WiFi.macAddress());
    while (WiFi.status() != WL_CONNECTED)
    {
      ledON;
      delay(500);
      LedOff;
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
  }
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  serverHTTP.on("/", HTTP_GET, handleRoot);
  serverHTTP.on("/api/led", HTTP_POST, handleLed);
  serverHTTP.onNotFound(handleNotFound);
  serverHTTP.begin();
  Serial.println("server start");
}
// /****** always loop **/////////
void loop()
{
  serverHTTP.handleClient();
}
