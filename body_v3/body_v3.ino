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

// Global variables that impact LED behavior
#define BRIGHTNESS          55
#define FRAMES_PER_SECOND  120

// Other setup
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

uint8_t offset = 0; // rotating "base color" used by many of the patterns
uint8_t speed = 30;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Set up variables for motion detection & idle/sleep

float X, Y, Z;
#define MOVE_THRESHOLD 3
int stopDetected = 0;
#define IDLE_TIMER  180
#define SLEEP_TIMER 600

int wakingUp = 0;

boolean idling = false;
boolean tired = false;
boolean sleeping = false;

// Create variable to represent file indices (used to add wakeUp delay to compensate for sound clip played from eyes)
int fileSelect = 1;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Set up the bike matrix
// Ref: https://jasoncoon.github.io/led-mapper/?l=JA1-AjATADF0DMCAspKIOxUQDhwJxQryQqTHTHJlpkCsxAbMZgFASdRnfZecd+ISLHyRxQroP4jGkqUJFV5CrgSIrVEAi01awBZHv3Bc2AvGOrM+XBVxor-NpiqY5ToWyosRxKPjQVNAsiPCIaHjAKFQoLPTw9Gj0+Ew+uipsFIxEGFDY0BTQjNBEiMhY0RQojChE9Mj02EwUTIxMGvJs8GhiVJAscMDQaND4iFSILCTRaCj49FT0LEzwTGhM+JnI2BKQjJBESMPYiBSIjIhEKMgo2PTZjPRETMhMfJmY8JhGns6YFEwaEwLEwRFwyj+4EEmGwuHguGQuEYuC2UJA0hAuBYuCIBAo6OAmNABEhUOJJMc5LUHj+FJJH089JABDEsEsTMUsAoIl+VmZ6FgdFgvlgnT0AsFuz84n6+2MkrA4hoA24fQJXWM4lyQSVxQQvmguzGCFyYRwPImODoFxwTCAA

#define NUM_LEDS 147

byte coordsX[NUM_LEDS] = { 0, 0, 0, 0, 8, 8, 8, 8, 16, 16, 16, 16, 25, 25, 25, 25, 33, 33, 33, 33, 41, 41, 41, 41, 49, 49, 49, 49, 58, 58, 58, 58, 66, 66, 66, 66, 74, 74, 74, 74, 82, 82, 82, 82, 90, 90, 90, 90, 99, 99, 99, 99, 107, 107, 107, 107, 115, 115, 115, 115, 123, 123, 123, 123, 132, 132, 132, 132, 140, 140, 0, 0, 0, 8, 8, 8, 16, 16, 16, 25, 25, 25, 33, 33, 33, 41, 41, 41, 49, 49, 49, 58, 58, 58, 66, 66, 66, 74, 74, 74, 82, 82, 82, 90, 90, 90, 90, 99, 99, 99, 107, 107, 107, 115, 115, 115, 123, 123, 123, 132, 132, 132, 140, 140, 140, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255 };
byte coordsY[NUM_LEDS] = { 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 141, 149, 97, 141, 149, 97, 149, 158, 88, 149, 158, 88, 149, 158, 88, 158, 88, 79, 158, 167, 79, 167, 176, 70, 185, 193, 62, 202, 211, 53, 211, 220, 44, 220, 229, 35, 26, 237, 26, 18, 237, 26, 18, 246, 18, 9, 246, 18, 9, 255, 9, 0, 255, 9, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte angles[NUM_LEDS] = { 236, 0, 19, 223, 223, 0, 32, 210, 191, 0, 64, 191, 159, 128, 96, 172, 146, 128, 109, 159, 141, 128, 114, 151, 137, 128, 118, 146, 136, 128, 119, 143, 134, 128, 121, 141, 133, 128, 122, 139, 133, 128, 122, 137, 132, 128, 123, 136, 132, 128, 123, 136, 131, 128, 124, 135, 131, 128, 124, 134, 131, 128, 124, 134, 130, 128, 125, 133, 130, 128, 215, 210, 32, 204, 201, 45, 191, 191, 64, 181, 183, 77, 172, 176, 88, 169, 96, 90, 164, 167, 96, 163, 166, 96, 165, 167, 96, 166, 168, 96, 166, 167, 96, 165, 167, 96, 93, 166, 96, 94, 164, 98, 96, 164, 97, 96, 162, 99, 97, 162, 99, 97, 161, 100, 99, 156, 155, 154, 153, 152, 151, 151, 150, 149, 149, 148, 103, 104, 105, 106, 107, 107, 108, 109, 109, 110, 110 };
byte radii[NUM_LEDS] = { 17, 15, 17, 22, 11, 8, 11, 17, 8, 0, 8, 15, 11, 8, 11, 17, 17, 15, 17, 22, 24, 23, 24, 28, 32, 31, 32, 34, 39, 38, 39, 41, 47, 46, 47, 49, 54, 54, 54, 56, 62, 62, 62, 63, 70, 69, 70, 71, 77, 77, 77, 79, 85, 85, 85, 86, 93, 92, 93, 94, 100, 100, 100, 101, 108, 108, 108, 109, 116, 115, 28, 34, 22, 24, 32, 17, 31, 38, 23, 32, 39, 24, 34, 41, 28, 45, 33, 38, 49, 56, 44, 60, 66, 54, 77, 83, 65, 94, 100, 76, 105, 111, 87, 115, 122, 98, 104, 132, 109, 114, 137, 114, 120, 148, 125, 131, 153, 131, 136, 164, 142, 147, 169, 148, 153, 191, 197, 203, 209, 216, 222, 229, 235, 242, 248, 255, 177, 184, 190, 197, 203, 210, 217, 224, 231, 238, 245 };

CRGB leds[NUM_LEDS];

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void setup() {
  delay(1000); // 1 second delay for recovery

  Serial.begin(115200);

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
    //// 2D map examples:
    //northEastPalette, // Coollllll ISH. Front-to-Back fun design. Beating heart almost
    eastPalette,      // Great. Standard front-to-back. Could be faster?
    //southEastPalette, // Mehhhh kind of a weird flashing pattern. Front to back though
    //westPalette,      // Not bad. Back-to-front kind of deal.
    //northWestPalette, // A lot like northEast. Kind of flashy but good. Back-to-front.

    
    inwardPalette, // Not bad, use sparingly.
    outwardPalette, // Pretty solid. 

    //// Others
    confusedEast, // Good shit.
    confusedEastV2,
    niftyFrontBack,

    // FOR IDLE ONLY
    //clockwisePalette
    //southWestPalette
    experimentalPalette


    //experimentalPalette
    
    // standard FastLED demo reel examples:
    //  rainbow,

    //// Not built yet
    //  rainbowWithGlitter,
    //  confetti,

    //// POSSIBLE REJECTS
    //northPalette,     // weird, no like... COULD BE GOOD IDLE WITH RIGHT COLORS
    //southWestPalette, // Bad.
    //southPalette,     // Opposite of north palette... I like it a little better but ugh. Get rid of her.
    //clockwisePalette, // Fuck this. Honestly it's just confusing.
    //counterClockwisePalette, // Also just confusing. Fuck this one too.
    //beatwave, // I hate it!
    //juggle, // Honestly... chaos. But do I hate it? Not sure...
    //sinelon, // Mediochre at best.
    //bpm, // Chaotic horror
};

const uint8_t patternCount = ARRAY_SIZE(patterns);

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Define color palette options

uint8_t currentPatternIndex = 0; // Index number of which pattern is current
uint8_t resumePatternIndex = 0;  // Used to store the currentPatternIndex before switching to idle colors

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

// Gradient palette "bhw1_33_gp" renamed "purple", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_33.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( purple ) {
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
    RainbowColors_p,  // 0  //Rainbow, keep
    bhw1_04_gp,       // 1  // Purple orange yellow, GOOD
    wiki_knutux_gp,   // 2  // Orange to green, okay
    //purple,       // 3  // Purple - just purple. Pretty alright. Idle color?
    bhw4_057_gp,      // 4  // Pink & red, good. Love vibes
    bhw1_28_gp,       // 5  // Mermaid colors, good with front-to-back patterns
    Sunset_Real_gp,   // 6  // Heat map good
    purplefly_gp,     // 7  // Pretty good. It's a vibe.
    LavaColors_p,     // 8  // Lava, hot, pretty good. A little too scary maybe. Angry? Bumped?
    IceColors_p       // 9  // Better than CloudColors_p. Keep.

    //// POSSIBLE REJECTS
    //dreaming_gp,      // Purple/pink, faded, nice??? I kinda hate it tbh
    //Analogous_1_gp,   // Blue / red - stark, decent. Honestly get rid of in favor of Sunset_Real_gp
    //OceanColors_p,    // Sparkly ocean vibes, okay. Maybe get rid of it.
    //ForestColors_p,   // Green, earthy, okay. Maybe eliminate.
    //PartyColors_p,    // Tighter rainbow, pretty good... maybe prefer normal rainbow on the body.
};

const uint8_t paletteCount = ARRAY_SIZE(palettes);

// This sets the starting color palette
uint8_t currentPaletteIndex = 0;
CRGBPalette16 currentPalette = palettes[currentPaletteIndex];

uint8_t resumePaletteIndex = 0;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Begin running code

void loop() {
  // If we are not idling or tired or sleeping, run normal animations
  //if (! idling && ! tired && ! sleeping) {
  if ( stopDetected < IDLE_TIMER && stopDetected < SLEEP_TIMER ) {

    if ( wakingUp > IDLE_TIMER || wakingUp > SLEEP_TIMER ) {
      wakeUp();
    }

    // Call the current pattern function once, updating the 'leds' array
    patterns[currentPatternIndex]();

    offset = beat8(speed);

    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);

    EVERY_N_SECONDS( 90 ) { nextPattern(); }
    EVERY_N_SECONDS( 60 ) { nextPalette(); }
  }
  if ( stopDetected > IDLE_TIMER && stopDetected < SLEEP_TIMER ) {
    idle();
  }
  if ( ! idling && tired && ! sleeping ) {
    goToSleep();
  }
  if ( ! idling && ! tired && sleeping ) {
    sleep();
  }
  
  EVERY_N_SECONDS( 1 ) { checkMotion(); }

  EVERY_N_SECONDS( 90 ) { nextPattern(); }
  EVERY_N_SECONDS( 60 ) { nextPalette(); }
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Pattern/Color Changing Functions

void nextPattern() {
  Serial.println("Updating pattern.");
  currentPatternIndex = ( currentPatternIndex + 1 ) % ( ARRAY_SIZE( patterns ) - 1 );
}

void nextPalette() {
  currentPaletteIndex = ( currentPaletteIndex + 1 ) % ARRAY_SIZE ( palettes );
  currentPalette = palettes[currentPaletteIndex]; 
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Idle & Sleep Functions

void idle() {
  // This function will loop through idle animations when bool `idling` is true

  idling = true;
  tired = false;

  resumePaletteIndex = currentPaletteIndex;
  resumePatternIndex = currentPatternIndex;

  // Setup the idle animation
  currentPalette = purple;
  currentPatternIndex = 6; // index 6 is not looped through in the main loop()
  speed = 10;
  FastLED.setBrightness(30);

  // Loop until idling = false || tired = true
  // Check for motion. If motion detected, exit loop and wake up. Else, sleep when timer reached
  while (idling && ! tired) {
    // Run LEDs
    // Call the current pattern function once, updating the 'leds' array
    patterns[currentPatternIndex]();

    offset = beat8(speed);

    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);

    EVERY_N_SECONDS( 1 ) { checkMotion(); }

    if ( stopDetected > IDLE_TIMER && stopDetected > SLEEP_TIMER ) {
      idling = false;
      tired = true;
      goToSleep();
    }
    if ( stopDetected < IDLE_TIMER && stopDetected < SLEEP_TIMER ) {
      idling = false;
      tired = false;
      noSleepForYou();
    }
  }
}

void noSleepForYou() {
  // Do local wake up things
  wakingUp = 0;
  idling = false;
  tired = false;
  sleeping = false;
  Serial.println("Waking up.");
  Serial.print("Resuming palette: ");
  Serial.print(resumePaletteIndex);
  Serial.print(". Resuming pattern: ");
  Serial.println(resumePatternIndex);

  currentPatternIndex = resumePatternIndex;
  currentPaletteIndex = resumePaletteIndex;
  currentPalette = palettes[currentPaletteIndex]; 
  speed = 30;
  FastLED.setBrightness(BRIGHTNESS);
}

void goToSleep() {
  // This function will turn off all lights after idle() has been running for some time
  Serial.println("Going to sleep");

  idling = false;
  FastLED.clear(true);
  FastLED.setBrightness(0);

  sleeping = true;
  tired = false;

  //sleep();
}

void sleep() {
  EVERY_N_SECONDS( 5 )  { Serial.println("Still sleeping."); }
  // This function simply watches for motion & triggers wakeUp if detected
  FastLED.clear(true);

  if ( stopDetected < IDLE_TIMER && stopDetected < SLEEP_TIMER ) {
    wakeUp();
  }
}

void wakeUp() {

  if ( wakingUp > 1800 ) {
    soundClipDelay();
    wakingUp = 0;
  }

  wakingUp = 0;
  idling = false;
  tired = false;
  sleeping = false;
  Serial.println("Waking up.");
  Serial.print("Resuming palette: ");
  Serial.print(resumePaletteIndex);
  Serial.print(". Resuming pattern: ");
  Serial.println(resumePatternIndex);

  currentPatternIndex = resumePatternIndex;
  currentPaletteIndex = resumePaletteIndex;
  currentPalette = palettes[currentPaletteIndex]; 
  speed = 30;
  FastLED.setBrightness(BRIGHTNESS);
}

void soundClipDelay() {
  // This functions introduces a delay to account for the 'brain' playing a sound clip.
  // Ideally this will keep the lights on eyes vs body in sync as much as possible without communication
  if ( fileSelect = 0 ) {
    delay(1845);
  }
  else if ( fileSelect = 1 ) {
    delay(2000);
  }
  else if ( fileSelect = 2 ) {
    Serial.println("Playing astralPlanes.wav");
  }

  if ( fileSelect = 0 ) { 
    fileSelect = 1; 
  }
  else if ( fileSelect = 1 )
  {
    fileSelect = 2;
  }
  else if ( fileSelect = 2 )
  {
    fileSelect = 0;
  }
}

////////////////////////////////////////////////////////////
// Define the patterns
// 2D map examples:

void northPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset - coordsY[i]);
  }
}

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

void southPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset + coordsY[i]);
  }
}

void southWestPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset + coordsX[i] + coordsY[i]);
  }
}

void westPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset + coordsX[i]);
  }
}

void northWestPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset + coordsX[i] - coordsY[i]);
  }
}

// HOMEMADE
void confusedEast()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset - coordsX[i] + angles[i]);
  }
}

void confusedEastV2() {
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset - coordsX[i] - angles[i]); // Good. Kinda like confusedEast but different
  }
}

void niftyFrontBack() {
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset - (coordsX[i] + coordsY[i]) - radii[i]); // Nifty! Chaotic front to back KEEP
  }
}

// EXPERIMENTAL
void experimentalPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, offset - coordsX[i] + radii[i]); // Neat. Flashy but pleasing. Like diagonal upwards from front wheel. Keep
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
  fadeToBlackBy(leds, NUM_LEDS, 5); // 5 was originally 20
  //int pos = beatsin16(13, 0, NUM_LEDS - 1);
  int pos = beatsin16(6, 0, NUM_LEDS - 1);
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
    //leds[beatsin16(i + 8, 0, NUM_LEDS - 1)] |= CHSV(i * hues, 200, 255);
    leds[beatsin16(i + 3, 0, NUM_LEDS - 1)] |= CHSV(i * hues, 200, 255);
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

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Watch for motion 

void checkMotion() {

Serial.print("Checking motion...");

  //CircuitPlayground.clearPixels();
  X = CircuitPlayground.motionX();
  Y = CircuitPlayground.motionY();
  Z = CircuitPlayground.motionZ();
 
   // Get the magnitude (length) of the 3 axis vector
  // http://en.wikipedia.org/wiki/Euclidean_vector#Length
  double storedVector = X*X;
  storedVector += Y*Y;
  //double storedVector = Y*Y;
  //storedVector += Z*Z;
  //double storedVector = Z*Z;
  storedVector = sqrt(storedVector);
  Serial.print("Len: "); Serial.print(storedVector);
  
  // wait a bit
  //delay(100);
  
  // get new data!
  X = CircuitPlayground.motionX();
  Y = CircuitPlayground.motionY();
  Z = CircuitPlayground.motionZ();
  double newVector = X*X;
  newVector += Y*Y;
  //double newVector = Y*Y;
  //newVector += Z*Z;
  //double newVector = Z*Z;
  newVector = sqrt(newVector);
  Serial.print(" New Len: "); Serial.println(newVector);
  
  // are we moving 
  if (abs(10*newVector - 10*storedVector) > MOVE_THRESHOLD) {
    Serial.println("Moving!");
    wakingUp = stopDetected;
    stopDetected = 0;
  }
  else {
    if ( stopDetected > IDLE_TIMER-1 ) {
      stopDetected++;
      Serial.print("Stopped. Stop counter updating to ");
      Serial.println(stopDetected);
      return;
    }
    else {
      stopDetected++;
      Serial.print("Stopped. Stop counter updating to ");
      Serial.println(stopDetected);
    }
  }
}