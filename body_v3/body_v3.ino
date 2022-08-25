#include <Adafruit_Circuit_Playground.h>
#include <FastLED.h>
#include <SPI.h>         

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Set up the infrared codes
/* Removing IR code for now...
#define MY_PROTOCOL NECX
#define MY_BITS 32
#define IR_0      0xE0E0887736  
#define IR_1      0xE0E020DF37  
#define IR_2      0xE0E0A05F38  
#define IR_3      0xE0E0609F39  
#define IR_4      0xE0E010EF40  
#define IR_5      0xE0E0906F41  
#define IR_6      0xE0E050AF42  
#define IR_7      0xE0E030CF
#define IR_8      0xE0E0B04F
#define IR_9      0xE0E0708F
#define MY_MUTE   0xE0E0F00F
#define MY_POWER  0xE0E040BF

uint32_t decodedValue = 0;
*/

// Light detection though?
uint8_t lux;

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

//boolean autoplay = true;
//uint8_t autoplaySeconds = 2;

boolean idling = false;
boolean tired = false;
boolean sleeping = false;

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

  // Init the IR receiver
  CircuitPlayground.irReceiver.enableIRIn(); // Start the receiver
  Serial.println("Ready to receive IR signals");
  
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
    RainbowColors_p,  // 0  //Rainbow, keep
    bhw1_04_gp,       // 1  // Purple orange yellow, GOOD
    wiki_knutux_gp,   // 2  // Orange to green, okay
    bhw1_33_gp,       // 3  // Purple - just purple. Pretty alright. Idle color?
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

  // Watch for IR input
  /*
  if (CircuitPlayground.irReceiver.getResults() && ! CircuitPlayground.irDecoder.decode()) {
    CircuitPlayground.irReceiver.enableIRIn();
  }
  if (CircuitPlayground.irReceiver.getResults() && CircuitPlayground.irDecoder.decode()) {
    CircuitPlayground.irDecoder.dumpResults(false);
    CircuitPlayground.irReceiver.enableIRIn();
  }
  */

  

  // If we are not idling or tired or sleeping, run normal animations
  if (! idling && ! tired && ! sleeping) {
      // Call the current pattern function once, updating the 'leds' array
    patterns[currentPatternIndex]();

    offset = beat8(speed);

    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);

    EVERY_N_SECONDS( 90 ) { nextPattern(); }
    EVERY_N_SECONDS( 60 ) { nextPalette(); }
    EVERY_N_SECONDS( 5 )  { Serial.println("Still awake."); }
  }
  /*if (idling && ! tired && ! sleeping) {
    idle();
  }*/
  /*if (! idling && tired && ! sleeping) {
    for (uint16_t i = 0; i < 500; i++) {
      patterns[currentPatternIndex]();

      offset = beat8(speed);

      // insert a delay to keep the framerate modest
      FastLED.delay(1000 / FRAMES_PER_SECOND);
    }
    goToSleep();
  } */
  //if (! idling && ! tired && sleeping) {
  //  sleep();
  //}

  luxDetect();
  //getSignal();
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Pattern/Color Changing Functions

void nextPattern() {
  Serial.println("Updating pattern.");
  currentPatternIndex = ( currentPatternIndex + 1 ) % ARRAY_SIZE( patterns );
}

void nextPalette() {
  currentPaletteIndex = ( currentPaletteIndex + 1 ) % ARRAY_SIZE ( palettes );
  currentPalette = palettes[currentPaletteIndex]; 
  //Serial.print("Updating colors to ");  
  //Serial.println(currentPaletteIndex);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Infrared Signal Check
/* Removing IR code for now...
void getSignal() {
  if ( CircuitPlayground.irReceiver.getResults() ) {
    CircuitPlayground.irDecoder.decode();
    decodedValue = CircuitPlayground.irDecoder.value;
    //decodedValue = CircuitPlayground.irDecoder.value;
    //CircuitPlayground.irDecoder.dumpResults(false);
    switch ( decodedValue ) {
      case IR_0:  // If IR_0 is decoded, enter idling mode
        CircuitPlayground.irDecoder.dumpResults(false);
        Serial.println("Idle signal received.");
        idling = true;
        tired = false;
        sleeping = false;
        CircuitPlayground.irReceiver.enableIRIn();
        decodedValue = 0;
        break;
      case IR_1:  // If IR_1 is decoded, trigger go to sleep
        CircuitPlayground.irDecoder.dumpResults(false);
        Serial.println("Go to sleep signal received.");
        idling = false;
        tired = true;
        sleeping = false;
        CircuitPlayground.irReceiver.enableIRIn();
        decodedValue = 0;
        break;
      case IR_2:  // If IR_2 is decoded, wake up
        CircuitPlayground.irDecoder.dumpResults(false);
        Serial.println("Wake up signal received.");
        wakeUp();
        //idling = false;
        //tired = false;
        //sleeping = false;
        CircuitPlayground.irReceiver.enableIRIn();
        decodedValue = 0;
        break;
      //case IR_3:  // If IR_3 is decoded, go to sleep
        //CircuitPlayground.irDecoder.dumpResults(false);
        //Serial.println("Sleep signal received.");

      default:    // If nothing is decoded, move on
        if ( CircuitPlayground.irDecoder.decode() ) {
          CircuitPlayground.irDecoder.dumpResults(false);
          decodedValue = 0;
        }
        //EVERY_N_SECONDS(5) { Serial.println("No signals match last 5 seconds."); }
        CircuitPlayground.irReceiver.enableIRIn();
        break;
    }
  }

}
*/

// Light detection?

void luxDetect() {
  lux = CircuitPlayground.lightSensor();
  //Serial.print("lux = ");
  //Serial.println(lux);

  if ( lux > 100 && ! idling && ! tired && ! sleeping ) {
    Serial.println("Starting idle");
    //idling = true;
    //tired = false;
    //sleeping = false;
    delay(200);
    idle();
  }
  if ( lux > 100 && idling && ! tired && ! sleeping ) {
    //Serial.println("");
    //idling = false;
    //tired = true;
    //sleeping = false;

    /*
    for (uint16_t i = 0; i < 500; i++) {
      patterns[currentPatternIndex]();

      offset = beat8(speed);

      // insert a delay to keep the framerate modest
      FastLED.delay(1000 / FRAMES_PER_SECOND);
    }*/

    delay(450);
    lux = CircuitPlayground.lightSensor();
    if (lux > 100 && idling && ! tired && ! sleeping ) { 
      delay(400);
      goToSleep();
    }
    if (lux < 100 && idling && ! tired && ! sleeping ) { 
      Serial.println("No sleep for you!");
      delay(300);
      wakeUp();
    }
  }
  if ( lux > 100 && ! idling && ! tired && sleeping ) {
    Serial.println("Time to wake up!");
    delay(300);
    wakeUp();
    //idling = false;
    //tired = false;
    //sleeping = false;
  }
  
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Idle & Sleep Functions

void idle() {
  // This function will loop through idle animations when bool `idling` is true

  idling = true;

  Serial.println("Starting idle.");
  Serial.print("Palette before idle: ");
  Serial.print(currentPaletteIndex);
  Serial.print(". Pattern before idle: ");
  Serial.println(currentPatternIndex);

  resumePaletteIndex = currentPaletteIndex;
  resumePatternIndex = currentPatternIndex;

  // Setup the idle animation
  currentPalette = palettes[3];
  currentPatternIndex = 3;
  speed = 15;
  FastLED.setBrightness(30);

  //decodedValue = 0;

  // Loop until idling = false || tired = true
  // Listen for IR signal from eyes -- if resume signal recv'd, set idling = false
  while (idling && ! tired) {
    // Check for IR signals
    //getSignal();

    // Check for light signal
    luxDetect();

    // Run LEDs
    // Call the current pattern function once, updating the 'leds' array
    patterns[currentPatternIndex]();

    offset = beat8(speed);

    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);
  }
}

void goToSleep() {
  // This function will fade out all lights after idle() has been running for some time
  Serial.println("Going to sleep");
  idling = false;
  FastLED.clear(true);
  FastLED.setBrightness(0);
  sleeping = true;
  tired = false;
  sleep();
}

void sleep() {
  EVERY_N_SECONDS( 5 )  { Serial.println("Still sleeping."); }
  // This function resets the idle variables and then simply watches for an IR signal from the 'brain' CircuitPlayground before exiting
  //currentPatternIndex = resumePatternIndex;
  //currentPaletteIndex = resumePaletteIndex;
  //speed = 30;
  //FastLED.setBrightness(BRIGHTNESS);
  //getSignal();

  FastLED.clear(true);

  delay(20);

  luxDetect();
}

void wakeUp() {
  idling = false;
  tired = false;
  sleeping = false;
  Serial.println("Waking up.");
  Serial.print("Resuming palette: ");
  Serial.print(resumePaletteIndex);
  Serial.print(". Resuming pattern: ");
  Serial.println(resumePatternIndex);

  delay(50);

  currentPatternIndex = resumePatternIndex;
  currentPaletteIndex = resumePaletteIndex;
  currentPalette = palettes[currentPaletteIndex]; 
  speed = 30;
  FastLED.setBrightness(BRIGHTNESS);
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