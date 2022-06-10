//Made By Aiden Nygaard
#include "Quizshow_Helper.h"

//##############################################################################
//
//  CONSTANTEN / VARIABLEN
//        
//##############################################################################    
  
//#####################
//  Buzzer Variablen
//#####################

long buzzduration = 0l;

//#####################
//    RGB Variablen
//#####################

//Ist ein Zwischen Spiecher womit man die ColorIDs für denn Jeweilige RGB-PIXEL Speichern kann
int ledcache[CONF_NUMPIXELS] = {};

const int coloramount = 9;
//Hier werden alle möglichen verwendbaren Farbschemas in einem Array gespeichert
//INDEX = {RED, GREEN, BLUE}
const int colorpallet[coloramount][3] = 
{
  {0, 0, 0},    //CLEAR   || ID = 0
  {0, 25, 0},   //GREEN   || ID = 1
  {25, 0, 0},   //RED     || ID = 2
  {35, 35, 35}, //WHITE   || ID = 3
  {25, 25, 0},  //YELLOW  || ID = 4
  {0, 0, 25},   //BLUE    || ID = 5
  {13, 0, 25},  //PURPULE || ID = 6
  {25, 13, 0},  //ORANGE  || ID = 7
  {0, 13, 13}   //CYAN    || ID = 8
};

const String colorname[coloramount] = 
{
  "CLEAR",   //ID = 0
  "GREEN",   //ID = 1
  "RED",     //ID = 2
  "WHITE",   //ID = 3
  "YELLOW",  //ID = 4
  "BLUE",    //ID = 5
  "PURPULE", //ID = 6
  "ORANGE",  //ID = 7
  "CYAN"     //ID = 8
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
  }
}

//Immer als letztes in der loop() methode ausführen lassen
void tick()
{
  updateBuzz();
}

//##############################################################################
//
//  Methoden für denn Buzzer
//        
//##############################################################################

void buzz(long duration)
{
  Serial.print("BUZZ set for ");
  Serial.print(duration);
  Serial.println("ms");

  buzzduration = millis() + duration;
}

void updateBuzz()
{
  if (isBuzzing())
  {
    tone(CONF_BUZZPIN, CONF_BUZZDEFAULTFREQ);
  }
  else
  {
    noTone(CONF_BUZZPIN);
  }
}

boolean isBuzzing()
{ 
  return buzzduration > 0 && millis() <= buzzduration;
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
