#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "ssid";
const char* password = "PW";

WebServer server(80);

const int led = 23;

void sendHtml(String url){
  String html = "";
  html += "<html><script src=\"http://www.w3schools.com/lib/w3data.js\"></script>";
  html += "<body><div w3-include-html=\"" + url + "\"></div>";
  html += "<script>w3IncludeHTML();</script></body></html>";
   server.send(200, "text/html", html);
}

void Main_page() { 
  digitalWrite(led, 1);
  delay(200);
  digitalWrite(led, 0);
  sendHtml("https://raw.githubusercontent.com/Park-SeungWoo/esp32_lecture/master/esp32_WEB_LED_CONTROL/html_file/index_arduino.html");
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", Main_page);
  
  server.on("/LEDON", []() {
    sendHtml("https://raw.githubusercontent.com/Park-SeungWoo/esp32_lecture/master/esp32_WEB_LED_CONTROL/html_file/index_arduino_ON.html");
    digitalWrite(led, HIGH);
    Serial.println("LED ON");
    
  });
  server.on("/LEDOFF", []() {
    sendHtml("https://raw.githubusercontent.com/Park-SeungWoo/esp32_lecture/master/esp32_WEB_LED_CONTROL/html_file/index_arduino_OFF.html");
    digitalWrite(led, LOW);
    Serial.println("LED OFF");
  });

  server.on("/WAITBLINK", [] () {
    sendHtml("https://raw.githubusercontent.com/Park-SeungWoo/esp32_lecture/master/esp32_WEB_LED_CONTROL/html_file/waiting_blink.html");
    Serial.println("Waiting blinking");
    for(int i = 0; i < 5; i++){
      digitalWrite(led, HIGH);
      delay(500);
      digitalWrite(led, LOW);
      delay(500);
    }
  });

  server.on("/LEDBLINK", []() {
    sendHtml("https://raw.githubusercontent.com/Park-SeungWoo/esp32_lecture/master/esp32_WEB_LED_CONTROL/html_file/index_arduino_BLINK.html");
    Serial.println("LED BLINK END");
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
