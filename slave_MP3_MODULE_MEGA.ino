///              MP3 PLAYER PROJECT
/// http://educ8s.tv/arduino-mp3-player/
//////////////////////////////////////////
#include <Wire.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "SoftwareSerial.h"
void receiveEvent(int howMany);
void bcd(int selector);
void playFirst();
void requestEvent();
int buzzer = 14; 
SoftwareSerial mySerial(52, 50);
# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]
const int A = 46;
const int B = 47;
const int C = 42;
const int D = 44;
const int E = 45;
const int F = 43;
const int G = 40;

const int IN1 = 30; //right back wheel Forward
const int IN2 = 31; //right back wheel  Reverse
const int IN3 = 32; //right front wheel  Forward
const int IN4 = 33; //right front wheel  Reverse
//To turn motor on, off and control speed
int en3 = 6; //enable for right back wheel pwm
int en4 = 7; //enable for right back wheel pwm
int en2 = 4; //enable for right front wheel  pwm
int en1 = 5; //enable for right front wheel  pwm

const int IN11 = 34; //left front wheel Forward
const int IN22 = 35; //left front wheel Reverse
const int IN33 = 36; //left back wheel Forward
const int IN44 = 37; //left back wheel Reverse
//To turn motor on, off and control speed
int en33 = 11;  // enable for left back wheel
int en44 = 10;  // enable for left back wheel
int en22 = 9;   // enable for left front wheel
int en11 = 8;   // enable for left front wheel

int QRE1113_Pin = 22; //connected to digital 1
int QRE1114_Pin = 24; //connected to digital 2

boolean dfPlayerStarted = false;
int songNumber = 0;
//task definitions:

void ldr ( void *pvParameters);
void readFromScreen ( void *pvParameters);


# define ACTIVATED LOW


boolean isPlaying = false;
//#if 1


#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini myDFPlayer;
SemaphoreHandle_t Mutex;
int currGear = 1;
int prevGear = 0;
void setup () {
  pinMode(A2, INPUT); //x
  pinMode(A1, INPUT); //y
  // pinMode( ,INPUT);

  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
   pinMode(buzzer, OUTPUT);

  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
  pinMode (en1, OUTPUT);
  pinMode (en2, OUTPUT);
  pinMode (en3, OUTPUT);
  pinMode (en4, OUTPUT);

  pinMode (IN11, OUTPUT);
  pinMode (IN22, OUTPUT);
  pinMode (IN33, OUTPUT);
  pinMode (IN44, OUTPUT);
  pinMode (en11, OUTPUT);
  pinMode (en22, OUTPUT);
  pinMode (en33, OUTPUT);
  pinMode (en44, OUTPUT);

  Serial.begin(115200);
  mySerial.begin(9600);
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  // pinMode(A6,INPUT);
  //  pinMode(A5,OUTPUT);
  if (!myDFPlayer.begin(mySerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    //while(true);
  }
  delay(1000);
  dfPlayerStarted= true;
  playFirst();
  isPlaying = true;
  Mutex = xSemaphoreCreateMutex();

  xTaskCreate(ldr, "ldr", 500, NULL, 2, NULL);
  xTaskCreate(joystick, "joystick", 500, NULL, 2, NULL);
  xTaskCreate(motor,"motor",500,NULL,3,NULL);
  xTaskCreate(getSong,"getSong",500,NULL,3,NULL);
}
void getSong ( void *pvParameters) {
    TickType_t joyStickWakeTime;
  const TickType_t joyStickDelay = pdMS_TO_TICKS(1500);
  joyStickWakeTime = xTaskGetTickCount();
while(1){
  songNumber = myDFPlayer.readCurrentFileNumber();
   vTaskDelayUntil(&joyStickWakeTime , joyStickDelay);
}
}
void joystick ( void *pvParameters) {
  TickType_t joyStickWakeTime;
  const TickType_t joyStickDelay = pdMS_TO_TICKS(1000);
  joyStickWakeTime = xTaskGetTickCount();
  while (1) {
//    Serial.println("JOYSTICK");
    int x = analogRead(A2);
    int y = analogRead(A1); //assign value of LDR sensor to a temporary variable
//    Serial.print("X"); Serial.println(x);
//    Serial.print("Y"); Serial.println(y);
    if (y > 800)
      bcd(1);
    else if (y < 300)
      bcd(3);
    else if (x < 300)
      bcd(2);
    else if (x > 800)
      bcd(4);
    //xSemaphoreGive(Mutex);
    vTaskDelayUntil(&joyStickWakeTime , joyStickDelay);
  }
}
void ldr ( void *pvParameters) {
  TickType_t ldrWakeTime;
  const TickType_t ldrDelay = pdMS_TO_TICKS(750);
  ldrWakeTime = xTaskGetTickCount();
  while (1) {
    //Serial.println("ldr");
    //xSemaphoreTake(Mutex, portMAX_DELAY);
    int temp = analogRead(A0); //input ldr pin      //assign value of LDR sensor to a temporary variable
    //  Serial.println("Intensity="); //print on serial monitor using ""
    //  Serial.println(temp);         //display output on serial monitor
    //HIGH means,light got blocked
    analogWrite(12, temp / 4); //pin 12 for led        //if light is not present,LED on
    //xSemaphoreGive(Mutex);
    vTaskDelayUntil(&ldrWakeTime , ldrDelay);

  }
}

void motor( void *pvParameters) {
  TickType_t motorLastWakeTime;
  const TickType_t motorDelay = pdMS_TO_TICKS(100);
  motorLastWakeTime = xTaskGetTickCount();
  while (1) {
   // Serial.println("motor");
    //control speed

    int QRE_Value2 = readQD();//left line follower
     Serial.println("value2");
      Serial.println(QRE_Value2);
    if (QRE_Value2 >
    2999) { //left line follower detected line so we need to steer to the right, dec pwm on left wheels
     
      analogWrite(en1, 255);
      analogWrite(en2, 255);
      analogWrite(en3, 255);
      analogWrite(en4, 255);
      //Serial.println(en1);

      //control direction
     digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

      //control speed REVERSE HERE
      analogWrite(en11, 255);
      analogWrite(en22, 255);
      analogWrite(en33, 255);
      analogWrite(en44, 255);
      //control direction
    digitalWrite(IN11, LOW);
    digitalWrite(IN22, HIGH);
    digitalWrite(IN33, HIGH);
    digitalWrite(IN44, LOW);
      // delay(500);
      digitalWrite(buzzer, HIGH);
    }


    int QRE_Value = readQD2();
    //Serial.println(QRE_Value);
      Serial.println("value1");
      Serial.println(QRE_Value);
    if (QRE_Value >2999) { //right line follower detected line so we need to steer to the left, dec pwm on right wheels
   //REVERSE
      analogWrite(en1, 255);
      analogWrite(en2, 255);
      analogWrite(en3, 255);
      analogWrite(en4, 255);
      //control direction
  digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

      //control speed
      analogWrite(en11, 255);
      analogWrite(en22, 255);
      analogWrite(en33, 255);
      analogWrite(en44, 255);
      //control direction
  digitalWrite(IN11, HIGH);
    digitalWrite(IN22, LOW);
    digitalWrite(IN33, LOW);
    digitalWrite(IN44, HIGH);
      //delay(500);
digitalWrite(buzzer, HIGH);
    }





if(QRE_Value <=2999 &&QRE_Value2 <=2999){
  digitalWrite(buzzer, LOW);
    analogWrite(en1, 180);
    analogWrite(en2, 180);
    analogWrite(en3, 180);
    analogWrite(en4, 180);
    //control direction
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    //control speed
    analogWrite(en11, 180);
    analogWrite(en22, 180);
    analogWrite(en33, 180);
    analogWrite(en44, 180);
    //control direction
    digitalWrite(IN11, HIGH);
    digitalWrite(IN22, LOW);
    digitalWrite(IN33, LOW);
    digitalWrite(IN44, HIGH);
}
    vTaskDelayUntil(&motorLastWakeTime , motorDelay);
  }
}


int readQD(){
//Returns value from the QRE1113
//Lower numbers mean more refleacive
//More than 3000 means nothing was reflected.
pinMode( QRE1113_Pin, OUTPUT );
digitalWrite( QRE1113_Pin, HIGH );
delayMicroseconds(10);
pinMode( QRE1113_Pin, INPUT );

long time = micros();

//time how long the input is HIGH, but quit after 3ms as nothing happens after that
while (digitalRead(QRE1113_Pin) == HIGH && micros() - time < 3000); 
int diff = micros() - time; return diff; }

void requestEvent() {
   if (dfPlayerStarted) {  //Use softwareSerial to communicate with mp3.
    //  Serial.println("dfplayer shaghal");
      Wire.write(songNumber);

       
//  int songNumber = 1;
// Wire.write(songNumber);
  }else{
     // Serial.println("dfplayer mesh shaghal");
      Wire.write(-1);
}
//Serial.println("on request");
  //int songNumber = myDFPlayer.readCurrentFileNumber();

  
  //Serial.println(songNumber);
}
int readQD2(){
//Returns value from the QRE1113
//Lower numbers mean more refleacive
//More than 3000 means nothing was reflected.
pinMode( QRE1114_Pin, OUTPUT );
digitalWrite( QRE1114_Pin, HIGH );
delayMicroseconds(10);
pinMode( QRE1114_Pin, INPUT );

long time = micros();

//time how long the input is HIGH, but quit after 3ms as nothing happens after that
while (digitalRead(QRE1114_Pin) == HIGH && micros() - time < 3000); 
int diff = micros() - time; return diff; }

void receiveEvent(int howMany)
{

  int x = Wire.read();    // receive byte as an integer
  //Serial.println(x);         // print the integer
  switch (x) {
    case 0:{
      // statements
      isPlaying=false;
      pause();}
      break;
    case 1:{
      // statements
      play();
      isPlaying=true;
    }
      break;
    case 2:
      // statements
      if(isPlaying){
      playNext();
      }
      break;
    case 3:
      // statements
      if(isPlaying){
      playPrevious();}
      break;
    default:
      // statements
      break;
  }
}

String curr = "";
void loop () {

}

void playFirst()
{
  execute_CMD(0x3F, 0, 0);
  //  delay(500);
//  setVolume(30);
  //  delay(500);
  execute_CMD(0x11, 0, 1);
  delay(500);
}

void pause()
{
  execute_CMD(0x0E, 0, 0);
//  delay(500);
}

void play()
{
  execute_CMD(0x0D, 0, 1);
//  setVolume(30);
//  delay(500);
}

void playNext()
{
  execute_CMD(0x01, 0, 1);
//  setVolume(30);
//  delay(500);
}

void playPrevious()
{
  execute_CMD(0x02, 0, 1);
//  delay(500);
}

void setVolume(int volume)
{
  execute_CMD(0x06, 0, volume); // Set the volume (0x00~0x30)
//  delay(2000);
}

void execute_CMD(byte CMD, byte Par1, byte Par2)
// Excecute the command and parameters
{
  // Calculate the checksum (2 bytes)
  word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
  // Build the command line
  byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
                            Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte
                          };
  //Send the command line to the module
  for (byte k = 0; k < 10; k++)
  {
    mySerial.write( Command_line[k]);
  }
}

void bcd(int selector) {
  if (selector == 1 ) {
    currGear = 1;
    digitalWrite(A, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(E, HIGH);
    digitalWrite(F, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(B, LOW);
    digitalWrite(C, LOW);
  }
  else if (selector == 2) {
    currGear = 2;
    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
    digitalWrite(C, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(F, HIGH);
    digitalWrite(E, LOW);
    digitalWrite(D, LOW);
  }
  else if (selector == 3) {
    currGear = 3;
    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
    digitalWrite(C, LOW);
    digitalWrite(D, LOW);
    digitalWrite(E, HIGH);
    digitalWrite(F, HIGH);
    digitalWrite(G, LOW);
  }
  else if (selector == 4) {
    currGear = 4;
    digitalWrite(F, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);
    digitalWrite(C, LOW);
    digitalWrite(A, HIGH);
    digitalWrite(D, HIGH);
    digitalWrite(E, HIGH);

  }
}
