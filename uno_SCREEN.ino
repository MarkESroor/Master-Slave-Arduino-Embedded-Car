
// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]
void general ( void *pvParameters);
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>

#include <FreeDefaultFonts.h>
# define ACTIVATED LOW
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
char* songName="";
boolean isPlaying = false;
// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP = 8, XM = A2, YP = A3, YM = 9; //ID=0x9341
const int TS_LEFT = 135, TS_RT = 884, TS_TOP = 100, TS_BOT = 890;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void lcd( void *pvParameters);
SemaphoreHandle_t xSemaphore;

Adafruit_GFX_Button on_btn, off_btn, on_btn2, off_btn2;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);      //restore shared pins
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);   //because TFT control pins
  digitalWrite(XM, HIGH);
  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {
    pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
    pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
  }
  return pressed;
}


void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)

  Serial.begin(9600);

  uint16_t ID = tft.readID();
  Serial.print("TFT ID = 0x");
  Serial.println(ID, HEX);
  showmsgXY(20, 60, 1, &FreeSans9pt7b, "FreeSans9pt7b");
  Serial.println("Calibrate for your Touch Panel");
  if (ID == 0xD3D3) ID = 0x9486; // write-only shield
  tft.begin(ID);
  //    tft.setRotation(2);            //PORTRAIT
  tft.fillScreen(BLACK);
  on_btn.initButton(&tft,  60, 200, 100, 40, WHITE, CYAN, BLACK, "Pause", 1);
  on_btn2.initButton(&tft,  60, 250, 100, 40, WHITE, CYAN, BLACK, "Play", 1);
  off_btn.initButton(&tft, 180, 200, 100, 40, WHITE, CYAN, BLACK, "Next", 1);
  off_btn2.initButton(&tft, 180, 250, 100, 40, WHITE, CYAN, BLACK, "Previous", 1);
  on_btn.drawButton(false);
  on_btn2.drawButton(false);
  off_btn.drawButton(false);
  off_btn2.drawButton(false);
 // tft.fillRect(40, 80, 160, 80, RED);
  //xTaskCreate(lcd, "lcd", 2000, NULL, 2, NULL);0xffff 
  //xTaskCreate(songNumber, "songNumber", 2000, NULL, 3, NULL);
  Serial.println("before");
  //Serial.println(xTaskCreate(general, "general", 1500, NULL, 3, NULL));
  Serial.println("after");
}
int currSong = 0;
int prevSong = 0;


byte x = 0;

void general ( void *pvParameters) {
  Serial.println("before loop");
while(1){
 
}
}

void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg)
{
  int16_t x1, y1;
  uint16_t wid, ht;
  tft.drawFastHLine(0, y, tft.width(), WHITE);
  tft.setFont(f);
  tft.setCursor(x, y);
  tft.setTextColor(WHITE);
  tft.setTextSize(sz);
  tft.print(msg);

}
void loop()
{
 //Serial.println("hi");
  Wire.beginTransmission(4); // transmit to device #4
    bool down = Touch_getXY();
    on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
    off_btn.press(down && off_btn.contains(pixel_x, pixel_y));
    if (on_btn.justReleased())
      on_btn.drawButton(false);
    if (off_btn.justReleased())
      off_btn.drawButton(false);
    if (on_btn.justPressed()) {
      on_btn.drawButton(true);
        Wire.write(0); //pause
        Serial.println("0");

      }
    
    if (off_btn.justPressed()) {
      off_btn.drawButton(true);
    //  tft.fillRect(40, 80, 160, 80, RED);
      //if (isPlaying)
      //{
        Wire.write(2); //playNext
         Serial.println("2");
      //}
    }

    on_btn2.press(down && on_btn2.contains(pixel_x, pixel_y));
    off_btn2.press(down && off_btn2.contains(pixel_x, pixel_y));
    if (on_btn2.justReleased())
      on_btn2.drawButton(false);
    if (off_btn2.justReleased())
      off_btn2.drawButton(false);
    if (on_btn2.justPressed()) {
      on_btn2.drawButton(true);
          Wire.write(1); //play
         Serial.println("1");
      //tft.fillRect(40, 80, 160, 80, GREEN);
    }
    if (off_btn2.justPressed()) {
      off_btn2.drawButton(true);
      //tft.fillRect(40, 80, 160, 80, RED);
      //if (isPlaying)
      //{
        Wire.write(3); //playPrev
      //}
    }
    
    showmsgXY(20, 60, 1, &FreeSans9pt7b, songName);
      
    //
    //vTaskDelayUntil(&lcdWake , lcdDelay);
    Wire.requestFrom(4, 1);
    while (Wire.available()) {
      //Serial.print("read value ");
      currSong = Wire.read();
      //Serial.println(currSong);
      break;
    }
     Wire.endTransmission(); 
//
switch (currSong) {
  case (-1):
    songName = "Waiting for mp3 player";
    
    break;
  case 1:
    songName = "Wonderful World";
    break;
  case 2:
    // statements
     songName = "Sunflower";
    break;
      case 3:
    // statements
     songName = "Memories";
    break;
      case 4:
    // statements
     songName = "Rather Be";
    break;
      case 5:
    // statements
     songName = "Stand By Me";
    break;
  default:
    // statements
    songName = "Waiting";
    break;
}
  if(currSong!= prevSong){
    tft.fillRect(10, 40, 200, 30, BLACK);
  }

    prevSong=currSong;
 
}
