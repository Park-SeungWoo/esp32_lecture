#define Led 22  //실행시 단 한번 실행되는 코드로 22번핀을 하나의 상수로 선언해줌
void setup() {
pinMode(Led, OUTPUT);  //22번핀을 OUTPUT으로 설정해 액츄에이터로서 동작할 수 있도록 함
}

void loop() {
digitalWrite(Led, HIGH);  //22번핀에 전류를 공급하여 상태를 HIGH로 바꿈
//delay(1000);  //1초
//delay(500);  //0.5초
delay(100);  //delay()함수를 사용해 100ms(0.1초)만큼 딜레이해
//delay(50);  //0.05초
//delya(10);  //0.01초
digitalWrite(Led, LOW);  //22번핀에 공급하던 전류를 차단하여 상태를 LOW로 바꿈
//delay(1000);
//delay(500);
delay(100);
//delay(50);
//delya(10);
}
