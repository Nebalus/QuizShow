//Made By Aiden Nygaard
#include "Quizshow_Helper.h"

//##############################################################################
//
//  CONSTANTEN / VARIABLEN
//        
//##############################################################################    

//#####################
//       CONFIG
//#####################

#define CONF_BUZZPIN              4    // Buzzer Pin 4
#define CONF_BUZZDEFAULTFREQ      2000 // Sendet einen 2KHz Signal
#define CONF_BUZZDEFAULTDURATION  100  // Sendet einen Signal für 100ms
#define CONF_BUZZCACHEMAX         20   // Wie viele Buzz anweisungen können gleichzeitig gespeichert werden 

#define CONF_RGBPIN               2    // RGB Datenleitung Pin
#define CONF_NUMPIXELS            4    // Anzahl der WS2812 LEDs oder der NEOPIXEL
  
//#####################
//  Buzzer Variablen
//#####################

long buzzduration = 0l;

//#####################
//    RGB Variablen
//#####################

//Ist ein Zwischen Spiecher womit man die ColorIDs für denn Jeweilige RGB-PIXEL Speichern kann
int ledcache[CONF_NUMPIXELS] = {};

//Hier werden alle möglichen verwendbaren Farbschemas in einem Array gespeichert
//INDEX = {GREEN, RED, BLUE}
const int colorpallet[9][3] = 
{
  {0, 0, 0},    //CLEAR   || ID = 0
  {25, 0, 0},   //GREEN   || ID = 1
  {0, 25, 0},   //RED     || ID = 2
  {35, 35, 35}, //WHITE   || ID = 3
  {25, 25, 0},  //YELLOW  || ID = 4
  {0, 0, 25},   //BLUE    || ID = 5
  {0, 13, 25},  //PURPULE || ID = 6
  {13, 25, 0},  //ORANGE  || ID = 7
  {13, 0, 13}   //CYAN    || ID = 8
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
  if (buzzduration > 0 && millis() <= buzzduration)
  {
    return true;
  }
  else
  {
    return false;
  }
}

//##############################################################################
//
//  Methoden für die RGB LEDs
//        
//##############################################################################

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
  Serial.print("UPDATE PIXELCOLOR ID:");
  Serial.print(pixelid);
  Serial.print(" with ColorID:");
  Serial.println(colorid);
  if(updatecache)
  {
    Serial.print("UPDATE PIXELCACHE ID:");
    Serial.print(pixelid);
    Serial.print(" with ColorID:");
    Serial.println(colorid);
    ledcache[pixelid] = colorid;
  }
  pixels.setPixelColor(pixelid, pixels.Color(colorpallet[colorid][0], colorpallet[colorid][1], colorpallet[colorid][2]));
  pixels.show();
}

//Eine Funktion um vereinfacht die Farben von den RGB-LEDs ändern zu können
void updatePixel(int pixelid, int colorid) 
{
  updatePixel(pixelid, colorid, true);
}

//Löscht alle Pixels und schaltet sie aus
void clearpixels()
{
  clearpixels(true);
}

//Löscht alle Pixels und schaltet sie aus
void clearpixels(boolean updatecache)
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
