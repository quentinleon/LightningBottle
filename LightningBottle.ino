#include <FastLED.h>

//PARAMETERS YOU CAN CHANGE
#define LED_PIN         9           //Refer to pinout image to get pin number for pin (should be 9 for you)
#define NUM_LEDS        50          //Set this to the number of LEDs on your strip

//positioning
#define OFFSET          0           //number of leds the effct is offset by on the strip (0 is centered, positive values moves it towards the end of the stip, negative towards the beginning
#define CENTER_SIZE     7           //size of the bright center of the effect in number of LEDs
#define EFFECT_SIZE     30          //number of LEDs lit at any given time

//brightness
#define BRIGHTNESS      70          //base brightness of the leds on the strip (0 off, 255 max)
#define MAX_BRIGHTNESS  110         //max pulse brightness of the LEDS (0 off, 255 max)
#define CENTER_RELTIVE_BRIGHTNESS 2 //the relative brightness of the center compared to the rest of the strip (2 = 2 times brighter)
//effect
#define MAX_PULSE_SPEED 1200        //the slowest rate (longest time interval) at which the brightness pulses (value is milliseconds).
#define MIN_PULSE_SPEED 420         //the fastest rate at which the brightness pulses. Will randomly choose a value between these two. (ms)
#define NOISE_AMP       20          //noise amplitude: the amount of noise to add to each LED (flickery effect) (0 to 255)

CRGB color = CRGB (42, 7, 255);  //Color of the strip in RGB (0 to 255 for each channel)

bool doubleSided = false;       //Set to true if the LEDs are double sided, false if not. (NOTE: double sided will work best with even number of LEDs)

//---------------------------------------------------------------------------------------------------------------

//internal vars, DO NOT CHANGE beyond this point
CRGB leds[NUM_LEDS];
int nled = doubleSided ? NUM_LEDS / 2 : NUM_LEDS;
int centerpoint = (nled / 2) + OFFSET;
int effectStart = centerpoint - (EFFECT_SIZE / 2);
int centerStart = centerpoint - (CENTER_SIZE / 2);

//runs once when the arduino gets power
void setup() {
  Serial.begin(9600);
  Serial.print("Initializing LEDs.\n");
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = CRGB ( 0, 0, 0);
  }
  FastLED.show();
  randomSeed(analogRead(0));
  Serial.print("Done.\n");
}

//runs forever as fast as possilbe
unsigned long timE = 0;
unsigned long changeTime = 0;
unsigned long targetTime = 0;
bool high = false;
double brightness = 0;
double b = 0;
void loop() {
  //compute pulse timings
  timE = millis();
  if (timE >= targetTime) {
    changeTime = timE;
    targetTime = changeTime + random(MIN_PULSE_SPEED, MAX_PULSE_SPEED);
    high = !high;
  }

  //compute brightness
  if (high) {
    brightness = (map(timE, changeTime, targetTime, BRIGHTNESS, MAX_BRIGHTNESS) / 255.0);
  } else {
    brightness = (map(timE, changeTime, targetTime, MAX_BRIGHTNESS, BRIGHTNESS) / 255.0);
  }

  //set LEDs
  for (int i = 0; i < nled; i++) {
    if (i >= centerStart && i < centerStart + CENTER_SIZE) {
      b = brightness + (random(-NOISE_AMP, NOISE_AMP) / 255.0);
      leds[i] = CRGB ((int)(color.r * b), (int)(color.g * b), (int)(color.b * b));
    }
    else if (i >= effectStart && i < effectStart + EFFECT_SIZE) {
      b = 1.0 - (float)(abs(i - centerpoint) / (float)(EFFECT_SIZE / 2.0));
      b *= brightness + (random(-NOISE_AMP, NOISE_AMP) / 255.0) / CENTER_RELTIVE_BRIGHTNESS;
      leds[i] = CRGB ((int)(color.r * b), (int)(color.g * b), (int)(color.b * b));
    }
    else {
      leds[i] = 0;
    }
  }

  //mirror if double sided
  if (doubleSided) {
     for (int i = 0; i < nled; i++) {
        leds[(NUM_LEDS - 1) - i] = leds[i];
     }
  }
  FastLED.show();
}
