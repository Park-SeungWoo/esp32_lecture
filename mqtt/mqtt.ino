#include <WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>  // mqtt를 사용하기 위한 library

#define led 23  // 온습도 data를 받고있음을 표시하기위해 사용, 온습도 data를 읽지 못할경우엔 blink하게 함
#define dhtpin 22  // dht11 sensor를 22번 핀에서 사용
#define dhttype DHT11  // dht의 타입을 지정

const char* ssid = "SK_WiFiGIGA58E3";
const char* pw = "1702099900";
const char* MQTT_B_IP = "broker.mqtt-dashboard.com";  // 브로커의 ip를 담아 브로커와 통신하기 위함

float tem;  // 온도 값을 담을 변수 선언
float humid;  // 습도 값을 담을 변수 선언
int state;  // data를 받을지 안받을지 결정하는 상태 변수

WiFiClient Cli;  // mqtt통신을 할 cilent 객체 선언
DHT dht(dhtpin, dhttype);  // dht센서를 사용하기위한 객체 선언
PubSubClient Client(Cli);  // 브로커를 통해 subscriber나 publisher와 통신할 수 있는 객체 생성

//////////////////////functions//////////////////////////

void ValueError() {  // dht가 data를 못읽어올 때 동작시킬 함수
  digitalWrite(led, HIGH);
  Serial.println("Value Error!");
  delay(500);
  digitalWrite(led, LOW);
}

void Wificonnect() {  // wifi연결
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);
  Serial.println("WiFi connecting!");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected!");
}

void mqtt_publish() {  // 온도와 습도 data를 publish 해주는 함수
  String packet;
  char msg[50];
  packet = "Temperature : " + String(tem) + "*C / " + "Humidity : " + String(humid) + "%";
  packet.toCharArray(msg,50);
  Serial.print("Publish message : ");
  Serial.println(msg);
  Client.publish("dhtout", msg);
  delay(1000);
}

void ExecuteDht() {  // state값이 1, 즉 data를 받겠다고 했을 때 실행되는 함수
  digitalWrite(led, HIGH);
  tem = dht.readTemperature();
  humid = dht.readHumidity();
  if(isnan(tem) || isnan(humid)) {
    ValueError();
  }
  else{
    Serial.print("tem : ");
    Serial.println(tem);
    Serial.print("humid : ");
    Serial.println(humid);
    mqtt_publish();
  }
}

void SuspendDht() {  // state값이 0, 즉 data를 받지 않겠다고 했을 때 실행되는 함수
  digitalWrite(led, LOW);
}

void callback(char* topic, byte* message, unsigned int l) {  // 스마트폰의 mqtt tool로 부터 메시지를 받을 때 실행되는 콜백 함수
  Serial.print("Message [");
  Serial.print(topic);
  Serial.print("] : ");
  for(int i = 0; i < l; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();

  if(message[1] == 'n') {
    state = 1;
    Client.publish("dhtout", "on");
  }
  else if(message[1] == 'f') {
    state = 0;
    Client.publish("dhtout", "off");
  }
  else{
    Serial.println("topic error");
  }
}

void reconnect() {  // 브로커에 연결하고 특정 토픽을 구독하는 함수로 loop함수에서 주기적으로 호출됨
  while(!Client.connected()) {
    Serial.println("MQTT connecting...");
    if(Client.connect("MQTTool-1852150979")){
      Serial.println("MQTT Connected!");
      Client.publish("dhtout", "Connected!");
      Client.subscribe("dhtin");
    }
    else {
      Serial.print("failed, rc = ");
      Serial.println(Client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

////////////////////////////////////////////////////////

void setup() {
  pinMode(led, OUTPUT);
  pinMode(dhtpin, INPUT);
  Serial.begin(115200);
  dht.begin();
  Wificonnect();
  Client.setServer(MQTT_B_IP, 1883);  // 브로커의 ip와 포트를 통해 서버를 설정함
  Client.setCallback(callback);  // callback함수 설정
}

void loop() {
  if (!Client.connected()) {  // 클라이언트가 브로커와 연결이 되지 않은 상태이면 reconnect함수를 호출함
    reconnect();
  }
  Client.loop();
  switch(state) {
    case 1:
      ExecuteDht();
      break;
    case 0:
      SuspendDht();
      break;
  }
}
