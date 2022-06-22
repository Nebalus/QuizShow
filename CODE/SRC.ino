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
  {2, 2, 4, -1, 0},
  {3, 3, 6, -1, 0}
};

const int modgreenbuttonid = 4;
const int modredbuttonid = 5;

//############################################################

enum scenarios {
    _UNKNOWN_ = -1,
    BOOT = 0,
    SETUP_ROUNDS = 1, 
    SETUP_TIME = 2,
    PLAYERREGISTER = 3,
    WAITBUZZ = 4,
    PLAYERBUZZED = 5,
    END = 6,
    CHANGESETUPREQUEST = 7
};

int currentscenario = _UNKNOWN_;

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
  changeScenario(BOOT);
  
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
  changeScenario(SETUP_ROUNDS);
}

void loop() 
{
  for (int buttonid = 0; buttonid < buttonamount; buttonid++)
  {
    if (isButtonPressed(buttonid))
    {
      const long lastupdated = buttonpins[buttonid][2] - buttonpins[buttonid][1];
      
      if(lastupdated > 0)
      {
        const long timepressed = millis() - buttonpins[buttonid][1];
       
        if(lastupdated <= 500 && timepressed >= 500)
        {
          //Wird ein mal ausgeführt wenn der knopf 0.5 sekunden gedrückt wurde

          Serial.print("Buttonid"); 
          Serial.println(buttonid); 
          Serial.print("isMod"); 
          Serial.println(isModeratorButton(buttonid)); 
          Serial.print("isRedPressed"); 
          Serial.println(isButtonPressed(modredbuttonid)); 
          Serial.print("isGreenPressed"); 
          Serial.println(isButtonPressed(modgreenbuttonid)); 

          switch(currentscenario)
          { 
            case SETUP_ROUNDS:
            {
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                Serial.println("SETUP_ROUNDS-LONGPRESS"); 
                changeScenario(SETUP_TIME);
                buzz(250);
              }
            }
            break;

            case SETUP_TIME:
            {
              Serial.println("SETUP_TIME-LONGPRESS"); 
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                changeScenario(PLAYERREGISTER);
                buzz(250);
              }
            }
            break;   
                      
            case PLAYERREGISTER:
            {
              Serial.println("PLAYERREGISTER-LONGPRESS"); 
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                changeScenario(WAITBUZZ);
                buzz(250);
              }
            }
            break;

            case END:
            {
              Serial.println("END-LONGPRESS"); 
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                changeScenario(CHANGESETUPREQUEST);
                buzz(250);
              }
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
          {
            Serial.println("SETUP_ROUNDS-PRESS"); 
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
              updateLCDContent();      
            }
          }
          break;
            
          case SETUP_TIME:
          {
            Serial.println("SETUP_TIME-PRESS"); 
            if(isModeratorButton(buttonid))
            {
              if(modgreenbuttonid == buttonid)
              {
                maxanswertime += 1000;
                buzz(100);
              }
              else if(modredbuttonid == buttonid)
              {
                if(maxanswertime - 1000 >= 1000)
                {
                  maxanswertime -= 1000;
                  buzz(100);
                }
              }
              updateLCDContent();      
            }      
          }
          break;

          case PLAYERREGISTER:
          {
            Serial.println("PLAYERREGISTER-PRESS"); 
            if(isPlayerButton(buttonid))
            {
              const int playerbuttonid = getPlayerButtonId(buttonid);
              if(!isPlayerButtonRegistered(playerbuttonid))
              {
                updatePixel(playerbuttons[playerbuttonid][1], playerbuttons[playerbuttonid][2], true);
                playerbuttons[playerbuttonid][3] = 1;
                buzz(250);
                updateLCDContent();
              }
            }
          }
          break;
            
          case WAITBUZZ:
          {
            Serial.println("WAITBUZZ-PRESS"); 
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
                changeScenario(PLAYERBUZZED);
              }
            }
          }
          break;
            
          case PLAYERBUZZED:
          {
            Serial.println("PLAYERBUZZED-PRESS"); 
            if(isModeratorButton(buttonid))
            {
              if(modgreenbuttonid == buttonid)
              {
                playerbuttons[selectedplayerid][4] = playerbuttons[selectedplayerid][4] + 1;
              }
              
              if(currentround >= maxrounds)
              {
                 changeScenario(END);
              }
              else
              {
                currentround++;
                changeScenario(WAITBUZZ);
              }
            }
          }
          break;
             
          case END:
          {
            Serial.println("END-PRESS");
          }
          break;

          case CHANGESETUPREQUEST:
          {
            Serial.println("CHANGESETUPREQUEST-PRESS");
            clearCache();
            if(isModeratorButton(buttonid))
            {
              if(modgreenbuttonid == buttonid)
              {
                changeScenario(SETUP_ROUNDS);
              }
              else if(modredbuttonid == buttonid)
              {
                changeScenario(PLAYERREGISTER);
              }      
            }
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

  //Wird Immer ausgefuhrt
  switch(currentscenario)
  {
    case PLAYERBUZZED:
    {
      const long remainingtime = playeranswertimestamp - millis();
      if(remainingtime < 0)
      {
        if(currentround >= maxrounds)
        {
           changeScenario(END);
        }
        else
        {
          currentround++;
          changeScenario(WAITBUZZ);
        }
      }
      else
      {
        updateLCDContent();
      }
    }
    break;
  }
  
  tick();
}

void changeScenario(int newscenarioid)
{
  currentscenario = newscenarioid;
  updateLCDContent();
}


void updateLCDContent()
{
  lcd.clear();
  
  switch(currentscenario)
  {
    case BOOT:
    {
      lcd.print("BOOTING...");
      lcd.setCursor(0, 1);
      lcd.print("Bitte Warte!");  
    }
    break;
    
    case SETUP_ROUNDS:
    {
      lcd.print("SETUP-RUNDEN:");
      lcd.setCursor(0, 1);
      lcd.print(maxrounds);
    }
    break;
      
    case SETUP_TIME:
    {
      lcd.print("SETUP-ZEIT:");
      lcd.setCursor(0, 1);
      lcd.print(maxanswertime / 1000);
      lcd.setCursor(2, 1);
      lcd.print("- SEK");
    }
    break;

    case PLAYERREGISTER:
    {
      lcd.print("REGESTRIERE:");
      lcd.setCursor(0, 1);
      byte playercount = 0;
      for(int i = 0; i < playerbuttonamount; i++)
      {
        if(isPlayerButtonRegistered(i))
        {
          playercount++;
        }
      }
      lcd.print(playercount);
    }
    break;
    
    case WAITBUZZ:
    {
      Serial.println("WAITBUZZ");
      lcd.print("Runde:");
      lcd.setCursor(6, 0);
      lcd.print(currentround);
      lcd.setCursor(0, 1);
      lcd.print("WAIT 4 BUZZ");
    }
    break;

    case PLAYERBUZZED:
    {
      lcd.print(getColorNameById(playerbuttons[selectedplayerid][2]));
      lcd.setCursor(0, 1);
      lcd.print((playeranswertimestamp - millis()) / 1000);
      lcd.setCursor(2, 1);
      lcd.print("- SEK");
    }
    break;

    case CHANGESETUPREQUEST:
    {
      lcd.print("Conf Aendern?");
      lcd.setCursor(3, 1);
      lcd.print("JA");
      lcd.setCursor(9, 1);
      lcd.print("NEIN");
    }
    break;
  }
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
