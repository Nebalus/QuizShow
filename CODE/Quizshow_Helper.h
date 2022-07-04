//Code made By Aiden Nygaard

#ifndef QUIZSHOW_HELPER_H
#define QUIZSHOW_HELPER_H

//#####################
//       CONFIG
//#####################

#define CONF_BUZZPIN              4    // Buzzer Pin 4
#define CONF_BUZZDEFAULTFREQ      2000 // Sendet einen 2KHz Signal
#define CONF_BUZZCACHEMAX         20   // Wie viele Buzz anweisungen können gleichzeitig gespeichert werden 

#define CONF_RGBPIN               2    // RGB Datenleitung Pin
#define CONF_NUMPIXELS            4    // Anzahl der WS2812 LEDs oder der NEOPIXEL

#define CONF_TICKDELAY            30   // Legt für die loop() methode die verzögerung in ms fest womit es ein nächstes mal ausgeführt werden soll

//##############################################################################################################################

#include <Arduino.h>
#include <Adafruit_NeoPixel.h> //Inhaltsbibliothek für das ansteuern der Neopixels 
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

void inithelperlib();
void tick();

void clearBuzzCache();
void buzz(long duration);
void buzz(long duration, boolean clearcache);
void buzz(long duration, long frequenz);
void buzz(long duration, long frequenz, boolean clearcache);
void updateBuzz();

String getColorNameById(int colorid);
void updateAllPixels(int colorid);
void clearPixels();
void updatePixel(int pixelid, int colorid);

#endif
