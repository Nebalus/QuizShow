//Made By Aiden Nygaard
#include "Quizshow_Helper.h" // Importiert die integerte libary
#include "pezo_pitches.h" // Importiert die integerte libary

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Setzt denn LCD addresse zu 0x27 für ein 16 zeichen und 2 Reihen Display
LiquidCrystal_I2C lcd(0x27, 16, 2);  

//############################################################

#define BUTTONAMOUNT   6 // Setzt die Knopf anzahl zu 6

//INDEX: {ButtonPin, Pressduration, Lastdcheckupdate}
long buttonpins[BUTTONAMOUNT][3] = { 
  {12, 0, 0},  //  ID: 0
  {11, 0, 0},  //  ID: 1
  {10, 0, 0},  //  ID: 2
  {9, 0, 0},   //  ID: 3
  {8, 0, 0},   //  ID: 4
  {7, 0, 0}    //  ID: 5
};

#define PLAYERBUTTONAMOUNT   4

//INDEX: {buttonpin arrayid, RGBLedid, ColorId, isRequested, Points}
int playerbuttons[PLAYERBUTTONAMOUNT][5] = 
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

scenarios currentscenario = PLAYERREGISTER;

int maxrounds = 10;
long maxanswertime = 3000; //In Millis

int selectedplayerid = -1;
long playeranswertimestamp = -1; 
int currentround = 1;

//############################################################

const long SHORTDELAY = 100l;
const long NORMALDELAY = 250l;
const long LONGDELAY = 500l;
const long ETERNITYDELAY = 1000l;

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
  for (int i = 0; i < BUTTONAMOUNT; i++)
  {
    pinMode(buttonpins[i][0], INPUT);
  }

  //Testet ob alle RGB LEDS leuchten
  for(int i = 0; i < PLAYERBUTTONAMOUNT; i++)
  {
    updatePixel(i, 8);
    delay(LONGDELAY);
  }

  buzz(SHORTDELAY, NOTE_F7, true);
  buzz(SHORTDELAY, NOTE_D7);
  buzz(SHORTDELAY, NOTE_G7);
  clearPixels(true);
  changeScenario(SETUP_ROUNDS);
}

void loop() 
{
  for (int buttonid = 0; buttonid < BUTTONAMOUNT; buttonid++)
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

          Serial.print("Buttonid: "); 
          Serial.println(buttonid); 
          Serial.print("isMod: "); 
          Serial.println(isModeratorButton(buttonid)); 
          Serial.print("isRedPressed: "); 
          Serial.println(isButtonPressed(modredbuttonid)); 
          Serial.print("isGreenPressed: "); 
          Serial.println(isButtonPressed(modgreenbuttonid)); 

          switch(currentscenario)
          { 
            case SETUP_ROUNDS:
            {
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                Serial.println("SETUP_ROUNDS-LONGPRESS"); 
                changeScenario(SETUP_TIME);
                buzz(SHORTDELAY, NOTE_F7, true);
                buzz(SHORTDELAY, NOTE_G7);
              }
            }
            break;

            case SETUP_TIME:
            {
              Serial.println("SETUP_TIME-LONGPRESS"); 
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                changeScenario(PLAYERREGISTER);
                buzz(SHORTDELAY, NOTE_F7, true);
                buzz(SHORTDELAY, NOTE_G7);
              }
            }
            break;   
            
            case PLAYERREGISTER:
            {
              Serial.println("PLAYERREGISTER-LONGPRESS"); 
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                if(getPlayerRegesteredCount() > 0)
                {
                  changeScenario(WAITBUZZ);
                  
                  buzz(SHORTDELAY, NOTE_F7, true);
                  buzz(SHORTDELAY, NOTE_G7); 
                }
                else
                {
                  buzz(SHORTDELAY, NOTE_E3, true);
                  buzz(SHORTDELAY, NOTE_NONE);
                  buzz(SHORTDELAY, NOTE_E3);
                }
              }
            }
            break;

            case END:
            {
              Serial.println("END-LONGPRESS"); 
              if(isModeratorButton(buttonid) && isButtonPressed(modgreenbuttonid) && isButtonPressed(modredbuttonid))
              {
                changeScenario(CHANGESETUPREQUEST);
                buzz(SHORTDELAY, NOTE_F7, true);
                buzz(SHORTDELAY, NOTE_G7);
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
                buzz(SHORTDELAY, NOTE_B6, true);
              }
              else if(modredbuttonid == buttonid)
              {
                if(maxrounds > 1)
                {
                  maxrounds--;
                  buzz(SHORTDELAY, NOTE_A6, true);
                }
                else
                {
                  buzz(SHORTDELAY, NOTE_E3, true);
                  buzz(SHORTDELAY, NOTE_NONE);
                  buzz(SHORTDELAY, NOTE_E3);
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
                buzz(SHORTDELAY, NOTE_B6, true);
              }
              else if(modredbuttonid == buttonid)
              {
                if(maxanswertime - 1000 >= 1000)
                {
                  maxanswertime -= 1000;
                  buzz(SHORTDELAY, NOTE_A6, true);
                }
                else
                {
                  buzz(SHORTDELAY, NOTE_E3, true);
                  buzz(SHORTDELAY, NOTE_NONE);
                  buzz(SHORTDELAY, NOTE_E3);
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
                updateLCDContent();

                buzz(SHORTDELAY, NOTE_F7, true);
                buzz(SHORTDELAY, NOTE_G7);
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
                selectedplayerid = playerid;
                playeranswertimestamp = millis() + maxanswertime;
                changeScenario(PLAYERBUZZED);

                buzz(ETERNITYDELAY, NOTE_G7, true);
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

                buzz(NORMALDELAY, NOTE_G7, true);
                buzz(NORMALDELAY, NOTE_D7);
                buzz(NORMALDELAY, NOTE_F7);
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

                buzz(SHORTDELAY, NOTE_B6, true);
              }
              else if(modredbuttonid == buttonid)
              {
                changeScenario(PLAYERREGISTER);

                buzz(SHORTDELAY, NOTE_B6, true);
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

           buzz(NORMALDELAY, NOTE_G7, true);
           buzz(NORMALDELAY, NOTE_D7);
           buzz(NORMALDELAY, NOTE_F7);
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

void changeScenario(scenarios newscenarioid)
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
      lcd.print(getPlayerRegesteredCount());
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
  for(int i = 0; i < PLAYERBUTTONAMOUNT; i++)
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

byte getPlayerRegesteredCount()
{
  byte playercount = 0;
  for(int i = 0; i < PLAYERBUTTONAMOUNT; i++)
  {
    if(isPlayerButtonRegistered(i))
    {
      playercount++;
    }
  }
  return playercount;
}

int getPlayerButtonId(int buttonid)
{
  for(int i = 0; i < PLAYERBUTTONAMOUNT; i++)
  {
    if(playerbuttons[i][0] == buttonid)
    {
      return i;
    }
  }
  return -1;
}
