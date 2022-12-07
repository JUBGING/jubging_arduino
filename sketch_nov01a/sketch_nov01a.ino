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

#define RXPIN 13
#define TXPIN 12
//#define GPSBAUD 9600

//TinyGPS gps;

//gps
//void getgps(TinyGPS &gps);

SoftwareSerial bt(TXD,RXD);
//SoftwareSerial uart_gps(RXPIN, TXPIN);


// scale - 10Kg loadcell : 226 / 5kg loadcell : 372
// ADC 모듈에서 측정된 결과값을 (loadcellValue)값 당 1g으로 변환해 줌
float loadcellValue = 226.0;

HX711 scale;
Servo servo;      //Servo 클래스로 servo객체 생성
int value = 0;    // 각도를 조절할 변수 value


void setup() {
  // put your setup code here, to run once:
  servo.attach(9);
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
  // put your main code here, to run repeatedly:

//  while(uart_gps.available())     // While there is data on the RX pin...
//  {
//      int c = uart_gps.read();    // load the data into a variable...
//      if(gps.encode(c))      // if there is a new valid sentence...
//      {
//        getgps(gps);         // then grab the data.
//      }   
//  }
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
      servo.write(90);
    }
    else if(flag == 'c'){
      Serial.write(2);
      servo.write(-90);
      bt.print(dtostrf(scale.get_units(5)*0.001 , 5, 2, pChrBuffer));
    }
  }


  // 1초 대기
  delay(1000);
  
}

//void getgps(TinyGPS &gps)
//{
//  // To get all of the data into varialbes that you can use in your code, 
//  // all you need to do is define variables and query the object for the 
//  // data. To see the complete list of functions see keywords.txt file in 
//  // the TinyGPS and NewSoftSerial libs.
//  
//  // Define the variables that will be used
//  float latitude, longitude;
//  // Then call this function
//  gps.f_get_position(&latitude, &longitude);
//  // You can now print variables latitude and longitude
//  Serial.print("Lat/Long: "); 
//  Serial.print(latitude,5); 
//  Serial.print(", "); 
//  Serial.println(longitude,5);
//  
//  // Same goes for date and time
//  int year;
//  byte month, day, hour, minute, second, hundredths;
//  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
//  // Print data and time
//  Serial.print("Date: "); Serial.print(month, DEC); Serial.print("/"); 
//  Serial.print(day, DEC); Serial.print("/"); Serial.print(year);
//  Serial.print("  Time: "); Serial.print(hour, DEC); Serial.print(":"); 
//  Serial.print(minute, DEC); Serial.print(":"); Serial.print(second, DEC); 
//  Serial.print("."); Serial.println(hundredths, DEC);
//  //Since month, day, hour, minute, second, and hundr
//  
//  // Here you can print the altitude and course values directly since 
//  // there is only one value for the function
//  Serial.print("Altitude (meters): "); Serial.println(gps.f_altitude());  
//  // Same goes for course
//  Serial.print("Course (degrees): "); Serial.println(gps.f_course()); 
//  // And same goes for speed
//  Serial.print("Speed(kmph): "); Serial.println(gps.f_speed_kmph());
//  Serial.println();
//  
//  // Here you can print statistics on the sentences.
//  unsigned long chars;
//  unsigned short sentences, failed_checksum;
//  gps.stats(&chars, &sentences, &failed_checksum);
//  //Serial.print("Failed Checksums: ");Serial.print(failed_checksum);
//  //Serial.println(); Serial.println();
//}
