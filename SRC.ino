//Made By Aiden Nygaard
#include "Quizshow_Helper.h"

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//############################################################

const int buttonamount = 6;
//INDEX: {ButtonPin, Pressduration, Lastdcheckupdate}
long buttonpins[buttonamount][3] = { 
  {12, 0, 0},  //  ID: 0
  {11, 0, 0},  //  ID: 1
  {10, 0, 0},  //  ID: 2
  {9, 0, 0},   //  ID: 3
  {8, 0, 0},   //  ID: 4
  {7, 0, 0}    //  ID: 5
};

const int playerbuttonamount = 4;
//INDEX: {buttonpin arrayid, RGBLedid, ColorId, isRequested, Points}
int playerbuttons[playerbuttonamount][5] = 
{
  {0, 0, 7, -1, 0},
  {1, 1, 8, -1, 0},
  {2, 2, 6, -1, 0},
  {3, 3, 4, -1, 0}
};

const int modgreenbuttonid = 4;
const int modredbuttonid = 5;

//############################################################

enum scenarios {
    SETUP_ROUNDS = 1, 
    SETUP_TIME = 2,
    PLAYERREGISTER = 3,
    WAITBUZZ = 4,
    PLAYERBUZZED = 5,
    END = 6,
    CHANGESETUPREQUEST = 7
};

int currentscenario = SETUP_ROUNDS;

int maxrounds = 10;
long maxanswertime = 3000; //In Millis

int selectedplayerid = -1;
long playeranswertimestamp = -1; 
int currentround = 1;

//############################################################

void setup()
{
  Serial.begin(57600);
  //Inizialisiert die Intgrierte Bibliotheke
  inithelperlib();

  lcd.init();
  lcd.backlight();

  //Inizialisiert die Button Pins
  for (int i = 0; i < buttonamount; i++)
  {
    pinMode(buttonpins[i][0], INPUT);
  }

  //Testet ob alle RGB LEDS leuchten
  for(int i = 0; i < playerbuttonamount; i++)
  {
    updatePixel(i, 8);
    delay(500);
  }

  buzz(500);
  clearPixels(true);
}

void loop() 
{
  for (int buttonid = 0; buttonid < buttonamount; buttonid++)
  {
    if (isButtonPressed(buttonid))
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
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                currentscenario = SETUP_TIME;
                buzz(250);
              }
              break;
              
            case SETUP_TIME:
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                currentscenario = PLAYERREGISTER;
                buzz(250);
              }
              break;

            case PLAYERREGISTER:
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                currentscenario = WAITBUZZ;
                buzz(250);
              }
              break;

            case END:
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                currentscenario = CHANGESETUPREQUEST;
                buzz(250);
              }
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
            if(isModeratorButton(buttonid))
            {
              if(modgreenbuttonid == buttonid)
              {
                maxrounds++;
                buzz(100);
              }
              else if(modredbuttonid == buttonid)
              {
                if(maxrounds > 1)
                {
                  maxrounds--;
                  buzz(100);
                }
              }      
            }
            break;
            
          case SETUP_TIME:
            if(isModeratorButton(buttonid))
            {
              if(modgreenbuttonid == buttonid)
              {
                maxanswertime += 1000;
                buzz(100);
              }
              else if(modredbuttonid == buttonid)
              {
                if(maxanswertime - 1000 > 1000)
                {
                  maxanswertime -= 1000;
                  buzz(100);
                }
              }
            }      
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
                buzz(1000);
                selectedplayerid = playerid;
                playeranswertimestamp = millis() + maxanswertime;
                currentscenario = PLAYERBUZZED;
              }
            }
            break;
            
          case PLAYERBUZZED:
            if(isModeratorButton(buttonid))
            {
              if(modgreenbuttonid == buttonid)
              {
                playerbuttons[selectedplayerid][4] = playerbuttons[selectedplayerid][4] + 1;
              }
              
              if(currentround >= maxrounds)
              {
                 currentscenario = END;
              }
              else
              {
                currentround++;
                currentscenario = WAITBUZZ;
              }
            }
            break;
             
          case END:
            
            break;

          case CHANGESETUPREQUEST:
            if(isModeratorButton(buttonid))
            {
              if(modgreenbuttonid == buttonid)
              {
                currentscenario = SETUP_ROUNDS;
              }
              else if(modredbuttonid == buttonid)
              {
                currentscenario = PLAYERREGISTER;
              }      
              clearCache();
            }
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

  lcd.clear();
  
  switch(currentscenario)
  {
    case SETUP_ROUNDS:
      lcd.print("SETUP-RUNDEN:");
      lcd.setCursor(0, 1);
      lcd.print(maxrounds);
      break;

    case SETUP_TIME:
      lcd.print("SETUP-ZEIT:");
      lcd.setCursor(0, 1);
      lcd.print(maxanswertime);
      break;

    case WAITBUZZ:
      lcd.print("Runde:");
      lcd.setCursor(6, 0);
      lcd.print(currentround);
      lcd.setCursor(0, 1);
      lcd.print("WAIT 4 BUZZ");
      break;

    case PLAYERBUZZED:
      lcd.print(getColorNameById(playerbuttons[selectedplayerid][2]));
      lcd.setCursor(0, 1);
      const long remainingtime = playeranswertimestamp - millis();
      if(remainingtime > 0)
      {
        lcd.print(remainingtime / 1000);
        lcd.setCursor(2, 1);
        lcd.print("-SEKUNDEN");
      }
      else
      {
        if(currentround >= maxrounds)
        {
           currentscenario = END;
        }
        else
        {
          currentround++;
          currentscenario = WAITBUZZ;
        }
      }
      break;

    case CHANGESETUPREQUEST:
      lcd.print("Conf Ändern?");
      lcd.setCursor(1, 1);
      lcd.print("JA");
      lcd.setCursor(7, 1);
      lcd.print("NEIN");
      break;
  }
  
  tick();
  delay(10);
}

void clearCache()
{
  selectedplayerid = -1;
  playeranswertimestamp = -1; 
  currentround = 1; 
  for(int i = 0; i < playerbuttonamount; i++)
  {
    playerbuttons[i][3] = -1;
    playerbuttons[i][4] = 0;
  }
}

boolean isButtonPressed(int buttonid)
{
  const int buttonpin = buttonpins[buttonid][0];
  return digitalRead(buttonpin) == HIGH;
}

boolean isModeratorButton(int buttonid)
{
  return buttonid == modgreenbuttonid || buttonid == modredbuttonid;
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
