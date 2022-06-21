//Made By Aiden Nygaard
#ifndef QUIZSHOW_HELPER_H
#define QUIZSHOW_HELPER_H

//#####################
//       CONFIG
//#####################

#define CONF_BUZZPIN              4    // Buzzer Pin 4
#define CONF_BUZZDEFAULTFREQ      2000 // Sendet einen 2KHz Signal
#define CONF_BUZZDEFAULTDURATION  100  // Sendet einen Signal für 100ms
#define CONF_BUZZCACHEMAX         20   // Wie viele Buzz anweisungen können gleichzeitig gespeichert werden 

#define CONF_RGBPIN               2    // RGB Datenleitung Pin
#define CONF_NUMPIXELS            4    // Anzahl der WS2812 LEDs oder der NEOPIXEL

//##############################################################################################################################

#include <Arduino.h>
#include <Adafruit_NeoPixel.h> //Inhaltsbibliothek für das ansteuern der Neopixels 
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

void inithelperlib();
void tick();

void buzz(long duration);
void updateBuzz();
boolean isBuzzing();

String getColorNameById(int colorid);
void loadPixelsFromCache();
void updateAllPixels(int colorid);
void updateAllPixels(int colorid, boolean updatecache);
void clearPixels();
void clearPixels(boolean updatecache);
void updatePixel(int pixelid, int colorid, boolean updatecache);
void updatePixel(int pixelid, int colorid);

#endif
