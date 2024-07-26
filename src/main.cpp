#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 12
#define NUM_LEDS 298
// #define LEDS_PER_SECTION 50
#define NUM_SECTIONS 6

//first good
//second 2 less
// third 2 less
// fourth 11 less
// fifth 12 less

CRGB leds[NUM_LEDS];

double avgSensor = 0;

long lastHit = 0;

double shiftReg[5];

int sectionsLit = 0;

int numLeds[6] = {50, 46, 50, 41, 49, 62};
int startLeds[6] = {0, 50, 96, 146, 187, 236};

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(115200);
  avgSensor = analogRead(A0) / 1023.0;
  lastHit = millis();
}

void loop() {
  long cMillis = (long) millis();
  for(int i = 0; i < NUM_SECTIONS; i++) {
    // double lvl;
    // if(cMillis - lastHit > i * 1000) {
    //   lvl = max(0l, lastHit - cMillis + 1000 * (i + 1)) / 1000.0;
    // } else {
    //   lvl = 0;
    // }
    // uint8_t lvlInt = (uint8_t) (lvl * 100);
    // Serial.print(lvl);
    // Serial.print(" ");
    if((i + NUM_SECTIONS - 1) % NUM_SECTIONS < sectionsLit) {
      for(int j = 0; j < numLeds[i]; j++) {
        if(sectionsLit == 6 && cMillis - lastHit > 4 * (startLeds[(i + NUM_SECTIONS - 1) % NUM_SECTIONS] + j)) {
          CRGB rgb;
          if(cMillis - lastHit > 4 * NUM_LEDS) {
            int offset = (int) (((cMillis - lastHit) - (4 * NUM_LEDS)) * 0.1f);
            CHSV hsv((uint8_t) ((startLeds[i] + j) * 255 / NUM_LEDS + offset), 255, 255);
            hsv2rgb_rainbow(hsv, rgb);
          } else {
            CHSV hsv((uint8_t) ((startLeds[i] + j) * 255 / NUM_LEDS), 255, 255);
            hsv2rgb_rainbow(hsv, rgb);
          }
          leds[startLeds[i] + j] = CRGB(((int) rgb.r) * 192 / 255, ((int) rgb.g) * 192 / 255, ((int) rgb.b) * 192 / 255);
        } else {
          // int offset = cMillis / 10 % LEDS_PER_SECTION;
          // if((j - offset < 4 && j - offset > -4) || 
          //     (j - LEDS_PER_SECTION - offset < 4 && j - LEDS_PER_SECTION - offset > -4) ||
          //     (j + LEDS_PER_SECTION - offset < 4 && j + LEDS_PER_SECTION - offset > -4)) {
            leds[startLeds[i] + j] = CRGB(192, 0, 0);
          // } else {
          //   leds[i * LEDS_PER_SECTION + j] = CRGB(0, 0, 0);
          // }
        }
      }
    } else {
      for(int j = 0; j < numLeds[i]; j++) {
        leds[startLeds[i] + j] = CRGB(0, 0, 0);
      }
    }
  }
  FastLED.show();
  int rawSensor = analogRead(A0);
  double sensor = rawSensor / 1023.0;
  for(int i = 0; i < 4; i++) {
    shiftReg[i] = shiftReg[i + 1];
  }
  if(fabs(sensor - avgSensor) > 0.0125 && cMillis - lastHit > 200) {
    lastHit = cMillis;
    sectionsLit++;
    if(sectionsLit > NUM_SECTIONS) {
      sectionsLit = 0;
    }
  }
  avgSensor = avgSensor * 0.99 + sensor * 0.01;
  // delay(1);
}