//Made By Aiden Nygaard
#include "Quizshow_Helper.h"

//############################################################

const int buttonamount = 6;
//INDEX: {ButtonPin, Pressduration, Lastdurationupdate}
long buttonpins[buttonamount][3] = { 
  {12, 0, 0},  //  ID: 0
  {11, 0, 0},  //  ID: 1
  {10, 0, 0},  //  ID: 2
  {9, 0, 0},   //  ID: 3
  {8, 0, 0},   //  ID: 4
  {7, 0, 0}    //  ID: 5
};

const int playerbuttonamount = 4;
//INDEX: {buttonpin arrayid, RGBLedid, ColorId, isReqisted, Points}
int playerbuttons[playerbuttonamount][5] = 
{
  {0, 0, 7, -1, 0},
  {1, 1, 8, -1, 0},
  {2, 2, 6, -1, 0},
  {3, 3, 4, -1, 0}
};

const int moderatorbuttonamount = 2;
//INDEX: {buttonpin arrayid, buttoncolor}
int moderatorbuttons[moderatorbuttonamount][2] = 
{
  {4, 0},
  {5, 1}
};

//############################################################

enum scenarios {
    SETUP_ROUNDS = 1, 
    SETUP_TIME = 2,
    PLAYERREGISTER = 3,
    WAITBUZZ = 4,
    SOMEONEBUZZED = 5,
    END = 6,
    CHANGESETUPREQUEST = 7
};

int currentscenario = PLAYERREGISTER;

int playeridwhobuzzed = -1;

//############################################################

void setup()
{
  Serial.begin(57600);
  //Inizialisiert die Intgrierte Bibliotheke
  inithelperlib();

  //Inizialisiert die Button Pins
  for (int i = 0; i < buttonamount; i++)
  {
    pinMode(buttonpins[i][0], INPUT);
  }

  //Testet ob alle RGB LEDS leuchten
  for(int i = 0; i < playerbuttonamount; i++)
  {
    delay(500);
    updatePixel(i, 8);
  }
  delay(500);

  buzz(500);
  clearPixels(true);
}

void loop() 
{
  for (int buttonid = 0; buttonid < buttonamount; buttonid++)
  {
    const long buttonpinid = buttonpins[buttonid][0];
    if (digitalRead(buttonpinid) == HIGH)
    {
      const long timepressed = millis() - buttonpins[buttonid][1];
      if(timepressed > 11)
      {
        const long lastupdated = buttonpins[buttonid][2] - buttonpins[buttonid][1];
        if(lastupdated <= 500 && timepressed >= 500)
        {
          //Wird ein mal ausgeführt wenn der knopf 0.5 sekunden gedrückt wurde
          switch(currentscenario)
          {
            case SETUP_ROUNDS:        
              break;
              
            case SETUP_TIME:
              break;

            case PLAYERREGISTER:
              break;

            case SOMEONEBUZZED:
              break;

            case END:
              break;
          }
        }
        
        //Wird Immer ausgeführt wenn ein knopf gedrückt wird 
      }
      else
      {
        //Wird ein mal ausgeführt wenn der knopf dedrückt wurde
        switch(currentscenario)
        {
          case SETUP_ROUNDS:          
            break;
            
          case SETUP_TIME:
            break;

          case PLAYERREGISTER:
            if(isPlayerButton(buttonid))
            {
              const int playerbuttonid = getPlayerButtonId(buttonid);
              if(!isPlayerButtonRegistered(playerbuttonid))
              {
                updatePixel(playerbuttons[playerbuttonid][1], playerbuttons[playerbuttonid][2], true);
                playerbuttons[playerbuttonid][3] = 1;
                buzz(250);
              }
            }
            break;
            
          case WAITBUZZ:
            if(isPlayerButton(buttonid))
            {
              const int playerid = getPlayerButtonId(buttonid);
              if(isPlayerButtonRegistered(playerid))
              {
                clearPixels(true);
                updatePixel(playerbuttons[playerid][1], playerbuttons[playerid][2], true);
                buzz(600);
                playeridwhobuzzed = playerid;
                currentscenario = SOMEONEBUZZED;
              }
            }
            break;
            
          case SOMEONEBUZZED:
            break;
             
          case END:
            break;

          case CHANGESETUPREQUEST:
            break;
        }
      }
    }
    else
    {
      buttonpins[buttonid][1] = millis();
    }
    buttonpins[buttonid][2] = millis();
  }

  
  tick();
  delay(10);
}

boolean isPlayerButtonRegistered(int playerbutton)
{
  return playerbuttons[playerbutton][3] > 0;
}

boolean isPlayerButton(int buttonid)
{
  return getPlayerButtonId(buttonid) >= 0;
}

int getPlayerButtonId(int buttonid)
{
  for(int i = 0; i < playerbuttonamount; i++)
  {
    if(playerbuttons[i][0] == buttonid)
    {
      return i;
    }
  }
  return -1;
}
