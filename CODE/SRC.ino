//Code made By Aiden Nygaard

#include "Quizshow_Helper.h" // Importiert die integerte libary
#include "pezo_pitches.h" // Importiert die integerte libary

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Setzt denn LCD addresse zu 0x27 für ein 16 zeichen und 2 Reihen Display
LiquidCrystal_I2C lcd(0x27, 16, 2);  

//############################################################

const long SHORTDELAY = 100l;
const long NORMALDELAY = 250l;
const long LONGDELAY = 500l;
const long ETERNITYDELAY = 1000l;

#define BUTTONAMOUNT         6    // Setzt die Knopf anzahl zu 6
#define PLAYERBUTTONAMOUNT   4    // Setzt die Spieler Knopf anzahl zu 4

#define MODGREENBUTTONID     5    // Setzt denn Grünen Moderator Knopfid zu 5
#define MODREDBUTTONID       4    // Setzt denn Roten Moderator Knopfid zu 5

//############################################################

//Speichert alle an geschlossenen Knopf Pins
//INDEX: {ButtonPin, PressTimeStamp, Lastdcheckupdate}
long buttons[BUTTONAMOUNT][3] = 
{ 
  {12, 0, 0},  // Button ID: 0
  {11, 0, 0},  // Button ID: 1
  {10, 0, 0},  // Button ID: 2
  {9, 0, 0},   // Button ID: 3
  {8, 0, 0},   // Button ID: 4
  {7, 0, 0}    // Button ID: 5
};

//Spiechert alle Spieler und ist unendlich erweiterbar... naja fast unendlich :P
//INDEX: {buttonpin arrayid, RGBLedid, ColorId, isRegestered, Points}
int players[PLAYERBUTTONAMOUNT][5] = 
{
  {0, 0, 7, -1, 0}, // Player ID: 0
  {1, 1, 8, -1, 0}, // Player ID: 1
  {2, 2, 5, -1, 0}, // Player ID: 2
  {3, 3, 6, -1, 0}  // Player ID: 3
};

//############################################################

//Speichert alle verschidende Zustände die auftreten werden
enum states 
{
    _UNKNOWN_ = -1,
    BOOT = 0,
    SETUPROUNDS = 1, 
    SETUPTIME = 2,
    PLAYERREGISTER = 3,
    WAITBUZZ = 4,
    PLAYERBUZZED = 5,
    GAMEEND = 6,
    CHANGESETUPREQUEST = 7
};

states currentstate = _UNKNOWN_;

int maxrounds = 10;
long maxanswertime = 3000; //In Millis

int selectedplayerid = -1;
long playeranswertimestamp = -1; 
int currentround = 1;

//############################################################

void setup()
{
  Serial.begin(57600);
  
  //Inizialisiert die Intgrierte Bibliothek
  inithelperlib();

  lcd.init();
  lcd.backlight();
  changeState(BOOT);
  
  //Inizialisiert die Button Pins
  for (int i = 0; i < BUTTONAMOUNT; i++)
  {
    pinMode(buttons[i][0], INPUT_PULLUP);
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
  
  clearPixels();
  
  changeState(SETUPROUNDS);
}

void loop() 
{
  boolean islongpressedallreadychecked = false; // Beinhaltet denn Wert ob beide Mod knöpfe schon bei diesen check geprüft wurde
                                                // NOTE: Wenn es weggelassen wird könnte bugs auftretten, z.b das zwei Zustände gleichzeitig überpruft wird
  
  for (int buttonid = 0; buttonid < BUTTONAMOUNT; buttonid++)
  {
    if (digitalRead(buttons[buttonid][0]) == LOW)
    {
      const long lastupdated = buttons[buttonid][2] - buttons[buttonid][1];
      
      if(lastupdated > 0)
      {
        const long timepressed = millis() - buttons[buttonid][1];
         
        if(lastupdated <= 500 && timepressed >= 500)
        {
          //Der folgende Quellcode wird ein mal ausgeführt wenn ein Knopf 0.5 sekunden gedrückt wurde

          Serial.print("Buttonid: "); 
          Serial.println(buttonid); 
          Serial.print("isMod: "); 
          Serial.println(isModeratorButton(buttonid)); 
          Serial.print("isRedPressed: "); 
          Serial.println(isButtonPressedForACertinTime(MODREDBUTTONID, 500)); 
          Serial.print("isGreenPressed: "); 
          Serial.println(isButtonPressedForACertinTime(MODGREENBUTTONID, 500)); 

          Serial.print("RED TIME: "); 
          Serial.println(millis() - buttons[MODREDBUTTONID][1]); 
          Serial.print("GREEN TIME: "); 
          Serial.println(millis() - buttons[MODGREENBUTTONID][1]); 

          if(isModeratorButton(buttonid) && !islongpressedallreadychecked && isButtonPressedForACertinTime(MODGREENBUTTONID, 500) && isButtonPressedForACertinTime(MODREDBUTTONID, 500))
          {    
            islongpressedallreadychecked = true;
            
            switch(currentstate)
            { 
              case SETUPROUNDS:
              {
                Serial.println("SETUPROUNDS-LONGPRESS"); 
                changeState(SETUPTIME);
                buzz(SHORTDELAY, NOTE_F7, true);
                buzz(SHORTDELAY, NOTE_G7);
              }
              break;
  
              case SETUPTIME:
              {
                Serial.println("SETUPTIME-LONGPRESS"); 
                changeState(PLAYERREGISTER);
                buzz(SHORTDELAY, NOTE_F7, true);
                buzz(SHORTDELAY, NOTE_G7);
              }
              break;   
              
              case PLAYERREGISTER:
              {
                Serial.println("PLAYERREGISTER-LONGPRESS");   
                if(getPlayerRegesteredCount() > 0)
                {
                  changeState(WAITBUZZ);
                  
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
              break;
  
              case GAMEEND:
              {
                Serial.println("END-LONGPRESS");           
                changeState(CHANGESETUPREQUEST);
                buzz(SHORTDELAY, NOTE_F7, true);
                buzz(SHORTDELAY, NOTE_G7);
              }
              break;
            }
          }
        }
        //Der folgende Quellcode wird immer ausgeführt wenn ein Knopf gedrückt wird

        
      }
      else
      {
        //Der folgende Quellcode wird einmal ausgeführt wenn ein Knopf gedrückt wurde
        
        switch(currentstate)
        {
          case SETUPROUNDS:    
          {
            Serial.println("SETUPROUNDS-PRESS"); 
            if(isModeratorButton(buttonid))
            {
              if(MODGREENBUTTONID == buttonid)
              {
                maxrounds++;
                buzz(SHORTDELAY, NOTE_B6, true);
              }
              else if(MODREDBUTTONID == buttonid)
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
            
          case SETUPTIME:
          {
            Serial.println("SETUPTIME-PRESS"); 
            if(isModeratorButton(buttonid))
            {
              if(MODGREENBUTTONID == buttonid)
              {
                maxanswertime += 1000;
                buzz(SHORTDELAY, NOTE_B6, true);
              }
              else if(MODREDBUTTONID == buttonid)
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
              const int playerid = getPlayerIdFromButtonId(buttonid);
              if(!isPlayerIdRegistered(playerid))
              {
                updatePixel(players[playerid][1], players[playerid][2]);
                players[playerid][3] = 1;
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
              const int playerid = getPlayerIdFromButtonId(buttonid);
              if(isPlayerIdRegistered(playerid))
              {
                clearPixels();
                
                updatePixel(players[playerid][1], players[playerid][2]);
                
                selectedplayerid = playerid;
                
                playeranswertimestamp = millis() + maxanswertime;
                
                changeState(PLAYERBUZZED);

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
              clearPixels();
              if(MODGREENBUTTONID == buttonid)
              {
                players[selectedplayerid][4] = players[selectedplayerid][4] + 1;
                
                buzz(SHORTDELAY, NOTE_F7, true);
                buzz(SHORTDELAY, NOTE_G7);
              }
              else if(MODREDBUTTONID == buttonid)
              {
                buzz(SHORTDELAY, NOTE_F7, true);
                buzz(NORMALDELAY, NOTE_G4);
              }
              
              if(currentround >= maxrounds)
              {
                changeState(GAMEEND);

                buzz(NORMALDELAY, NOTE_G7, true);
                buzz(NORMALDELAY, NOTE_D7);
                buzz(NORMALDELAY, NOTE_F7);
              }
              else
              {
                currentround++;
                changeState(WAITBUZZ);
              }
            }
          }
          break;
             
          case GAMEEND:
          {
            Serial.println("END-PRESS");
            if(isPlayerButton(buttonid))
            {
              const int playerid = getPlayerIdFromButtonId(buttonid);
              if(selectedplayerid != playerid)
              {
                selectedplayerid = playerid;

                buzz(SHORTDELAY, NOTE_B6, true);
                updateLCDContent(); 
              }
            }
          }
          break;

          case CHANGESETUPREQUEST:
          {
            Serial.println("CHANGESETUPREQUEST-PRESS");
            if(isModeratorButton(buttonid))
            {
              clearCache();
              clearPixels();
              if(MODGREENBUTTONID == buttonid)
              {
                changeState(SETUPROUNDS);

                buzz(SHORTDELAY, NOTE_B6, true);
              }
              else if(MODREDBUTTONID == buttonid)
              {
                changeState(PLAYERREGISTER);

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
      buttons[buttonid][1] = millis();
    }
    buttons[buttonid][2] = millis();
  }

  //Wird Immer ausgefuhrt
  switch(currentstate)
  {
    case PLAYERBUZZED:
    {
      const long remainingtime = playeranswertimestamp - millis();
      if(remainingtime < 0)
      {
        buzz(SHORTDELAY, NOTE_F7, true);
        buzz(NORMALDELAY, NOTE_G4);
        
        if(currentround >= maxrounds)
        {
           changeState(GAMEEND);

           buzz(NORMALDELAY, NOTE_G7, true);
           buzz(NORMALDELAY, NOTE_D7);
           buzz(NORMALDELAY, NOTE_F7);
        }
        else
        {
          currentround++;
          changeState(WAITBUZZ);
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

void changeState(states newstateid)
{
  if(currentstate != newstateid)
  {
    currentstate = newstateid;
    updateLCDContent(); 
  }
}

void updateLCDContent()
{
  lcd.clear();
  
  switch(currentstate)
  {
    case BOOT:
    {
      lcd.print("BOOTING...");
      lcd.setCursor(0, 1);
      lcd.print("Bitte Warte!");  
    }
    break;
    
    case SETUPROUNDS:
    {
      lcd.print("SETUP-RUNDEN:");
      lcd.setCursor(0, 1);
      lcd.print(maxrounds);
    }
    break;
      
    case SETUPTIME:
    {
      lcd.print("SETUP-ZEIT:");
      lcd.setCursor(0, 1);
      lcd.print(maxanswertime / 1000);
      lcd.print(" - SEK");
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
      lcd.print(currentround);
      lcd.setCursor(0, 1);
      lcd.print("WAIT 4 BUZZ");
    }
    break;

    case PLAYERBUZZED:
    {
      lcd.print(getColorNameById(players[selectedplayerid][2]));
      lcd.setCursor(0, 1);
      lcd.print((playeranswertimestamp - millis()) / 1000);
      lcd.print(" - SEK");
    }
    break;

    case GAMEEND:
    {
      int currentmaxpoints = -1;
      int winnerlist[PLAYERBUTTONAMOUNT];

      //Ist der algorythmus der auswertet (wer & wie viele) gewonnen hat
      for(int i = 0; i < PLAYERBUTTONAMOUNT; i++)
      {
        if(isPlayerIdRegistered(i))
        {
          const int playerpoints = players[i][4];
          if(playerpoints > currentmaxpoints)
          {
            for(int winneri = 0; winneri < PLAYERBUTTONAMOUNT; winneri++)
            {
              winnerlist[winneri] = -1;
            }
            winnerlist[0] = i;
            currentmaxpoints = playerpoints;
          }
          else if(playerpoints == currentmaxpoints)
          {
            for(int winneri = 0; winneri < PLAYERBUTTONAMOUNT; winneri++)
            {
              if(winnerlist[winneri] == -1)
              {
                winnerlist[winneri] = i;
                break;
              }
            }
          }
        }
      }

      lcd.print("WIN:");

      for(int winneri = 0; winneri < PLAYERBUTTONAMOUNT; winneri++)
      {
        if(winnerlist[winneri] != -1)
        {
          lcd.print(getColorNameById(players[winnerlist[winneri]][2]).substring(0, 3));
          lcd.print(" ");
        }
        else
        {
          break;
        }
      }
  
      lcd.setCursor(0, 1);
      lcd.print(getColorNameById(players[selectedplayerid][2]));
      lcd.print(" ");
      lcd.print(players[selectedplayerid][4]);
      lcd.print("P");
    }
    break;
    
    case CHANGESETUPREQUEST:
    {
      lcd.print("Config Aendern?");
      lcd.setCursor(3, 1);
      lcd.print("JA");
      lcd.setCursor(9, 1);
      lcd.print("NEIN");
    }
    break;
  }
}

/*###################################################
 * @description Löscht die Cache
 *###################################################
*/
void clearCache()
{
  selectedplayerid = -1;
  playeranswertimestamp = -1; 
  currentround = 1; 
  for(int i = 0; i < PLAYERBUTTONAMOUNT; i++)
  {
    players[i][3] = -1;
    players[i][4] = 0;
  }
}

/*###################################################
 * @description Überprüft ob der Knopf mit der 
 *              "buttonid" für eine bestimmte zeit 
 *              gedrückt wurde
 * 
 * @param INTEGER buttonid
 * @param LONG timestamp
 * 
 * @return BOOLEAN isbuttonpressedforacertintime
 *###################################################
*/
boolean isButtonPressedForACertinTime(int buttonid, long timestamp)
{
  return millis() - buttons[buttonid][1] >= timestamp;
}

/*###################################################
 * @description Überprüft ob der "buttonid" einen 
 *              Moderator Knopf ist
 * 
 * @param INTEGER buttonid
 * 
 * @return BOOLEAN ismoderatorbutton
 *###################################################
*/
boolean isModeratorButton(int buttonid)
{
  return buttonid == MODGREENBUTTONID || buttonid == MODREDBUTTONID;
}

/*###################################################
 * @description Überprüft ob der "playerid" 
 *              regestriert wurde 
 *              (Aktiviert & Freigeschaltet)
 *              und wenn ja gebt er denn Wert "true"
 *              züruck
 * 
 * @param INTEGER playerid
 * 
 * @return BOOLEAN isplayeridregestered
 *###################################################
*/
boolean isPlayerIdRegistered(int playerid)
{
  return players[playerid][3] > 0;
}

/*###################################################
 * @description Überprüft ob der "buttonid" einen 
 *              Spieler Button ist und wenn ja gebt 
 *              er denn Wert "true" züruck
 * 
 * @param INTEGER buttonid
 * 
 * @return BOOLEAN isaplayerbutton
 *###################################################
*/
boolean isPlayerButton(int buttonid)
{
  return getPlayerIdFromButtonId(buttonid) >= 0;
}

/*###################################################
 * @description Sucht und zählt wie viele der 
 *              SpielerIds sich regestriert hat
 * 
 * @return INTEGER playerregesteredamount
 *###################################################
 */
int getPlayerRegesteredCount()
{
  int playercount = 0;
  for(int i = 0; i < PLAYERBUTTONAMOUNT; i++)
  {
    if(isPlayerIdRegistered(i))
    {
      playercount++;
    }
  }
  return playercount;
}

/*###################################################
 * @description Sucht & Überprüft ob der übergebene 
 *              wert "buttonid" einen "playerid" ist# 
 *              und wenn es exestiert gebt er die 
 *              "playerid" züruck
 * 
 * @param INTEGER buttonid
 * 
 * @return INTEGER playerid
 *###################################################
 */
int getPlayerIdFromButtonId(int buttonid)
{
  for(int i = 0; i < PLAYERBUTTONAMOUNT; i++)
  {
    if(players[i][0] == buttonid)
    {
      return i;
    }
  }
  return -1;
}
