#include <SoftwareSerial.h>
#include <Wire.h>
#include <Servo.h>

//輸出到L298N的腳位
#define MT_LF 2  //ENA L2
#define IN5 30
#define IN6 31

#define IN7 32
#define IN8 33
#define MT_RF 3  //ENB R2

#define MT_R 5 //ENC L1
#define IN3 43
#define IN4 42

#define IN1 41
#define IN2 40
#define MT_L 4  //END R1

#define ENE 6
#define IN9 7
#define IN10 8

//const int dirPin = 12;  // 方向引脚(步進腳位)
//const int stepPin = 13; // 步进引脚
#define dirPin1 12
#define stepPin1 13
#define stepPin2 22
#define dirPin2 23
#define stepPin3 46
#define dirPin3 47
const int STEPS_PER_REV = 200;//non of use

char Car_status;
int Speed_value=160;
// 255-70 = 185
SoftwareSerial BT(10,11);

Servo myservo;

void setup() {
  pinMode(MT_L,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(MT_R,OUTPUT);
  
  pinMode(MT_LF,OUTPUT);
  pinMode(IN5,OUTPUT);
  pinMode(IN6,OUTPUT);
  pinMode(IN7,OUTPUT);
  pinMode(IN8,OUTPUT);
  pinMode(MT_RF,OUTPUT);

  pinMode(stepPin1,OUTPUT); //步進
  pinMode(dirPin1,OUTPUT);
  pinMode(stepPin2,OUTPUT); //步進
  pinMode(dirPin2,OUTPUT);
  pinMode(stepPin3,OUTPUT); //步進
  pinMode(dirPin3,OUTPUT);
  
  pinMode(ENE,OUTPUT);
  pinMode(IN9,OUTPUT);
  pinMode(IN10,OUTPUT);

  BT.begin(38400); // 設定和 HC-06 通訊的速度 (預設 9600)
  Serial.begin(38400); 

  myservo.attach(9);

}
 
void loop() {
//當Serial有資料傳輸時，會將讀取到的字元存到Car_status。
  if(Serial.available() == true){   
    Car_status = Serial.read();  
  }
  
//當藍芽有資料傳輸時，會將讀取到的字元存到Car_status。
  if (BT.available()){      
    Car_status =BT.read();  
    Serial.println(Car_status);  
  }
  
//依照Car_status的字元來選擇車子方向控制
 switch(Car_status){   
  case 'F':
    Forward();
    Serial.println("Forward");
    Car_status='N';
    break;
  case 'B':
    Backward();
    Serial.println("Backward");
    Car_status='N';
    break;
  case 'R':
    TurnRight();
    Serial.println("TurnRight");
    Car_status='N';
    break;
  case 'L':
    TurnLeft();
    Serial.println("TurnLeft");
    Car_status='N';
    break;
  case 'S':
    allstop();
    Serial.println("Stop");
    Car_status='N';
    break;
  case 'P':
    Right();
    Serial.println("Right");
    Car_status='N';
    break;
  case 'Q':
    Left();
    Serial.println("Left");
    Car_status='N';
    break;
  case 'Y': // clockwise:上升
    CW();
    Car_status='N';
    break;
  case 'Z': // counterclockwise: 下降
    CCW();
    Car_status='N';
    break;
  case 'H':// stretch
    stretch();
    Car_status='N';
    break;
  case 'I':// shorten
    shorten();
    Car_status='N';
    break;
  case 'A': 
    stop_arm();
    Car_status = 'N';
    break;
  case 'C':
    claw();
    Car_status = 'N';
    break;
  case 'D':
    rele();
    Car_status = 'N';
    break;
  case 'v':
    Speed_value=0; //先將速度歸0
    do{
      if (BT.available()){
        Car_status =BT.read();  //讀取藍芽傳送字元
        Serial.println(Car_status); 
        //當藍芽讀取到字元'e'時，就會跳出迴圈。
        if(Car_status=='e'){        
          Serial.println(Speed_value);
          break;
        }         
        else
        //將傳送來的速度數值字元轉成整數。
        Speed_value=10*Speed_value+(Car_status-48); 
      }
    }while(true);      
  }
}

void claw(){ // changed
  myservo.write(180);
  delay_using_millis(200);
  }

void rele(){
  myservo.write(90);
  delay_using_millis(200);
  }

// API
int speed = 200;//original: 150
// pay attention to max range
// 255-70 = 185
void Forward(){
  L1_forward(speed);
  L2_forward(speed);
  R1_forward(speed);
  R2_forward(speed);
  delay_using_millis(200);
  }
void Backward(){
  L1_backward(speed);
  R1_backward(speed);
  L2_backward(speed);
  R2_backward(speed);
  delay_using_millis(200);
  }

void Left(){//平移
  L1_backward(250);
  R1_forward(250);
  L2_forward(250);
  R2_backward(250);
  delay_using_millis(200);
  }
void Right(){//平移

  L1_forward(250);
  R1_backward(220);//1:後輪
  L2_backward(250);
  R2_forward(220);//since it's too fast
  delay_using_millis(200);
  }

void TurnRight(){//#at same place
  L1_forward(speed);
  R1_backward(speed);
  L2_forward(speed);
  R2_backward(speed);
  delay_using_millis(200);
  }

void TurnLeft(){
  L1_backward(speed);
  R1_forward(speed);
  L2_backward(speed);
  R2_forward(speed);
  delay_using_millis(200);
  }

// Detail
void L1_forward(int sp)//左前轮前进
{
  digitalWrite(IN5,LOW);
  digitalWrite(IN6,HIGH);
  analogWrite(MT_LF,sp);
}
void R1_forward(int sp)//右前轮前进
{
  digitalWrite(IN7,HIGH);
  digitalWrite(IN8,LOW);
  analogWrite(MT_RF,sp);
}
void L2_forward(int sp)//左后轮前进
{
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  analogWrite(MT_L,sp);
}
void R2_forward(int sp)//右后轮前进
{
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  analogWrite(MT_R,sp+10);
}
void allstop()
{
  digitalWrite(IN5,LOW);
  digitalWrite(IN6,LOW);  
  digitalWrite(IN7,LOW);
  digitalWrite(IN8,LOW);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
}
void L1_backward(int sp)//左前轮后退
{
  digitalWrite(IN5,HIGH);
  digitalWrite(IN6,LOW);
  analogWrite(MT_LF,sp);
}
void R1_backward(int sp)//右前轮后退
{
  digitalWrite(IN7,LOW);
  digitalWrite(IN8,HIGH);
  analogWrite(MT_RF,sp);
}
void L2_backward(int sp)//左后轮后退
{
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  analogWrite(MT_L,sp);
}
void R2_backward(int sp)//右后轮后退
{
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  analogWrite(MT_R,sp);
}
void CW(){// clockwise
  // 设置电机顺时针旋转
  digitalWrite(dirPin1,LOW); 
  digitalWrite(dirPin2,LOW); 
  digitalWrite(dirPin3,LOW); 
  // 电机慢速旋转
  while(1){
    Car_status = BT.read();//****
    if(Car_status == 'A'){
      Car_status = 'N';
      stop_arm();
      break;
      }
    digitalWrite(stepPin1,HIGH); 
    delayMicroseconds(800); // original 1000
    digitalWrite(stepPin1,LOW); 
    delayMicroseconds(800); 
    digitalWrite(stepPin2,HIGH); 
    delayMicroseconds(800); 
    digitalWrite(stepPin2,LOW); 
    delayMicroseconds(800);
    digitalWrite(stepPin3,HIGH); 
    delayMicroseconds(800); 
    digitalWrite(stepPin3,LOW); 
    delayMicroseconds(800);  
    //delay_using_millis(200);
  }
  delay_using_millis(100);
  }

void CCW(){// counterclockwise
  digitalWrite(dirPin1,HIGH);
  digitalWrite(dirPin2,HIGH);
  digitalWrite(dirPin3,HIGH);
  // 电机快速旋转
  while(1){
    Car_status = BT.read();
    if(Car_status == 'A'){
      stop_arm();
      Car_status = 'N';
      break;
      }
    digitalWrite(stepPin1,HIGH);
    delayMicroseconds(800);
    digitalWrite(stepPin1,LOW);
    delayMicroseconds(800);
    digitalWrite(stepPin2,HIGH);
    delayMicroseconds(800);
    digitalWrite(stepPin2,LOW);
    delayMicroseconds(800);
    digitalWrite(stepPin3,HIGH);
    delayMicroseconds(800);
    digitalWrite(stepPin3,LOW);
    delayMicroseconds(800);
  }
  delay_using_millis(100);
  }
void stop_arm(){
  digitalWrite(dirPin1,LOW);
  digitalWrite(stepPin1,LOW);
  digitalWrite(dirPin2,LOW);
  digitalWrite(stepPin2,LOW);
  digitalWrite(dirPin3,LOW);
  digitalWrite(stepPin3,LOW);
  digitalWrite(IN9,LOW);
  digitalWrite(IN10,LOW);
  }

void stretch(){
  digitalWrite(IN9,HIGH);
  digitalWrite(IN10,LOW);
  analogWrite(ENE,150);
  }

void shorten(){
  digitalWrite(IN10,HIGH);
  digitalWrite(IN9,LOW);
  analogWrite(ENE,200);
  }
void delay_using_millis(unsigned long period){
  unsigned long timing = millis();
  while(millis() < timing + period );
  }
