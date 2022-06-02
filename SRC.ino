//Made By Aiden Nygaard
#include "Quizshow_Helper.h"

const int buttonamount = 6;
//INDEX: {ButtonPin, Pressduration, lastpressupdate}
long buttonpins[buttonamount][3] = {
  {12, 0, 0}, 
  {11, 0, 0},
  {10, 0, 0},
  {9, 0, 0},
  {8, 0, 0},
  {7, 0, 0}
};

const int playerbuttonamount = 4;
//INDEX: {buttonpin arrayid, RGBLedid}
const int playerbuttons[playerbuttonamount][2] = 
{
  {0, 0},
  {1, 1},
  {2, 2},
  {3, 3}
};

int registeredplayers[playerbuttonamount][2] = 
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
    const long buttonpinid = buttonpins[i][0];
    if (digitalRead(buttonpinid) == HIGH)
    {
      const long timepressed = millis() - buttonpins[i][1];
      if(timepressed > 0)
      {
        const long lastpressupdate = timepressed - (millis() - buttonpins[i][2]);
        if(lastpressupdate <= 1000 && timepressed >= 1000)
        {
          //Wird ein mal ausgeführt wenn der knopf 1 Sekunde dedrückt wurde
          buzz(1000);
        }
        
        Serial.println(timepressed);
        //Wird wieder und wieder ausgefühlt wenn der knopf gedrückt wurde
      }
      else
      {
        //Wird ein mal ausgeführt wenn der knopf dedrückt wurde
      }

      buttonpins[i][2] = millis();
    }
    else
    {
      buttonpins[i][1] = millis();
    }
  }

  tick();
  delay(20);
}
