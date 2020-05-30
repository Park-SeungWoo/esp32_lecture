#include <WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>

#define led 23  // dht11이 값을 읽지 못하였을 떄 표시하기 위함
#define dhtpin 22  // dht11을 22번 핀에서 사용
#define dhttype DHT11  // dht11를 이용하기 위해 type을 DHT11로 지정해둠

const char* ssid = "U+NetB495";
const char* pw = "1C1A027155";
unsigned long chID = 1071436;
const char* WKey = "DJO95TTVZ8GKD9PY";

float tem = 0.0;  // 온도값을 담을 변수 미리 선언
float humid = 0.0;  // 습도값을 담을 변수 미리 선언

DHT dht(dhtpin, dhttype);
WiFiClient Cli;  // thingspeak에 접속 할 wifi client객체를 하나 생성

void ValueError() {  // dht11 센서가 값을 읽어오지 못할 때 작동하는 함수
  digitalWrite(led, HIGH);
  Serial.println("Value Error!");
  delay(500);
  digitalWrite(led, LOW);
}

void setup() {
  pinMode(led, OUTPUT);
  pinMode(dhtpin, INPUT);
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, pw);
  Serial.println("Start connection");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected!");
  ThingSpeak.begin(Cli);  // 미리 선언해 둔 wifi 객체로 thingspeak와 통신을 시작
}

void loop() {
  tem = dht.readTemperature();
  if(isnan(tem)){  // 온도를 읽어오지 못했을 때 동작
    ValueError();
  }
  else{// dht11이 온도 값을 제대로 읽어왔을 때 동작
    Serial.print("tem : ");
    Serial.println(tem);
    ThingSpeak.writeField(chID, 1, tem, WKey);  // 값을 보냄 (채널ID, 필드번호, 값, API키)
    delay(15000);  // thingspeak의 무료버전을 사용기에 15초마다 한번씩 데이터를 보낼 수 있어 습도값은 15초 후에 읽고 전송하기위함
  }

  humid = dht.readHumidity();
  if(isnan(humid)){  // 습도를 읽어오지 못했을 때 동작
    ValueError();
  }
  else{  // dht11이 습도 값을 제대로 읽어왔을 때 동작
    Serial.print("humidity : ");
    Serial.println(humid);
    ThingSpeak.writeField(chID, 2, humid, WKey);  //값을 보냄 (채널ID, 필드번호, 값, API키)
    delay(15000);  // 습도값도 보낸 후 15초의 delay를 줌
  }
}
