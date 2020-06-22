#include <WiFiClientSecure.h>
#include <DHT.h>
#include <PubSubClient.h>  // mqtt를 사용하기 위한 library

#define led 23  // 온습도 data를 받고있음을 표시하기위해 사용, 온습도 data를 읽지 못할경우엔 blink하게 함
#define dhtpin 22  // dht11 sensor를 22번 핀에서 사용
#define dhttype DHT11  // dht의 타입을 지정

const char* ssid = "U+NetB495";
const char* pw = "1C1A027155";
const char* host = "a1fqcs3gnxvnkh-ats.iot.us-west-2.amazonaws.com";  // aws에서 연결한 사물/상호작용 에서 주소를 가져옵니다.

// 사물 인증서 (파일 이름: xxxxxxxxxx-certificate.pem.crt)
const char cert_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUHoVXsxBNnPrx1N0Ig7/q+usUYdYwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIwMDYyMjA5NDQz
NloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMV26PVmkVDXnvf+3HQs
0kFQxLtj+4/hI0AkC6xe7ob5wCDTYhOidKfqVfSMO8wS/4EcYIMNpYm1MXthq+Ud
2eFw3gAUW9mwGjCeHo8S/EzcX5oVa0b/sh/Q1Yu9B8Av5CgxwmNMYrATFO5DC0SK
MjQOsF1+QMKk+a50jY/MHHfIETrv1maN7n0qsCQ03sVSw4gh3KFG0DF/Jtd0APWP
izSVY9jpgivSYiaMbtcwNHuUvZPU6gUNfVxmCmJXh1uwCPh8Z29C4ieOIIOB21D5
NaWGXSP5Jqt6RuV+XhqdflBmc/RgZaCy7UkFrKb6JCragdbJQM2rrKi1J/3GZfxZ
uLcCAwEAAaNgMF4wHwYDVR0jBBgwFoAUY1ds6Gn8cB4AbMFNdkrQJNYXleMwHQYD
VR0OBBYEFASVGtfTokg1KKVzbZvQ3nWM1ergMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAK/iuoMZ9WZ8TLdEZu2MYFC4xo
QDQZREJp0isL8RI5obWNFLW9aJwJciKjd6CcHcj5WyGUjMfi3XxMFNbWsVta0Y4P
qEDiSu93BYVspfV/76JYIlUnqDfOIlwpqEcfSmHfSUWisvXtNSvi2E71bZdUK5QE
wCJPmgWIsILqkIh09MyQR9v/qTCsJp1vmV4jOI5IhnGiY+85KLOeaqATtLpsgktr
6CrOlq0vk4UY678jfo+iruWermXnRq8XKPx7I0S+Vosx2u6D1U8Wv4EJ8V1gsaI9
OXM4ykCDIjqwTdpyHZ3P1io1AmWgyTU+w8ncp7DvOwz0xKOXCRiO7grfoqnF
-----END CERTIFICATE-----
)EOF";

// 사물 인증서 프라이빗 키 (파일 이름: xxxxxxxxxx-private.pem.key)
const char key_str[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEAxXbo9WaRUNee9/7cdCzSQVDEu2P7j+EjQCQLrF7uhvnAINNi
E6J0p+pV9Iw7zBL/gRxggw2libUxe2Gr5R3Z4XDeABRb2bAaMJ4ejxL8TNxfmhVr
Rv+yH9DVi70HwC/kKDHCY0xisBMU7kMLRIoyNA6wXX5AwqT5rnSNj8wcd8gROu/W
Zo3ufSqwJDTexVLDiCHcoUbQMX8m13QA9Y+LNJVj2OmCK9JiJoxu1zA0e5S9k9Tq
BQ19XGYKYleHW7AI+Hxnb0LiJ44gg4HbUPk1pYZdI/kmq3pG5X5eGp1+UGZz9GBl
oLLtSQWspvokKtqB1slAzausqLUn/cZl/Fm4twIDAQABAoIBAEpKAjrQ67rOZDBI
4geTFUcda+ZUJ0BqIyjuZrXiwTpmm7G91Rgt9lV2BACNAbI1M4sPcf/cEntMf+E7
TcBVCrOWEu9UuXmclz3TPwv0yJbiEdWWWVXMvObbWnvIEjJr0hqNHeTdyAMWKrm4
33RZyalsHU4PQa35AzZvAxMmZ09oe7JcY240KllGR439FFURKi11Qh7Y9t+lHywf
JC/c5EgaWmhChtN5CwiKM+TVUjyWxSbvAYBXz8YUFr9linP4DFjQWBL/FjpwYF2P
X96s/WjkvuxsqxBQd4qsK9ueas8OszpVdkzsKevM6iB8RJYfUR26J3AkTCuVHJo0
rQY7cAECgYEA9kYVD62AWAY8IBJ+CF73Hx49GwNlzwbx8MUv/L/61B25kz6+wPWp
1R+FseCbIPylP5vXoI8bGiNVnzi3TniCJHFP3zWI0UI+aVuH0Yx7OOZobiiTivtK
QDZMQzuw6u9VVvKHWfer6fMntUPJdeEHesaYblAPxsqR9RNUPMSovTcCgYEAzUNZ
I0+mgDr0M9Mz5/ylE+X+16s4U865vsexUvabpkI7T6lQ+SI5guCYzMFTjJTvzmXB
ZODV0A6pdBjivIbJjKNzIFy0fPHeTi+fByE5Fe/E+ZoXyJLUaL/FDv62ryX+UT46
OPzlN+olgUyH2CCVpug0F1N1EBCgX3HTRQNwoIECgYAr3F9z41qwUqvyqlh39THe
AOyxGvHO5VXBo9zgDvEFRiX+63Xdt+L/ahXNkEsQnO5xXbgRiGszRIfDS76Z4yyK
gQPQpSAw5LNo4DxYPcTUZXTD6cYdQpP+y6zZbJAdx3JTge5JP6sGh5/S0LfYnrNO
UoBjZS7qILnOBBtEl2HWEQKBgDrxAvCr15Ds3ODg3f/YwYJkQfimNweBzj6lUxsS
q4FjdWqboR/ZHxCg89+gCCDPR8lJ2O3hziPvgeNPnhQq1EdpTA7X8BAM2Bw1eoHu
nVjaQAK4p+m1l/74SlrS85x52Xx01P02SY/z0Y8+bae3O6Hw/bxQ/8V8MMEXi2Y+
5aGBAoGASEEhAx5q84ZJLNnOSELo3RKad57vg52lPS9OQqRqeosWAJpJPKZ60CTW
CxUmw7/ZI+QqwuIdAyyrdJtfLjtyCuH95IIGIJ3tFhgh/YVpxw2wssF8HXX4I6Xx
2Y7eeKULSLVzi8wFD61i00ykHiIfseW/I2ydJ2mc/DTuszcnv5I=
-----END RSA PRIVATE KEY-----
)EOF";

// Amazon Trust Services(ATS) 엔드포인트 CA 인증서 (서버인증 > "RSA 2048비트 키: Amazon Root CA 1" 다운로드)
const char ca_str[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF"; 

float tem;  // 온도 값을 담을 변수 선언
float humid;  // 습도 값을 담을 변수 선언

WiFiClientSecure Cli = WiFiClientSecure();  // mqtt통신을 할 secured 된 wifi cilent 객체 선언
DHT dht(dhtpin, dhttype);  // dht센서를 사용하기위한 객체 선언
PubSubClient Client(Cli);  // aws를 통해 subscriber나 publisher와 통신할 수 있는 객체 생성

//////////////////////functions//////////////////////////

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

void callback(char* topic, byte* message, unsigned int l) {  // 스마트폰의 mqtt tool로 부터 메시지를 받을 때 실행되는 콜백 함수
  Serial.print("Message [");
  Serial.print(topic);
  Serial.print("] : ");
}

void ExecuteDht() {  // dht 값을 읽어 값을 판별하고 publish하는 함수
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

////////////////////////////////////////////////////////

void setup() {
  pinMode(led, OUTPUT);
  pinMode(dhtpin, INPUT);
  Serial.begin(115200);
  dht.begin();
  Wificonnect();
  Cli.setCACert(ca_str);  // 이것부터 밑에 두개까지는 인증서를 연결해줌
  Cli.setCertificate(cert_str);
  Cli.setPrivateKey(key_str);
  Client.setServer(host, 8883);  // 브로커의 ip와 포트를 통해 서버를 설정함
  Client.setCallback(callback);  // callback함수 설정
}

void loop() {
  if (!Client.connected()) {  // 클라이언트가 브로커와 연결이 되지 않은 상태이면 reconnect함수를 호출함
    reconnect();
  }
  Client.loop();
  ExecuteDht();
}
