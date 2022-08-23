#include <Adafruit_Circuit_Playground.h>
#include <FastLED.h>

//FASTLED_USING_NAMESPACE

// Identifying data pins

#define LED_TYPE    NEOPIXEL
#define COLOR_ORDER GRB
//#define NUM_STRIPS  5
#define NUM_LEDS_PER_STRIP    70
#define NUM_LEDS    NUM_LEDS_PER_STRIP
//CRGB ledsA1[NUM_LEDS_PER_STRIP];
//CRGB ledsA2[NUM_LEDS_PER_STRIP];
//CRGB ledsA3[NUM_LEDS_PER_STRIP];
//CRGB ledsA4[NUM_LEDS_PER_STRIP];
//CRGB ledsA5[NUM_LEDS_PER_STRIP];

CRGB leds[NUM_LEDS_PER_STRIP];

#define BRIGHTNESS          120
#define FRAMES_PER_SECOND  60

void setup() {
  //FastLED.addLeds<NEOPIXEL, A1>(ledsA1, NUM_LEDS_PER_STRIP);
  //FastLED.addLeds<NEOPIXEL, A2>(ledsA2, NUM_LEDS_PER_STRIP);
  //FastLED.addLeds<NEOPIXEL, A3>(ledsA3, NUM_LEDS_PER_STRIP);
  //FastLED.addLeds<NEOPIXEL, A4>(ledsA4, NUM_LEDS_PER_STRIP);
  //FastLED.addLeds<NEOPIXEL, A5>(ledsA5, NUM_LEDS_PER_STRIP);

  FastLED.addLeds<NEOPIXEL, A1>(leds, NUM_LEDS_PER_STRIP).setDither(BRIGHTNESS < 255);
  FastLED.addLeds<NEOPIXEL, A2>(leds, NUM_LEDS_PER_STRIP).setDither(BRIGHTNESS < 255);
  FastLED.addLeds<NEOPIXEL, A3>(leds, NUM_LEDS_PER_STRIP).setDither(BRIGHTNESS < 255);
  FastLED.addLeds<NEOPIXEL, A4>(leds, NUM_LEDS_PER_STRIP).setDither(BRIGHTNESS < 255);
  FastLED.addLeds<NEOPIXEL, A5>(leds, NUM_LEDS_PER_STRIP).setDither(BRIGHTNESS < 255);

  delay(3000); // 3 second delay for recovery

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  pride();
  FastLED.show();  
}

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}