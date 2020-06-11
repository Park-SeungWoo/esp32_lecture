#include <WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define led 23
#define dhtpin 22
#define dhttype DHT11

const char* ssid = "U+NetB495";
const char* pw = "1C1A027155";
const char* MQTT_B_IP = "broker.mqtt-dashboard.com";

float tem;
float humid;
String jsondata;

WiFiClient Cli;
DHT dht(dhtpin, dhttype);
PubSubClient Client(Cli);
StaticJsonBuffer<200> jsonBuf;
JsonObject& root = jsonBuf.createObject();

///////////////////////////////
void ValError() {
  digitalWrite(led, HIGH);
  Serial.println("Value Error!");
  delay(500);
  digitalWrite(led, LOW);
}

void WiFiConnect() {
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

void reconnect() {
  while(!Client.connected()) {
    Serial.println("MQTT connecting...");
    if(Client.connect("pswarkeungoo")){
      Serial.println("MQTT Connected!");
    }
    else {
      Serial.print("failed, rc = ");
      Serial.println(Client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int l) {
  Serial.println("callback");
}

void SaveValToJson_send() {
  root["Temperature"] = tem;
  root["Humidity"] = humid;
  if (tem > 24.0 || humid > 70.0) {
    root["Tip"] = "Turn on the air conditioner.";
    digitalWrite(led, HIGH);
  }
  else {
    root["Tip"] = "You're in a pleasant state.";
    digitalWrite(led, LOW);
  }
  root.printTo(jsondata);
  char resultJsondata[100];
  jsondata.toCharArray(resultJsondata, 100);
  Client.publish("tem_humid_from_dht11_psw", resultJsondata);
  jsondata = "";
}

void DhtSensing() {
  tem = dht.readTemperature();
  humid = dht.readHumidity();
  if (isnan(tem) || isnan(humid)) {
    ValError();
  }
  else{
    SaveValToJson_send();
  }
  delay(1000);
}

/////////////////////////////////

void setup() {
  pinMode(led, OUTPUT);
  pinMode(dhtpin, INPUT);
  Serial.begin(115200);
  dht.begin();
  WiFiConnect();
  Client.setServer(MQTT_B_IP, 1883);
  Client.setCallback(callback);
}

void loop() {
  if (!Client.connected()) {
    reconnect();
  }
  Client.loop();
  DhtSensing();
}
