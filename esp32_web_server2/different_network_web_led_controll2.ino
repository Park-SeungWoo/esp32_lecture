#include <WiFi.h>
#include <WebServer.h>
#define led 23

const char* ssid = "";
const char* PW = "";

WebServer server(81);  //HTTP를 사용하기위한 포트(81)으로 서버 통신을 할 객체 생성

void sendHtml(String url)  // 외부에서 작성해 인터넷에 올린 html파일을 읽어와 esp32 서버로 보내주는 함수
{
  String html = "";
  html += "<html><script src=\"http://www.w3schools.com/lib/w3data.js\"></script>";
  html += "<body><div w3-include-html=\"" + url + "\"></div>";
  html += "<script>w3IncludeHTML();</script></body></html>";
   server.send(200, "text/html", html);
}


////////////////////////////page별 동작 함수들/////////////////////////
void Main_P()  //처음페이지 접속시 동작하는 함수
{
  sendHtml("https://raw.githubusercontent.com/Park-SeungWoo/esp32_lecture/master/esp32_web_server2/main.html");
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
}

void LedOn()  //LEDON 페이지에 접속시 동작하는 함수
{
  sendHtml("https://raw.githubusercontent.com/Park-SeungWoo/esp32_lecture/master/esp32_web_server2/LEDON.html");
  Serial.println("LED ON!");
  digitalWrite(led, HIGH);
}

void LedOff()   //LEDOFF 페이지에 접속시 동작하는 함수
{
  sendHtml("https://raw.githubusercontent.com/Park-SeungWoo/esp32_lecture/master/esp32_web_server2/LEDOFF.html");
  Serial.println("LED OFF!");
  digitalWrite(led, LOW);
}
//////////////////////////////////////////////////////////////////


void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);  //esp32를 WIFI_STA모드로 동작시킨다
  WiFi.begin(ssid, PW);
  Serial.println("");

  //connecting
  while (WiFi.status() != WL_CONNECTED) //wifi가 연결중임을 표시
  {
    delay(500);
    Serial.print("-");
  }
  Serial.println("");
  Serial.println("Connected!");
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());

  server.on("/", Main_P);  // IP주소로 접속하였을 시 Main()함수가 동작함
  server.on("/LEDON", LedOn);  // IP주소/LEDON 으로 접속하였을 시 LedOn()함수가 동작함
  server.on("/LEDOFF", LedOff);   // IP주소/LEDOFF 으로 접속하였을 시 LedOff()함수가 동작함

  server.begin();  // 서버 사용을 시작하는 함수로, 아까 설정한 포트(81)로 서버에서 통신이 들어오게 대기를 시작한다.
  Serial.println("Server Start!");
}

void loop() {
  server.handleClient();  // 클라이언트들의 요청된 주소를 보고 setup함수에 만들어놓은 주소와 일치하면 그 함수의 코드가 실행되도록 한다.
}
