// PASSED
#include<SoftwareSerial.h>

// Wheels
#define MR 11//ENB
#define IN3 12
#define IN4 13

#define ML 10//ENA
#define IN1 2
#define IN2 3

// Cargo stack
#define dirPin1 43
#define stepPin1 42
#define dirPin2 41
#define stepPin2 40

// infrared : global is more stable
#define SensorLeft 53
#define SensorRight 52
int SL;
int SR;

// 顏色sensor
#define S0 30
#define S1 31
#define S2 32
#define S3 33
#define sensorOut 34  

//超音波
#define trigPin 6
#define echoPin 7 // digital input will do the job

void setup() {
  // color sensor
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // 設定 sensorOut 為輸出
  pinMode(sensorOut, INPUT);
  
  // 設定頻率尺度(frequency scaling)為 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  // button
  pinMode(46,INPUT);

  //Arduino 輸出電壓控制車子
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);

  // Cargo stack

  pinMode(dirPin1,OUTPUT);
  pinMode(stepPin1,OUTPUT);
  pinMode(dirPin2,OUTPUT);
  pinMode(stepPin2,OUTPUT);

  // infrared
  pinMode(SensorLeft, INPUT);
  pinMode(SensorRight, INPUT);

  //超音波
  pinMode(trigPin, OUTPUT); // 定義輸入及輸出 
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
}

void Release(){ // stack (rolling stick) not yet
  delay_using_millis(3000);
  delay_using_millis(3000);
}

void let_go(){
  digitalWrite(dirPin1,LOW);//clockwise
  digitalWrite(dirPin2,LOW);
  for(int x = 0; x < (200)*16; x++){// to be adjust to 'release time'
   digitalWrite(stepPin1,HIGH); 
   delayMicroseconds(800); 
   digitalWrite(stepPin1,LOW); 
   delayMicroseconds(800);
   digitalWrite(stepPin2,HIGH); 
   delayMicroseconds(800); 
   digitalWrite(stepPin2,LOW); 
   delayMicroseconds(800);
  }
  delay_using_millis(1000);
}
void claw_back(){
  digitalWrite(dirPin1,HIGH);//clockwise
  digitalWrite(dirPin2,HIGH);
  for(int x = 0; x < (200)*16; x++){// to be adjust to 'release time'
   digitalWrite(stepPin1,HIGH); 
   delayMicroseconds(800); 
   digitalWrite(stepPin1,LOW); 
   delayMicroseconds(800);
   digitalWrite(stepPin2,HIGH); 
   delayMicroseconds(800); 
   digitalWrite(stepPin2,LOW); 
   delayMicroseconds(800);
  }
  delay_using_millis(1000);
}

int Red(){  
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);  
  return pulseIn(sensorOut, LOW);
  }
int Green(){
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);  
  return pulseIn(sensorOut, LOW);
  }
int Blue(){
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);  
  return pulseIn(sensorOut, LOW);
  }

int Read_n_Judge_color(){
  if (31<=Red() && Red() <= 50 && 39<= Green() && Green() <= 91 && 30<= Blue() && Blue() <= 69){
    Serial.println("Red");
    return 1;
    }
  else if (51<=Red() && Red() <= 80 && 47<= Green() && Green() <= 67 && 40<= Blue() && Blue() <= 64){
    Serial.println("Green");
    return 2;
    }
  else {
    return 3;
    }
  }

void delay_using_millis(unsigned long period){
  unsigned long timing = millis();
  while(millis() < timing + period );
  }

void Move(){
  // change to global doesn't improved significantly
  SL = digitalRead(SensorLeft); 
  SR = digitalRead(SensorRight);
  if (SL == 0 && SR == 0) {
    Stop_it();
    delay_using_millis(50);
    Forward(230);//200 original
    delay_using_millis(80);
  }
  if (SL == 1 && SR == 0) {
    Stop_it();
    delay_using_millis(50);
    Left(230);
    delay_using_millis(50);
  }
  if (SL == 0 && SR == 1) {
    Stop_it();
    delay_using_millis(50);
    Right(230);
    delay_using_millis(50);
  }
  if (SL == 1 && SR == 1) {
    Stop_it();
    delay_using_millis(50);
  }
}

void Forward(unsigned int sp){
  analogWrite(ML,sp);
  analogWrite(MR,sp);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  }
void Stop_it(){
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);  
  }
void Right(unsigned int sp){
  analogWrite(ML,sp);
  analogWrite(MR,sp);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  }
void Left(unsigned int sp){
  analogWrite(ML,sp);
  analogWrite(MR,sp);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  }

int ultra(){
  long duration, cm, inches;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);     // 給 Trig 高電位，持續 10微秒
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);             // 讀取 echo 的電位
  duration = pulseIn(echoPin, HIGH);   // 收到高電位時的時間
 
  cm = (duration/2) / 29.1;         // 將時間換算成距離 cm 或 inch  
  inches = (duration/2) / 74; 

  Serial.print("Distance : ");  
  Serial.print(inches);
  Serial.print("in,   ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  
  delay_using_millis(250);
  return cm;
  }


int num_red = 0;// number of reds read
int num_task = 0;// order of task: number of green read

void loop (){
  Read_n_Judge_color();

  Move();// maybe here

  if(Read_n_Judge_color() == 2){ // green
    num_task += 1;
    num_red = 0;
    delay_using_millis(50);
// kept this just in case
//    while(digitalRead(46)!=1){ // stop when button not pushed
//      Stop_it();
//      }
//    Serial.println("Was pushed!");
    while(ultra()>8){
      Stop_it();
      Serial.println("Stop");
    }
    Serial.println("!!!GO!!!");
    Serial.println("Detected! start to move");
    Move();
    delay_using_millis(50);
    }

  if(Read_n_Judge_color() == 1){ // red 
    num_red += 1;
    delay_using_millis(50);

    if(num_task == num_red){
      Stop_it();
      delay_using_millis(50);

      let_go();
      claw_back();
      Move();

      delay_using_millis(350);
      }
    while(Read_n_Judge_color()!=3){
      Read_n_Judge_color();
      Move();
      delay_using_millis(50);
      }
    }

}
