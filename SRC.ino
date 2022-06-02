//Made By Aiden Nygaard
#include "Quizshow_Helper.h"

const int buttonamount = 6;
//INDEX: {ButtonPin, }
const int buttonpins[buttonamount] = 
{
  12, 
  11,
  10,
  9,
  8,
  7
};

const int playerbuttonamount = 4;
//INDEX: {buttonpin arrayid, RGBLedid}
const int playerbuttons[playerbuttonamount][2] = 
{
  {0, 0},
  {1, 1},
  {2, 2},
  {3, 3},
};

int registeredplayers[playerbuttonamount][3] = 
{
  
};

//############################################################

void setup()
{
  Serial.begin(9600);
  inithelperlib();
  
  for (int i = 0; i < buttonamount; i++)
  {
    pinMode(buttonpins[i][0], INPUT);
  }
  
  buzz(500);
  clearpixels();
}

void loop() 
{
  for (int i = 0; i < buttonamount; i++)
  {
    const buttonpinid = buttonpins[i][0];
    if (digitalRead(buttonpinid) == HIGH)
    {
      
      buzz(1000);
    }
  }

  tick();
}
