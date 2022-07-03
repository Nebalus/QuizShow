//Code made By Aiden Nygaard

#include "Quizshow_Helper.h"

//##############################################################################
//
//  CONSTANTEN / VARIABLEN
//        
//##############################################################################    
  
//#####################
//  Buzzer Variablen
//#####################

long buzzcache[CONF_BUZZCACHEMAX][2] = {};

//#####################
//    RGB Variablen
//#####################

//Ist ein Zwischen Spiecher womit man die ColorIDs für denn Jeweilige RGB-PIXEL Speichern kann
long ledcache[CONF_NUMPIXELS] = {};

const int COLORAMOUNT = 10;
//Hier werden alle möglichen verwendbaren Farbschemas in einem Array gespeichert
//INDEX = {RED, GREEN, BLUE}
const int colorpallet[COLORAMOUNT][3] = 
{
  {0, 0, 0},    //CLEAR   || ID = 0
  {0, 25, 0},   //GREEN   || ID = 1
  {25, 0, 0},   //RED     || ID = 2
  {35, 35, 35}, //WHITE   || ID = 3
  {25, 25, 0},  //YELLOW  || ID = 4
  {0, 0, 25},   //BLUE    || ID = 5
  {13, 0, 25},  //PURPULE || ID = 6
  {25, 13, 0},  //ORANGE  || ID = 7
  {0, 13, 13},  //CYAN    || ID = 8
  {22, 11, 15}  //PINK    || ID = 9
};

const String colorname[COLORAMOUNT] = 
{
  "CLEAR",   //ID = 0
  "GREEN",   //ID = 1
  "RED",     //ID = 2
  "WHITE",   //ID = 3
  "YELLOW",  //ID = 4
  "BLUE",    //ID = 5
  "PURPULE", //ID = 6
  "ORANGE",  //ID = 7
  "CYAN",    //ID = 8
  "PINK"     //ID = 9
};

//##############################################################################
//
//  System Relevante Methoden
//        
//##############################################################################

boolean isinit = false;

Adafruit_NeoPixel pixels(CONF_NUMPIXELS, CONF_RGBPIN, NEO_GRB + NEO_KHZ800);

//Immer in der setup() methode inizialisieren
void inithelperlib()
{
  if(!isinit)
  {
    isinit = true;
    pixels.begin();
    
    pinMode(CONF_RGBPIN, OUTPUT);   // Inizialisiert den RGB Datenleitung Pin
    pinMode(CONF_BUZZPIN, OUTPUT);  // Inizialisiert den Buzzer Pin

    clearBuzzCache();   //Wird benötigt um denn Buzzcache vollständig zu initialisieren
  }
}

//Immer als letztes in der loop() methode ausführen lassen
void tick()
{
  updateBuzz();
  delay(CONF_TICKDELAY);
}

//##############################################################################
//
//  Methoden für denn Buzzer
//        
//##############################################################################

void clearBuzzCache()
{
  for(int i = 0; i < CONF_BUZZCACHEMAX; i++)
  {
    buzzcache[i][0] = -1;
    buzzcache[i][1] = -1;
  }
}

void buzz(long duration)
{
  buzz(duration, CONF_BUZZDEFAULTFREQ, false);
}

void buzz(long duration, boolean clearcache)
{
  buzz(duration, CONF_BUZZDEFAULTFREQ, clearcache);
}

void buzz(long duration, long frequenz)
{
  buzz(duration, frequenz, false);
}

void buzz(long duration, long frequenz, boolean clearcache)
{
  if(clearcache)
  {
    clearBuzzCache();
  }

  for(int i = 0; i < CONF_BUZZCACHEMAX; i++)
  {
    if(buzzcache[i][0] <= -1 && buzzcache[i][1] <= -1)
    {
      if(i == 0)
      {
        buzzcache[i][0] = millis() + duration;
        buzzcache[i][1] = frequenz;
      }
      else
      {
        buzzcache[i][0] = buzzcache[i - 1][0] + duration;
        buzzcache[i][1] = frequenz; 
      }
      
      Serial.print("BUZZ SET Duration: ");
      Serial.print(duration);
      Serial.print("ms");
      Serial.print(" Frequenz: ");
      Serial.print(frequenz);
      Serial.print(" ClearCache: ");
      Serial.println(clearcache);
      break;
    }
  }
}

void updateBuzz()
{
  const long buzztime = buzzcache[0][0];
  if (buzztime > -1)
  {
    if(buzztime >= millis())
    {
      const long buzzfreq = buzzcache[0][1];
      tone(CONF_BUZZPIN, buzzfreq);
    }
    else
    {
      for(int i = 0; i < CONF_BUZZCACHEMAX; i++)
      {
        if(buzzcache[i][0] > -1 && buzzcache[i][1] > -1)
        {
          buzzcache[i][0] = buzzcache[i + 1][0];
          buzzcache[i][1] = buzzcache[i + 1][1];
        }
        else
        {
          break;
        }
      }
    }
  }
  else
  {
    noTone(CONF_BUZZPIN); 
  }
}

//##############################################################################
//
//  Methoden für die RGB LEDs
//        
//##############################################################################

String getColorNameById(int colorid)
{
  if(colorname[colorid] != "")
  {
    return colorname[colorid];
  }
  return "UNKNOWNCOLOR";
}

void loadPixelsFromCache()
{
  for(int i = 0; i < CONF_NUMPIXELS; i++)
  {
    updatePixel(i, ledcache[i], false);
  }
}

void updateAllPixels(int colorid)
{
  updateAllPixels(colorid, true);
}

void updateAllPixels(int colorid, boolean updatecache)
{
  for(int i = 0; i < CONF_NUMPIXELS; i++)
  {
    updatePixel(i, colorid, updatecache);
  }
}

//Eine Funktion um vereinfacht die Farben von den RGB-LEDs ändern zu können
void updatePixel(int pixelid, int colorid, boolean updatecache) 
{
  Serial.print("UPDATE NEOPIXEL ID:");
  Serial.print(pixelid);
  Serial.print(" with ColorID:");
  Serial.print(colorid);
  Serial.print(" (");
  Serial.print(getColorNameById(colorid));
  Serial.println(")");
  if(updatecache)
  {
    Serial.print("UPDATE PIXELCACHE ID:");
    Serial.print(pixelid);
    Serial.print(" with ColorID:");
    Serial.print(colorid);
    Serial.print(" (");
    Serial.print(getColorNameById(colorid));
    Serial.println(")");
    ledcache[pixelid] = colorid;
  }
  pixels.setPixelColor(pixelid, pixels.Color(colorpallet[colorid][1], colorpallet[colorid][0], colorpallet[colorid][2]));
  pixels.show();
}

//Eine Funktion um vereinfacht die Farben von den RGB-LEDs ändern zu können
void updatePixel(int pixelid, int colorid) 
{
  updatePixel(pixelid, colorid, true);
}

//Löscht alle Pixels und schaltet sie aus
void clearPixels()
{
  clearPixels(true);
}

//Löscht alle Pixels und schaltet sie aus
void clearPixels(boolean updatecache)
{
  Serial.println("CLEAR PIXELS");
  if(updatecache)
  {
    for(int i = 0; i < CONF_NUMPIXELS; i++)
    {
      ledcache[i] = 0;
    }
  }
  pixels.clear();
  pixels.show();
}
