#include <WiFi.h>
#include <DHT.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

#define dhtpin 23  // 23번핀을 사용하여 값을 읽어옴 
#define dhttype DHT11  //DHT11을 사용하기위해 dhttype상수에 DHT11이라는 값을 미리 담아둠 
#define led 2  // 와이파이에 연결하게되면 표시를 하기위해 사용함 

const char* ssid = "";
const char* pw = "";
static int cliip = 0;  //client ip를 담을 전역변수를 선언하여 코드의 어느 위치에서든 client로 값을 보낼 수 있도록함 

AsyncWebServer server(80);  //비동기식 웹서버를 생성해 80번 포트로 통신함 
WebSocketsServer webSocket = WebSocketsServer(81);  //웹 소켓통신을 이용해 81번포트로 통신함 
DHT dht(dhtpin, dhttype);  //DHT센서를 미리정해둔 상수 두개를 통해 사용함 


//html코드를 서버에 올릴때 사용할 html코드를 담고있는 상수로 아래와 같이 사용함 
const char index_html[] PROGMEM = R"rawliteral(  
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=0.7">
        <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
        <title>DHT SENSOR</title>
        <style>
            body {
                background: #34495e;
            }

            .mes {
                text-align: center;
                color: #2ecc71;
                font-size: 60px;
                font-family:Impact, Haettenschweiler, 'Arial Narrow Bold', sans-serif;
            }

            .box {
                width: 500px;
                height: 200px;
                position: relative;
                top: 50%;
                left: 50%;
                background: #191919;
                text-align: center;
                border-radius: 10px;
                transform: translatex(-50%);
            }

            .centerbox {
                position: relative;
                top: 50%;
                left: 50%;
                transform: translate(-50%, -50%);
            }

            #temperature {
                position: relative;
                line-height: 50px;
                text-align: center;
                color: #b4e4c8;
                font-size: 30px;
                font-weight: bold;
            }

            #humidity {
                position: relative;
                line-height: 50px;
                text-align: center;
                color: #b4e4c8;
                font-size: 30px;
                font-weight: bold;
            }
        </style>
    </head>
    <body>
        <div class="mes">
            <p>DHT Values</p>
        </div>
        <div class="box">
            <div class="centerbox">
                <div id='temperature'></div>
                <div id='humidity'></div>
            </div>
        </div>
        <script>
            let wsip = "ws://192.168.219.105:81/";
            function init() {
                websocket = new WebSocket(wsip);
                websocket.onopen = function(evt) {onOpen(evt)};
                websocket.onclose = function(evt) {onClose(evt)};
                websocket.onmessage = function(evt) {onMessage(evt)};
                websocket.onerror = function(evt) {onError(evt)};
            }
            function onOpen(evt) {
                alert('Connected');
            }
            function onClose(evt) {
                alert('Disconnected');
            }
            function onError(evt) {
                alert('Error');
            }
            function onMessage(evt) {
                let Data = evt.data.split('_');
                if (Data[0] == 't') {
                    document.getElementById('temperature').innerHTML = "<i class=\"fa fa-thermometer-2\" style=\"font-size:48px;color:skyblue\"></i>" + " Temperature : " + Data[1] + " &deg;C";
                }
                else if (Data[0] == 'h') {
                    document.getElementById('humidity').innerHTML = "<i class=\"fa fa-tint\" style=\"font-size:48px;color:skyblue\"></i>" + " Humidity : " + Data[1] + " %";
                }
            }
            window.addEventListener("load", init, false);
        </script>
    </body>
    </html>)rawliteral";

void indexHTML(AsyncWebServerRequest *request)  //위의 raw html코드를 가진 상수를 통해 서버에 html코드를 전송하는 함수
{
  request->send(200, "text/html", index_html);
}


//콜백 함수
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
  pinMode(dhtpin, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  dht.begin();  // DHT센서의 센싱을 시작함 
  WiFi.mode(WIFI_STA);
  //////////////고정 Ip할당///////////////
  IPAddress ip(, , , );
  IPAddress gateway(, , , );
  IPAddress subnet(, , , );
  WiFi.config(ip, gateway, subnet);
  ////////////////////////////////
  WiFi.begin(ssid, pw);
  Serial.println("");

  while(WiFi.status() != WL_CONNECTED)
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

  server.on("/", HTTP_GET, indexHTML);  //주소가"/"인 서버를 열고 HTTP통신으로 indexHTML함수에서 html코드를 서버로 보냄 

  server.begin();
  Serial.println("Server Start!");
  webSocket.begin();  // 웹 소켓 통신 시작 
  webSocket.onEvent(onWebSocketEvent);  //콜백 실행 
  Serial.println("Socket Start!");
}

void loop() {
  webSocket.loop();
  float tem = dht.readTemperature();  //온도값을 읽어와 tem변수에 저장함 
  float humid = dht.readHumidity();  //습도값을 읽어와 humid변수에 저장함 
  
  // 소켓 통신으로 데이터를 보낼 때 데이터가 온도값인지 습도값인지 표시하기위한 일종의 헤더 정보 
  String t = "t_";  // 온도를 표시하는 헤더 정보 
  String h = "h_";  // 습도를 표시하는 헤더 정보 
  if(isnan(tem) || isnan(humid))  // 온도와 센서값을 읽어오지 못할때 
  {
    t += "--";  // 헤더정보 "t_"의 뒤에 --를 더해 값을 못 읽어왔을때 --로 표시하게 함 
    h += "--";  // 헤더정보 "h_"의 뒤에 --를 더해 값을 못 읽어왔을때 --로 표시하게 함 
    webSocket.sendTXT(cliip, t);  // 헤더정보와 함께 온도 값을 클라이언트로 보냄 
    webSocket.sendTXT(cliip, h);  // 헤더정보와 함께 습도 값을 클라이언트로 보냄 
  }
  else  // 온도와 센서값을 읽어오면 
  {
    t += String(tem);  // 읽어온 온도값을 String으로 형변환해 온도 헤더정보에 온도값을 추가함 
    h += String(humid);  // 읽어온 습도값을 String으로 형변환해 습도 헤더정보에 온도값을 추가함 
    webSocket.sendTXT(cliip, t);  // 헤더정보와 온도값이 담긴 t변수의 값을 클라이언트로 보냄 
    webSocket.sendTXT(cliip, h);  // 헤더정보와 습도값이 담긴 h변수의 값을 클라이언트로 보냄 
  }
  delay(100);  //0.1초 딜레이를 줌 
} 
