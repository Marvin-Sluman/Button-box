#include <Arduino.h>
#include "FastLED.h"

#define NUM_LEDS 12
#define DATA_PIN 20

CRGB leds[NUM_LEDS];

bool blinkOn = false;
char buffer[6];
unsigned int rpm, maxRpm, ledsCount;
char currentByte;
float redlinePerc;
int pins[] = {0,1,2,3,4,5,6,7,8,9,10,11,12};
const int ledsAvail = 12;

/*
Amount of leds that needs to light up is calculated with the redline percentage.
Leds only begin to light after 0.8 rpm % after which is linearly lights up more leds.
80% -> 0 leds
90% -> half amount of leds
100% -> all leds

*/

int calcLeds() {
  redlinePerc = ((float)rpm / (float)maxRpm);
  //Serial.println(redlinePerc);
  if((redlinePerc - 0.85) < 0){ //If redline percentage is lower than 0.8, no led on
    ledsCount = 0;
  } else /*if(redlinePerc < 1.0)*/ { 
    ledsCount = (int)round(((redlinePerc - 0.85) / 0.15) * ledsAvail); //determine led count by ((redline % - 0.8) / 0.2 * led available  
  } /*else { //If rpm is above rpm limit. Leds need to blink to indicate shifting
    ledsCount = ledsAvail + 1;
  }*/
  return ledsCount;
}

void showLeds(int ledsShow) {
  for(int i = 0; i < ledsAvail; i++){
    if(i < ledsShow){
      if(i < 3){
        leds[i] = CRGB(0, 15, 0);
      }else if(i < 6){
        leds[i] = CRGB(15, 15, 0);
      }else if(i < 9){
        leds[i] = CRGB(15, 8, 0);
      }else{
        leds[i] = CRGB(15, 0, 0);
      }
    }else{
      leds[i] = CRGB(0,0,0);
    }
  }
  FastLED.show();
}

void blinkLeds(){
  if(blinkOn){
    for(int i = 0; i < ledsAvail; i++){
      leds[i] = CRGB(0, 0, 0);
    }
    blinkOn = false;
  }else{
    for(int i = 0; i < ledsAvail; i++){
      leds[i] = CRGB(50, 0, 0);
    }
    blinkOn = true;
  }
  FastLED.show();
}

void setup() {
  for (unsigned int i = 0; i<sizeof(pins); i++){
    pinMode(pins[i], INPUT_PULLUP);
  }
  pinMode(16, OUTPUT);
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() {
  for (unsigned int i = 0; i<sizeof(pins); i++){
    Joystick.button(i+1, !(digitalRead(pins[i])));
  } 

  for (unsigned int i = sizeof(pins); i<32; i++){
    Joystick.button(i, 0);
  }
  Joystick.button(16, 0);

  while (Serial.available() > 0) {
    static byte index = 0;
    currentByte = Serial.read();

    if(currentByte == 'B'){ //Start of data transmission
      index = 0;
    }else if(currentByte == 'R'){ //Current RPM has been sent 
      buffer[index] = '\0';
      rpm = atoi(buffer);
      calcLeds();
      showLeds(ledsCount);
      //Serial.println(rpm);
    }else if(currentByte == 'M'){ //Max RPM has been sent
      buffer[index] = '\0';
      maxRpm = atoi(buffer);
      Serial.print("Max RPM is:");
      Serial.println(maxRpm);
    }else {
      buffer[index] = currentByte;
      index++;
    }
  }
  //Serial.println(ledsCount);
}