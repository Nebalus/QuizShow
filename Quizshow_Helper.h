//Made By Aiden Nygaard
#ifndef QUIZSHOW_HELPER_H
#define QUIZSHOW_HELPER_H

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

void clearpixels();
void clearpixels(boolean updatecache);
void updatePixel(int pixelid, int colorid, boolean updatecache) ;
void updatePixel(int pixelid, int colorid);
void loadPixelsFromCache();

#endif
