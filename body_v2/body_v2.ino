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

#define NUM_LEDS_PER_STRIP    70
#define NUM_LEDS    NUM_LEDS_PER_STRIP
//#define NUM_STRIPS  4
//CRGB ledsA1[NUM_LEDS_PER_STRIP];
//CRGB ledsA2[NUM_LEDS_PER_STRIP];
//CRGB ledsA3[NUM_LEDS_PER_STRIP];
//CRGB ledsA4[NUM_LEDS_PER_STRIP];
//CRGB ledsA5[NUM_LEDS_PER_STRIP];
//CRGB leds[NUM_LEDS_PER_STRIP];

// Global variables that impact LED behavior
#define BRIGHTNESS          120
#define FRAMES_PER_SECOND  120

// Other setup
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

uint8_t offset = 0; // rotating "base color" used by many of the patterns
uint8_t speed = 30;

boolean autoplay = true;
uint8_t autoplaySeconds = 2;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Set up the eye matrices

// Top Tube
#define NUM_LEDS_TT 70 // Top Tube

byte coordsX_TT[NUM_LEDS_TT] = { 0, 0, 0, 0, 15, 15, 15, 15, 30, 30, 30, 30, 45, 45, 45, 45, 60, 60, 60, 60, 75, 75, 75, 75, 90, 90, 90, 90, 105, 105, 105, 105, 120, 120, 120, 120, 135, 135, 135, 135, 150, 150, 150, 150, 165, 165, 165, 165, 180, 180, 180, 180, 195, 195, 195, 195, 210, 210, 210, 210, 225, 225, 225, 225, 240, 240, 240, 240, 255, 255 };
byte coordsY_TT[NUM_LEDS_TT] = { 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85, 170, 255, 0, 85 };
byte angles_TT[NUM_LEDS_TT] = { 9, 4, 0, 251, 10, 5, 0, 250, 11, 6, 0, 249, 13, 7, 0, 248, 15, 8, 0, 247, 19, 10, 0, 245, 24, 13, 0, 242, 32, 19, 0, 236, 45, 32, 0, 223, 64, 64, 0, 191, 83, 96, 128, 159, 96, 109, 128, 146, 104, 114, 128, 141, 109, 118, 128, 137, 112, 119, 128, 136, 114, 121, 128, 134, 116, 122, 128, 133, 118, 122 };
byte radii_TT[NUM_LEDS_TT] = { 255, 250, 249, 250, 228, 223, 221, 223, 201, 196, 194, 196, 175, 168, 166, 168, 149, 141, 138, 141, 124, 114, 111, 114, 100, 87, 83, 87, 78, 62, 55, 62, 62, 39, 28, 39, 55, 28, 0, 28, 62, 39, 28, 39, 78, 62, 55, 62, 100, 87, 83, 87, 124, 114, 111, 114, 149, 141, 138, 141, 175, 168, 166, 168, 201, 196, 194, 196, 228, 223 };

CRGB leds_TT[NUM_LEDS_TT];

// Down Tube
#define NUM_LEDS_DT 55 // Down Tube

byte coordsX_DT[NUM_LEDS_DT] = { 0, 0, 0, 14, 14, 14, 28, 28, 28, 43, 43, 43, 57, 57, 57, 71, 71, 71, 85, 85, 85, 99, 99, 99, 113, 113, 113, 128, 128, 128, 142, 142, 142, 156, 156, 156, 170, 170, 170, 184, 184, 184, 198, 198, 198, 213, 213, 213, 227, 227, 227, 241, 241, 241, 255 };
byte coordsY_DT[NUM_LEDS_DT] = { 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0, 128, 255, 0 };
byte angles_DT[NUM_LEDS_DT] = { 6, 2, 253, 7, 2, 253, 8, 3, 252, 9, 3, 252, 11, 4, 251, 13, 4, 251, 16, 6, 249, 22, 8, 247, 32, 13, 242, 51, 32, 223, 77, 96, 159, 96, 114, 141, 106, 119, 136, 111, 122, 133, 114, 123, 132, 117, 124, 131, 118, 124, 131, 119, 125, 130, 120 };
byte radii_DT[NUM_LEDS_DT] = { 255, 252, 252, 229, 226, 226, 203, 199, 199, 177, 173, 173, 151, 146, 146, 126, 120, 120, 101, 94, 94, 77, 68, 68, 56, 42, 42, 42, 19, 19, 42, 19, 19, 56, 42, 42, 77, 68, 68, 101, 94, 94, 126, 120, 120, 151, 146, 146, 177, 173, 173, 203, 199, 199, 229 };

CRGB leds_DT[NUM_LEDS_DT];

// Chain Stays
#define NUM_LEDS_CS 11 // Chain Stays

byte coordsX_CS[NUM_LEDS_CS] = { 0, 26, 51, 77, 102, 128, 153, 179, 204, 230, 255 };
byte coordsY_CS[NUM_LEDS_CS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte angles_CS[NUM_LEDS_CS] = { 4, 4, 6, 8, 13, 32, 96, 114, 119, 122, 123 };
byte radii_CS[NUM_LEDS_CS] = { 255, 209, 163, 118, 73, 33, 33, 73, 118, 163, 209 };

CRGB leds_CS[NUM_LEDS_CS];

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
  FastLED.addLeds<NEOPIXEL, A3>(leds_TT, NUM_LEDS_TT); // Top Tube
  FastLED.addLeds<NEOPIXEL, A5>(leds_DT, NUM_LEDS_TT); // Down Tube
  FastLED.addLeds<NEOPIXEL, A2>(leds_CS, NUM_LEDS_TT); // Left Chainstay
  FastLED.addLeds<NEOPIXEL, A3>(leds_CS, NUM_LEDS_TT); // Right Chainstay

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
    clockwisePalette,
    outwardPalette,
    counterClockwisePalette,
    inwardPalette
//    northPalette,
//    northEastPalette,
//    eastPalette,
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

  EVERY_N_SECONDS( 60 ) { nextPattern(); }
  EVERY_N_SECONDS( 60 ) { 
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


void clockwisePalette()
{
  for (uint16_t i = 0; i < NUM_LEDS_TT; i++)
  {
    leds_TT[i] = ColorFromPalette(currentPalette, offset + angles_TT[i]);
    leds_DT[i] = ColorFromPalette(currentPalette, offset + angles_DT[i]);
    leds_CS[i] = ColorFromPalette(currentPalette, offset + angles_CS[i]);
  }
}

void counterClockwisePalette()
{
  for (uint16_t i = 0; i < NUM_LEDS_TT; i++)
  {
    leds_TT[i] = ColorFromPalette(currentPalette, offset - angles_TT[i]);
    leds_DT[i] = ColorFromPalette(currentPalette, offset - angles_DT[i]);
    leds_CS[i] = ColorFromPalette(currentPalette, offset - angles_CS[i]);

  }
}

void outwardPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS_TT; i++)
  {
    leds_TT[i] = ColorFromPalette(currentPalette, offset - radii_TT[i]);
    leds_DT[i] = ColorFromPalette(currentPalette, offset - radii_DT[i]);
    leds_CS[i] = ColorFromPalette(currentPalette, offset - radii_CS[i]);

  }
}

void inwardPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS_TT; i++)
  {
    leds_TT[i] = ColorFromPalette(currentPalette, offset + radii_TT[i]);
    leds_DT[i] = ColorFromPalette(currentPalette, offset + radii_DT[i]);
    leds_CS[i] = ColorFromPalette(currentPalette, offset + radii_CS[i]);
  }
}