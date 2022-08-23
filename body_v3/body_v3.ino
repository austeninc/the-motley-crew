#include <Adafruit_Circuit_Playground.h>
#include <FastLED.h>
#include <SPI.h>         

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Set up the board & LED strips

// Identifying data pins
#define LED_TYPE    NEOPIXEL
#define COLOR_ORDER GRB

//#define NUM_LEDS_PER_STRIP    70
//#define NUM_LEDS    NUM_LEDS_PER_STRIP
//#define NUM_STRIPS  4
//CRGB ledsA1[NUM_LEDS_PER_STRIP];
//CRGB ledsA2[NUM_LEDS_PER_STRIP];
//CRGB ledsA3[NUM_LEDS_PER_STRIP];
//CRGB ledsA4[NUM_LEDS_PER_STRIP];
//CRGB ledsA5[NUM_LEDS_PER_STRIP];
//CRGB leds[NUM_LEDS_PER_STRIP];

// Global variables that impact LED behavior
#define BRIGHTNESS          120
#define FRAMES_PER_SECOND  60

// Other setup
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

uint8_t offset = 0; // rotating "base color" used by many of the patterns
uint8_t speed = 30;

boolean autoplay = true;
uint8_t autoplaySeconds = 2;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Set up the bike matrix
// Ref: https://jasoncoon.github.io/led-mapper/?l=JA1-AjATADF0DMCAspKIOxUQDhwJxQryQqTHTHJlpkCsxAbMZgFASdRnfZecd+ISLHyRxQroP4jGkqUJFV5CrgSIrVEAi01awBZHv3Bc2AvGOrM+XBVxor-NpiqY5ToWyosRxKPjQVNAsiPCIaHjAKFQoLPTw9Gj0+Ew+uipsFIxEGFDY0BTQjNBEiMhY0RQojChE9Mj02EwUTIxMGvJs8GhiVJAscMDQaND4iFSILCTRaCj49FT0LEzwTGhM+JnI2BKQjJBESMPYiBSIjIhEKMgo2PTZjPRETMhMfJmY8JhGns6YFEwaEwLEwRFwyj+4EEmGwuHguGQuEYuC2UJA0hAuBYuCIBAo6OAmNABEhUOJJMc5LUHj+FJJH089JABDEsEsTMUsAoIl+VmZ6FgdFgvlgnT0AsFuz84n6+2MkrA4hoA24fQJXWM4lyQSVxQQvmguzGCFyYRwPImODoFxwTCAA

#define NUM_LEDS 147

byte coordsX[NUM_LEDS] = { 0, 0, 0, 0, 8, 8, 8, 8, 16, 16, 16, 16, 25, 25, 25, 25, 33, 33, 33, 33, 41, 41, 41, 41, 49, 49, 49, 49, 58, 58, 58, 58, 66, 66, 66, 66, 74, 74, 74, 74, 82, 82, 82, 82, 90, 90, 90, 90, 99, 99, 99, 99, 107, 107, 107, 107, 115, 115, 115, 115, 123, 123, 123, 123, 132, 132, 132, 132, 140, 140, 0, 0, 0, 8, 8, 8, 16, 16, 16, 25, 25, 25, 33, 33, 33, 41, 41, 41, 49, 49, 49, 58, 58, 58, 66, 66, 66, 74, 74, 74, 82, 82, 82, 90, 90, 90, 99, 99, 99, 107, 107, 107, 115, 115, 115, 123, 123, 123, 132, 132, 132, 140, 140, 140, 148, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255 };
byte coordsY[NUM_LEDS] = { 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 137, 146, 91, 137, 146, 91, 146, 155, 82, 146, 155, 82, 146, 155, 82, 155, 164, 73, 155, 164, 73, 164, 173, 64, 182, 191, 55, 200, 209, 46, 209, 219, 36, 219, 228, 27, 228, 237, 18, 228, 237, 18, 237, 246, 9, 237, 246, 9, 246, 255, 0, 246, 255, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte angles[NUM_LEDS] = { 4, 6, 9, 1, 4, 7, 9, 1, 4, 7, 10, 1, 5, 8, 11, 2, 5, 8, 12, 2, 6, 9, 13, 2, 6, 10, 14, 2, 7, 11, 15, 2, 8, 12, 17, 3, 9, 14, 19, 3, 10, 16, 21, 3, 12, 19, 25, 4, 15, 23, 29, 5, 19, 28, 35, 7, 26, 36, 43, 10, 40, 48, 52, 19, 64, 64, 64, 64, 88, 79, 254, 251, 11, 254, 251, 12, 251, 248, 15, 250, 247, 16, 250, 247, 17, 246, 242, 22, 245, 241, 23, 240, 236, 28, 231, 227, 33, 222, 219, 38, 216, 214, 43, 211, 209, 47, 206, 205, 51, 203, 202, 54, 198, 198, 58, 195, 194, 61, 191, 191, 64, 188, 188, 67, 69, 177, 174, 172, 170, 167, 165, 164, 162, 160, 159, 157, 77, 80, 82, 84, 86, 88, 90, 92, 93, 95, 96 };
byte radii[NUM_LEDS] = { 196, 198, 200, 196, 184, 186, 188, 183, 172, 174, 176, 171, 160, 162, 165, 159, 148, 150, 153, 147, 136, 138, 141, 135, 124, 126, 129, 122, 112, 114, 118, 110, 99, 102, 107, 98, 88, 91, 96, 86, 76, 79, 85, 74, 64, 68, 75, 61, 52, 58, 65, 49, 41, 48, 56, 37, 31, 39, 49, 25, 22, 33, 44, 14, 18, 31, 43, 6, 22, 33, 196, 196, 203, 183, 184, 191, 172, 174, 184, 160, 162, 173, 148, 150, 161, 138, 141, 156, 126, 129, 146, 118, 123, 143, 119, 126, 143, 125, 135, 144, 127, 137, 148, 131, 142, 153, 137, 149, 160, 133, 145, 157, 143, 155, 167, 141, 153, 165, 153, 165, 177, 153, 165, 178, 179, 176, 181, 186, 192, 198, 205, 213, 221, 229, 238, 247, 187, 192, 197, 202, 209, 215, 222, 230, 238, 246, 255 };

CRGB leds[NUM_LEDS];

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void setup() {
  delay(1000); // 1 second delay for recovery

  Serial.begin(9600);

  // Init Circuit Playground library & disable onboard speaker (it is shite)
  CircuitPlayground.begin();
  CircuitPlayground.speaker.off();
  
  // tell FastLED about the LED strip configuration
  Serial.println("Initializing LED objects...");
  FastLED.addLeds<NEOPIXEL, A3>(leds, 0, 70); // Top Tube
  FastLED.addLeds<NEOPIXEL, A5>(leds, 70, 55); // Down Tube
  FastLED.addLeds<NEOPIXEL, A2>(leds, 125, 11); // Left Chainstay
  FastLED.addLeds<NEOPIXEL, A1>(leds, 136, 11); // Right Chainstay

  delay(2000); // 2 second delay for recovery
  Serial.println("Done.");

  Serial.print("Setting master brightness to ");
  Serial.println(BRIGHTNESS);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Define pattern options

typedef void (*SimplePatternList[])();
SimplePatternList patterns = {
    // 2D map examples:
    bpm,
    bpm,
    bpm,
    //beatwave,
    //clockwisePalette,
    //experimentalPalette,
    outwardPalette,
    southEastPalette,
    westPalette,
    northEastPalette,
    juggle,
    sinelon,
    eastPalette,
    outwardPalette,
    counterClockwisePalette,
    inwardPalette
//    northPalette,
//    southEastPalette,
//    southPalette,
//    southWestPalette,
//    westPalette,
//    northWestPalette,

    // standard FastLED demo reel examples:
    //  rainbow,
    //  rainbowWithGlitter,
    //  confetti,
    //  sinelon,
    //  juggle,
    //  bpm
};

const uint8_t patternCount = ARRAY_SIZE(patterns);

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Define color palette options

uint8_t currentPatternIndex = 0; // Index number of which pattern is current

CRGBPalette16 IceColors_p = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

// Gradient palette "purplefly_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/rc/tn/purplefly.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.
DEFINE_GRADIENT_PALETTE( purplefly_gp ) {
    0,   0,  0,  0,
   63, 239,  0,122,
  191, 252,255, 78,
  255,   0,  0,  0};

// Gradient palette "Sunset_Real_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/atmospheric/tn/Sunset_Real.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( Sunset_Real_gp ) {
    0, 120,  0,  0,
   22, 179, 22,  0,
   51, 255,104,  0,
   85, 167, 22, 18,
  135, 100,  0,103,
  198,  16,  0,130,
  255,   0,  0,160};

// Gradient palette "bhw1_28_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_28.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_28_gp ) {
    0,  75,  1,221,
   30, 252, 73,255,
   48, 169,  0,242,
  119,   0,149,242,
  170,  43,  0,242,
  206, 252, 73,255,
  232,  78, 12,214,
  255,   0,149,242};

// Gradient palette "Analogous_1_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/red/tn/Analogous_1.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE( Analogous_1_gp ) {
    0,   3,  0,255,
   63,  23,  0,255,
  127,  67,  0,255,
  191, 142,  0, 45,
  255, 255,  0,  0};

// Gradient palette "bhw4_057_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw4/tn/bhw4_057.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw4_057_gp ) {
    0,   6,  0,  1,
   22,  17,  0,  1,
   58,  20,  1,  2,
   91,  75,  0, 12,
  127, 199,  1, 41,
  204,  17,  0,  1,
  255,   6,  0,  1};

// Gradient palette "bhw1_33_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_33.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_33_gp ) {
    0,   2,  1,  8,
   94,  79,  2,212,
  140, 110, 11,197,
  255,   2,  1,  8};

// Gradient palette "wiki_knutux_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/wkp/knutux/tn/wiki-knutux.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 64 bytes of program space.

DEFINE_GRADIENT_PALETTE( wiki_knutux_gp ) {
    0,  10,108, 18,
   31,  10,108, 18,
   31,  32,141, 15,
   63,  32,141, 15,
   63, 104,173, 23,
   95, 104,173, 23,
   95, 177,187, 23,
  127, 177,187, 23,
  127, 255,205,  1,
  159, 255,205,  1,
  159, 255,159,  1,
  191, 255,159,  1,
  191, 255, 86,  1,
  223, 255, 86,  1,
  223, 255, 51,  1,
  255, 255, 51,  1};

// Gradient palette "bhw1_04_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_04.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_04_gp ) {
    0, 229,227,  1,
   15, 227,101,  3,
  142,  40,  1, 80,
  198,  17,  1, 79,
  255,   0,  0, 45};

// Gradient palette "dreaming_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/jjg/neo10/liht/tn/dreaming.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 80 bytes of program space.

DEFINE_GRADIENT_PALETTE( dreaming_gp ) {
    0, 229,100,228,
   25, 229,100,228,
   25, 165, 66,188,
   51, 165, 66,188,
   51, 107, 41,151,
   76, 107, 41,151,
   76,  64, 25,119,
  102,  64, 25,119,
  102,  32, 13, 91,
  127,  32, 13, 91,
  127,  20,  4, 66,
  153,  20,  4, 66,
  153,  13,  3, 47,
  178,  13,  3, 47,
  178,   8,  3, 31,
  204,   8,  3, 31,
  204,   5,  2, 19,
  229,   5,  2, 19,
  229,   2,  1, 10,
  255,   2,  1, 10};



//typedef void (*SimplePaletteList[])
const CRGBPalette16 palettes[] = {
    RainbowColors_p,  // Rainbow, keep
    bhw1_04_gp,       // Purple orange yellow, GOOD
    dreaming_gp,      // Purple/pink, faded, nice
    wiki_knutux_gp,   // Orange to green, okay
    bhw1_33_gp,       // Purple - just purple. Pretty good
    bhw4_057_gp,      // Pink & red, good
    Analogous_1_gp,   // Blue / red - stark, decent.
    bhw1_28_gp,       // Mermaid colors, okay
    Sunset_Real_gp,   // Heat map good
    purplefly_gp,     // decent, not sure
    LavaColors_p,     // Lava, hot, pretty good
    OceanColors_p,    // Sparkly ocean vibes, okay
    ForestColors_p,   // Green, earthy, okay
    PartyColors_p,    // Tighter rainbow, pretty good
    IceColors_p       // Better than CloudColors_p
};

const uint8_t paletteCount = ARRAY_SIZE(palettes);

// This sets the starting color palette
uint8_t currentPaletteIndex = 0;
CRGBPalette16 currentPalette = palettes[currentPaletteIndex];

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Begin running code

void loop() {
  // Call the current pattern function once, updating the 'leds' array
  patterns[currentPatternIndex]();

  offset = beat8(speed);

  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  EVERY_N_SECONDS( 20 ) { nextPattern(); }
  EVERY_N_SECONDS( 20 ) { 
    nextPalette();
  }
}

void nextPattern() {
  Serial.println("Updating pattern.");
  currentPatternIndex = ( currentPatternIndex + 1 ) % ARRAY_SIZE( patterns );
}

void nextPalette() {
  currentPaletteIndex = ( currentPaletteIndex + 1 ) % ARRAY_SIZE ( palettes );
  currentPalette = palettes[currentPaletteIndex]; 
  Serial.print("Updating colors to ");
  Serial.println(currentPaletteIndex);
}

////////////////////////////////////////////////////////////
// This IS the code!
// 2D map examples:
void northEastPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset - (coordsX[i] + coordsY[i]));
  }
}

void eastPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset - coordsX[i]);
  }
}

void southEastPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset - coordsX[i] + coordsY[i]);
  }
}

// EXPERIMENTAL
void experimentalPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset - coordsX[i] + angles[i]);
  }
}

void westPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset + coordsX[i]);
  }
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 30;
  CRGBPalette16 palette = currentPalette;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  //uint8_t beat = beatsin8(BeatsPerMinute, 0, 400);
  for (int i = 0; i < NUM_LEDS; i++)
  { // 9948
    leds[i] = ColorFromPalette(palette, offset + (i * 2), beat - offset + (i * 10));
  }
}

void clockwisePalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset + angles[i]);
  }
}

void counterClockwisePalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset - angles[i]);

  }
}

void outwardPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset - radii[i]);

  }
}

void inwardPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset + radii[i]);
  }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(offset, 255, 192);
}

const byte dotCount = 4;
const byte hues = 240 / dotCount;

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (int i = 0; i < dotCount; i++)
  {
    leds[beatsin16(i + 8, 0, NUM_LEDS - 1)] |= CHSV(i * hues, 200, 255);
  }
}

void beatwave() {
  
  uint8_t wave1 = beatsin8(9, 0, 255);                        // That's the same as beatsin8(9);
  uint8_t wave2 = beatsin8(8, 0, 255);
  uint8_t wave3 = beatsin8(7, 0, 255);
  uint8_t wave4 = beatsin8(6, 0, 255);

  for (int i=0; i<NUM_LEDS; i++) {
    //leds[i] = ColorFromPalette( currentPalette, i+wave1+wave2+wave3+wave4, 255, currentBlending); 
    leds[i] = ColorFromPalette( currentPalette, i+wave1+wave2+wave3+wave4, 255); 
  }
  
} // beatwave()

//////////////////////////////////////////////
// Possible color blending feature?
// See https://github.com/atuline/FastLED-Demos/blob/master/beatwave/beatwave.ino


//EVERY_N_MILLISECONDS(100) {
//    uint8_t maxChanges = 24; 
//    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
//  }

//  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
//    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
//  }