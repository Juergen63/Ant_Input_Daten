#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define POT A0
#define ENTER_BTN 8
//Reset Button uno:RESET to Ground
//Colors Potentiometer
// 5V -> yellow (rd)
// GND -> white (or)
// A0 -> brown (bl)

#define LEFT_STOP -150   // in GRAD x-360 (210°)
#define RIGHT_STOP 209   // in GRAD (209°)
#define GRAD_SIGN 223    // °
int TO_LEFT_SIGN = 60;   // <
int EQUAL_SIGN = 61;     // =
int TO_RIGHT_SIGN = 62;  // >

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLUMNS, LCD_ROWS);

const unsigned long interval_lcd = 50;

int potValue, pos, grad, newAz, newAz2, remapPotValue;

int min_pot = 0;
int max_pot = 1023;

String msg, subStrg;

char buffer_0 [15]; //Display Buffer Row 1
char buffer_1 [15]; //Display Buffer Row 2
char leftDirection, rightDirection;

void setup() {
  pinMode(POT, INPUT);
  pinMode(ENTER_BTN, INPUT_PULLUP); //no external resistor needed

  Wire.begin ();
  Serial.begin(9600);

  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();

  //Init Start End Position
  //Read the Potentiometer Value 
  //Set with Enter Button
  lcd.setCursor(0,0);
  sprintf(buffer_0,"Start Position");
  lcd.print(buffer_0);
  
  while (digitalRead(ENTER_BTN)); //wait for Start Position
  delay(100);
  min_pot = analogRead(POT);
  sprintf(buffer_0,"START SET  %4.d",min_pot);
  lcd.setCursor(0,0);
  lcd.print(buffer_0);
  delay(100);
  lcd.setCursor(1,1);
  sprintf(buffer_1,"Turn To End ");
  lcd.print(buffer_1);
  delay(100);
  while (digitalRead(ENTER_BTN)); //wait for End Position
  max_pot = analogRead(POT);
  sprintf(buffer_0,"END SET    %4.d",max_pot);
  lcd.setCursor(0,0);
  lcd.print(buffer_0);
  delay(100);
  lcd.init();
  sprintf(buffer_1,"Start%3d End%4d",min_pot,max_pot);
  lcd.setCursor(0,1);
  lcd.print(buffer_1);
  delay(500);
  //while (digitalRead(ENTER_BTN));
  //End Startup

}

void loop() 
{
  static unsigned long previousMillis_lcd = 0;
  unsigned long currentMillis = millis();
  if (Serial.available() != 0) 
  {    //wait for data available   
    msg = Serial.readString();
    subStrg = (msg.substring(1,4));
    newAz = subStrg.toInt();
  }
  potValue = analogRead(POT);
  pos = map(potValue,min_pot,max_pot,LEFT_STOP,RIGHT_STOP);
  //                     0,    1023,  -155(205)    195
  if (pos < 0)
  {
    grad = pos + 360; 
  }else
  {
    grad = pos;
  }
  if (newAz >= 210)
  {
      newAz2 = newAz - 360;
  }else
  {
    newAz2 = newAz;
  }
  remapPotValue = map(newAz2,LEFT_STOP,RIGHT_STOP,min_pot,max_pot);
  //Direction Signs Calculation
  if (potValue < remapPotValue)
  {
    leftDirection = char(20);
    rightDirection = char(62);
  }else
  {
    leftDirection = char(60);
    rightDirection = char(20);
  }
  //Write Display
  if (currentMillis - previousMillis_lcd >= interval_lcd) 
  {
    sprintf(buffer_0,"AntPosition%4d%1c",grad,char(GRAD_SIGN));
    lcd.setCursor(0,0);
    lcd.print(buffer_0);
    sprintf(buffer_1,"%1c  NewAz%4d%1c  %1c",leftDirection,newAz,char(GRAD_SIGN),rightDirection);
    lcd.setCursor(0,1);
    lcd.print(buffer_1); 
  
    previousMillis_lcd = currentMillis;
 }
  
}
