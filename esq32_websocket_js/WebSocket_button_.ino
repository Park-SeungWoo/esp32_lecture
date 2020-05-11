#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#define button 23  
#define led 2

static int cliip = 0;

const char* ssid = "U+NetB495";
const char* pw = "1C1A027155";

WebServer server(80);  
WebSocketsServer webSocket = WebSocketsServer(81);  //소켓통신을 하기위한 작업

void Main_P()
{
  String html = "";
  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<meta charset=\"UTF-8\">";
  html += "<title>Main Page</title>";
  html += "<style>";
  html += "body {background: #34495e; background-attachment:unset;}";
  html += ".mes {text-align: center;color: #2ecc71;font-size: 60px;font-family:Impact, Haettenschweiler, 'Arial Narrow Bold', sans-serif;}";
  html += ".box {width: 300px;height: 300px;padding: 40px;position: relative;top: 50%;left: 50%;background: #191919;text-align: center;border-radius: 10px;transform: translatex(-50%);}";
  html += "#buton {border: 3px solid black;border-radius: 40px;background: #2ecc71;width: 80px;height: 80px;position: absolute;left: 50%;top: 50%;transform: translate(-50%, -50%);}";
  html += "#butoff {border: 3px solid black;border-radius: 50px;background: #34495e;width: 100px;height: 100px;position: absolute;left: 50%;top: 50%;transform: translate(-50%, -50%);}";
  html += "</style>";
  html += "</head>";
  html += "<body>";
  html += "<div class=\"mes\">";
  html += "<p>Button State</p>";
  html += "</div>";
  html += "<div class=\"box\">";
  html += "<div id=\"buttonstate\">";
  html += "</div>";
  html += "</div>";
  html += "<script type=\"text/javascript\">";
  html += "var wsUri = \"ws://192.168.219.105:81/\";";
  html += "function init() {websocket = new WebSocket(wsUri);websocket.onopen = function(evt) { onOpen(evt) };websocket.onclose = function(evt) { onClose(evt) };websocket.onmessage = function(evt) { onMessage(evt) };websocket.onerror = function(evt) { onError(evt) }; }";
  html += "function onOpen(evt) {alert('connected');}";
  html += "function onClose(evt) {alert('disconnected');}";
  html += "function onMessage(evt) {if (evt.data == \"1\") {document.getElementById('buttonstate').innerHTML = \"<div id='buton'></div>\";}else if (evt.data == \"0\") {document.getElementById('buttonstate').innerHTML = \"<div id='butoff'></div>\";}}";
  html += "function onError(evt) {alert('error');}";
  html += "window.addEventListener(\"load\", init, false);";
  html += "</script>";
  html += "</body>";
  html += "</html>";
  server.send(200, "text/html", html);
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
   cliip = num;
   switch(type) {   //웹소켓이벤트의 타입
     case WStype_DISCONNECTED:   //클라이언트 연결 해제시
       Serial.printf("[%u] Disconnected!\n", num);
       break;
       
     case WStype_CONNECTED:     //클라이언트 연결시
       {
           IPAddress ip = webSocket.remoteIP(num);
           Serial.printf("[%u] Connected!\n", num);
           Serial.println(ip.toString());
       }
       break;

     default:
       break;
   }
}

void setup() {
  pinMode(button, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ////////////////고정 IP 할당/////////////////
  IPAddress ip(192, 168, 219, 105);
  IPAddress gateway(192, 168, 219, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  ////////////////////////////////////////
  WiFi.begin(ssid, pw);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected!");
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());
  
  digitalWrite(led, HIGH);  //WiFi에 연결되었음을 built in led로 표시
  delay(500);
  digitalWrite(led, LOW);

  server.on("/", Main_P);
  
  server.begin();
  Serial.println("Server Start!");
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  Serial.println("Socket Start!");
}

void loop() {
  server.handleClient();
  webSocket.loop();
  if (digitalRead(button) == LOW)
  {
    webSocket.sendTXT(cliip, "1");
    Serial.println("button pushed!");
  }
   else if (digitalRead(button) == HIGH)
  {
    webSocket.sendTXT(cliip, "0");
  }
  delay(30);
}
