#include <FastLED.h>

#define NUM_LEDS 8
#define LED_PIN 2
// LED model: WS2812B
// declare time delay for 0 and 1 code
// const int T_0_HIGH = 0.35;
// const int T_0_LOW = 0.8;
// const int T_1_HIGH = 35;
// const int T_1_LOW = 35;

CRGB leds[NUM_LEDS];

//range const
float RangeLower = 0;
float RangeUpper = 1.5;

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(200); // [0, 255]
  Serial.begin(9600);
  setColor(7, 100,100,100);
  FastLED.show();
  delay(1000);
}

void loop() {
  // for (int i=0; i<100; i++){
  //   for (int j=0; j<NUM_LEDS; j++){
  //     setColor(j,100-i, 0+i, 0);
  //   }
  //   FastLED.show();
  //   delay(10);
  // }
  // for (int j=0; j<NUM_LEDS; j++){
  //   setColor(j, 0,0,0);
  // }
  // FastLED.show();
  // delay(1000);
  // Serial.print("Force: ");
  // Serial.println(1.5);
  // forceLightUpdate(1.5);
  // delay(1000);
  // Serial.print("Force: ");
  // Serial.println(1.1);
  // forceLightUpdate(1.1);
  // delay(1000);
  // Serial.print("Force: ");
  // Serial.println(0.1);
  // forceLightUpdate(0.1);
  // delay(1000);
}

void forceLightUpdate(float force) {
  float scale = (force - RangeLower)/(RangeUpper - RangeLower)*NUM_LEDS;
  int num_leds_full = floor(scale);
  float last_led_vibrant = round((scale - num_leds_full)*255);
  if (scale == NUM_LEDS) { // if the force is at maximum
    for (int j=NUM_LEDS-1; j>-1; j--){ // start from the front of the hand
      setColor(j, 255, 0, 0); //red
    }
  }
  else { //if force is not max
    for (int j=num_leds_full-1; j>-1; j--){ //
      setColor(j, 0, 255, 0); //green
    }
    setColor(num_leds_full, 0, last_led_vibrant,0);
  }
  
  FastLED.show();
  delay(10);
}

// r,g,b in byte [0,255]
void setColor(int led_num, int r, int g, int b) {
  leds[led_num].r = r;
  leds[led_num].g = g;
  leds[led_num].b = b;
}
