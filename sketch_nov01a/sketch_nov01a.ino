#include "HX711.h"
#include <SoftwareSerial.h>
#include<Servo.h> //Servo 라이브러리를 추가
//#include <TinyGPS.h>


// HX711 circuit wiring
#define LOADCELL_DOUT_PIN         2
#define LOADCELL_SCK_PIN          3

// Bluetooth
#define TXD                       4
#define RXD                       5

SoftwareSerial bt(TXD,RXD);

// scale - 10Kg loadcell : 226 / 5kg loadcell : 372
// ADC 모듈에서 측정된 결과값을 (loadcellValue)값 당 1g으로 변환해 줌
float loadcellValue = 226.0;

HX711 scale;
Servo servo;      //Servo 클래스로 servo객체 생성
Servo servo1;
int value = 0;    // 각도를 조절할 변수 value
int angle = 0;


void setup() {
  // put your setup code here, to run once:
  servo.attach(6);
  servo1.attach(7);
  servo.write(0);
  servo1.write(180);
  Serial.begin(115200);
  bt.begin(9600);
  //uart_gps.begin(GPSBAUD);

  // 로드셀 HX711 보드 pin 설정
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // 부팅 후 잠시 대기 (2초)
  delay(2000);

  // 측정값 1회 읽어오기
  Serial.print("read: \t\t\t");
  Serial.println(scale.read());

  delay(1000);

  // 스케일 설정
  scale.set_scale(loadcellValue);
  
  // 오프셋 설정(10회 측정 후 평균값 적용) - 저울 위에 아무것도 없는 상태를 0g으로 정하는 기준점 설정(저울 위에 아무것도 올려두지 않은 상태여야 합니다.)   
  scale.tare(10);    

  // 설정된 오프셋 및 스케일 값 확인
  Serial.print("Offset value :\t\t");
  Serial.println(scale.get_offset());
  Serial.print("Scale value :\t\t");
  Serial.println(scale.get_scale());

  // (read - offset) 값 확인 (scale 미적용)
  Serial.print("(read - offset) value: \t");  
  Serial.println(scale.get_value());
  delay(2000);

}

void loop() {
// 오프셋 및 스케일이 적용된 측정값 출력 (5회 측정 평균값) 
  Serial.print("value :\t");
  Serial.print(scale.get_units(5), 2);    // 5회 측정 평균값, 소수점 아래 2자리 출력
  Serial.println(" g");
  
  char pChrBuffer[50];
  bt.print(dtostrf(scale.get_units(5)*0.001 , 5, 2, pChrBuffer));

  if(bt.available()){
    char flag = bt.read();
    Serial.print(flag);
    if(flag == 'a'){
      Serial.print(0);
      bt.print(dtostrf(scale.get_units(5)*0.001 , 5, 2, pChrBuffer));
    }
    else if(flag == 'b'){
      Serial.write(1);
      for(angle=0; angle < 110; angle++){
        Serial.write(4);
        servo.write(angle);
        servo1.write(110 - angle);
        delay(15);
      }
    }
    else if(flag == 'c'){
      Serial.write(2);
      for(angle=110; angle > 0; angle--){
        servo.write(angle);
        servo1.write(110 - angle);
        delay(15);
      }
      bt.print(dtostrf(scale.get_units(5)*0.001 , 5, 2, pChrBuffer));
    }
  }
  // 1초 대기
  delay(1000);
}
