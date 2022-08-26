// Include CP & FastLED libraries
#include <Adafruit_Circuit_Playground.h>
#include <FastLED.h>

// Include these libraries to access internal flash storage
#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>

// This is used to play sound. Thank you AloyseTech!
#include <Audio_FeatherM0.h>
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Set up accessing flash storage
//
// Define the board type... Circuit Playground Express is 
// not auto-recognized by the SPIFlash/Sdfat libaries
#define EXTERNAL_FLASH_DEVICES  GD25Q16C
#define EXTERNAL_FLASH_USE_SPI  SPI
#define EXTERNAL_FLASH_USE_CS   SS

// Initialize the flash device
Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS, EXTERNAL_FLASH_USE_SPI);
Adafruit_SPIFlash flash(&flashTransport);

// Create a file system object from SdFat
FatFileSystem fatfs;
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Set up audio player
SamdAudio AudioPlayer;

// Define audio properties
#define NUM_AUDIO_CHANNELS 1
#define AUDIO_BUFFER_SIZE 1024 //512 works fine for 22.05kh, use 1024 for 32khz and 44.1khz

// Define the audio file sample rate
const unsigned int sampleRate = 32000; //hz

// Assign file names to variables for playback
const char *filename0 = "fackinGooglyEyes-8bit.wav";  // 0
const char *filename1 = "oiClarence.wav";             // 1
const char *filename2 = "astralPlanes.wav";           // 2

// Create variable to represent file indices (for wakeUp function)
int fileSelect = 1;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Set up the board & LED strips

#define DATA_PIN_L    A1
#define DATA_PIN_R    A2

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// Global variables that impact LED behavior
#define BRIGHTNESS 55          // Brightness
#define FRAMES_PER_SECOND 120   // Smoothness

// Other setup
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

uint8_t offset = 0; // rotating "base color" used by many of the patterns
uint8_t speed = 30;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Set up variables for motion detection & idle/sleep

float X, Y, Z;
#define MOVE_THRESHOLD 4.5
int stopDetected = 0;
#define IDLE_TIMER  180
#define SLEEP_TIMER 600

int wakingUp = 0;

boolean idling = false;
boolean tired = false;
boolean sleeping = false;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Set up the eye matrices

#define NUM_LEDS 90

// Left eye
#define NUM_LEDS_L 90

byte L_coordsX[NUM_LEDS_L] = { 116, 151, 197, 232, 255, 255, 255, 243, 209, 174, 139, 104, 58, 35, 12, 0, 0, 23, 46, 93, 128, 162, 197, 220, 232, 232, 220, 185, 151, 116, 81, 46, 23, 12, 12, 35, 70, 104, 151, 185, 209, 209, 209, 185, 139, 104, 70, 35, 23, 23, 46, 81, 128, 162, 185, 197, 174, 151, 116, 70, 46, 58, 93, 139, 162, 174, 162, 128, 93, 58, 58, 81, 116, 151, 162, 151, 116, 81, 70, 81, 116, 151, 139, 104, 81, 81, 116, 139, 104, 93 };
byte L_coordsY[NUM_LEDS_L] = { 255, 255, 231, 206, 170, 134, 97, 61, 24, 12, 0, 0, 24, 36, 73, 121, 158, 194, 231, 243, 243, 231, 206, 182, 134, 97, 61, 36, 24, 12, 24, 49, 85, 121, 170, 206, 219, 231, 231, 206, 170, 134, 97, 61, 49, 49, 61, 85, 121, 170, 194, 219, 219, 206, 182, 134, 97, 73, 73, 85, 158, 194, 206, 206, 182, 146, 109, 85, 85, 121, 158, 194, 194, 182, 146, 109, 97, 109, 146, 182, 182, 158, 121, 109, 134, 170, 170, 146, 121, 146 };
byte L_angles[NUM_LEDS_L] = { 197, 185, 167, 152, 139, 128, 116, 105, 89, 77, 66, 55, 38, 31, 18, 4, 248, 235, 222, 206, 194, 179, 161, 147, 128, 114, 100, 85, 70, 58, 45, 30, 16, 4, 244, 230, 218, 204, 184, 165, 145, 128, 110, 90, 67, 50, 34, 18, 4, 243, 231, 214, 194, 175, 157, 128, 99, 76, 52, 26, 244, 228, 213, 188, 169, 139, 100, 59, 35, 6, 243, 221, 203, 177, 143, 90, 45, 17, 248, 226, 206, 165, 83, 27, 0, 231, 210, 172, 15, 244 };
byte L_radii[NUM_LEDS_L] = { 221, 221, 212, 227, 239, 229, 239, 245, 243, 231, 241, 246, 243, 255, 254, 252, 255, 235, 240, 211, 197, 183, 178, 186, 186, 197, 210, 201, 199, 221, 220, 224, 225, 230, 239, 227, 194, 183, 178, 164, 156, 142, 156, 164, 153, 162, 178, 205, 209, 218, 197, 182, 153, 142, 132, 120, 101, 114, 114, 149, 170, 179, 152, 132, 103, 80, 70, 88, 116, 144, 149, 147, 114, 93, 59, 55, 73, 108, 122, 132, 93, 55, 24, 70, 98, 118, 73, 24, 59, 80 };

CRGB L_leds[NUM_LEDS_L];

// Right eye
#define NUM_LEDS_R 90

#define NUM_LEDS 90

byte R_coordsX[NUM_LEDS] = { 153, 115, 77, 38, 26, 13, 0, 13, 26, 51, 89, 128, 166, 204, 230, 242, 255, 242, 230, 191, 166, 128, 89, 51, 26, 26, 26, 38, 64, 102, 140, 179, 217, 230, 242, 230, 204, 179, 140, 102, 64, 38, 38, 51, 64, 102, 140, 179, 204, 217, 217, 191, 153, 115, 77, 51, 51, 64, 89, 128, 179, 191, 191, 166, 128, 89, 64, 64, 89, 128, 153, 179, 166, 140, 102, 77, 77, 102, 140, 166, 166, 140, 102, 77, 89, 128, 153, 153, 115, 102 };
byte R_coordsY[NUM_LEDS] = { 255, 255, 255, 228, 201, 161, 121, 81, 40, 13, 0, 0, 0, 13, 54, 81, 121, 161, 201, 228, 242, 242, 242, 228, 188, 148, 107, 67, 40, 27, 27, 27, 54, 81, 121, 161, 201, 228, 242, 242, 228, 188, 148, 107, 81, 54, 54, 67, 94, 121, 161, 201, 228, 228, 215, 188, 148, 121, 81, 67, 94, 134, 174, 201, 215, 201, 174, 134, 94, 94, 107, 148, 188, 215, 201, 174, 134, 107, 107, 134, 174, 201, 188, 161, 121, 107, 134, 174, 188, 161 };
byte R_angles[NUM_LEDS] = { 185, 198, 210, 225, 233, 247, 4, 16, 28, 38, 50, 62, 74, 86, 100, 110, 123, 137, 151, 168, 179, 194, 208, 222, 237, 250, 9, 24, 37, 51, 66, 80, 97, 108, 123, 138, 157, 172, 189, 204, 218, 235, 250, 12, 26, 48, 67, 89, 107, 121, 140, 162, 183, 200, 217, 233, 249, 7, 35, 60, 99, 128, 151, 172, 195, 216, 235, 0, 29, 57, 90, 139, 169, 188, 210, 231, 0, 27, 74, 128, 163, 187, 214, 238, 11, 54, 128, 172, 206, 228 };
byte R_radii[NUM_LEDS] = { 210, 210, 231, 236, 227, 223, 242, 237, 253, 255, 243, 230, 237, 242, 221, 216, 219, 201, 207, 191, 193, 184, 201, 219, 216, 197, 201, 207, 204, 193, 184, 201, 203, 195, 197, 178, 171, 180, 184, 193, 204, 195, 174, 156, 156, 149, 138, 140, 144, 151, 156, 155, 164, 164, 172, 175, 151, 128, 122, 115, 106, 103, 124, 128, 138, 140, 144, 126, 106, 70, 57, 84, 108, 138, 128, 124, 103, 74, 47, 57, 90, 115, 108, 113, 84, 47, 34, 77, 98, 74 };

CRGB R_leds[NUM_LEDS_R];

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Begin SETUP functions.

void setup()
{
  delay(1000); // 1 second delay for recovery

  Serial.begin(115200);

  // Init Circuit Playground library & disable onboard speaker (it is shite)
  CircuitPlayground.begin();
  CircuitPlayground.speaker.off();
  //CircuitPlayground.strip.setBrightness(120); // Remove flash

  // Initialize flash library and check its chip ID.
  if (!flash.begin()) {
      Serial.println("Error, failed to initialize flash chip!");
      while(1);
  }
  Serial.print("Flash chip JEDEC ID: 0x"); Serial.println(flash.getJEDECID(), HEX);

  // First call begin to mount the filesystem.
  if (!fatfs.begin(&flash)) {   // Check that it returns true to make sure the FS was mounted.
      Serial.println("Failed to mount filesystem!");
      Serial.println("Was CircuitPython loaded on the board first to create the filesystem?");
      while(1);
  }
  Serial.println("Mounted filesystem!");

  // Initialize audio player
  Serial.print("Initializing Audio Player...");
  if (AudioPlayer.begin(sampleRate, NUM_AUDIO_CHANNELS, AUDIO_BUFFER_SIZE) == -1) 
  {
      Serial.println(" failed!");
      return;
  }
  Serial.println(" done.");

  // tell FastLED about the LED strip configuration
  Serial.println("Setting up lights...");
  FastLED.addLeds<LED_TYPE, DATA_PIN_L, COLOR_ORDER>(L_leds, NUM_LEDS_L); // LEFT EYE
  FastLED.addLeds<LED_TYPE, DATA_PIN_R, COLOR_ORDER>(R_leds, NUM_LEDS_R); // RIGHT EYE

  FastLED.setBrightness(BRIGHTNESS);

  // Play the wake up sound
  AudioPlayer.play(filename0, 0);
  Serial.println("Speaking first words.....");
  delay(1845);
  AudioPlayer.stopChannel(0);
  Serial.println("Let the light shine!");
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Define pattern options

typedef void (*SimplePatternList[])();
SimplePatternList patterns = {
    // 2D map examples:
    clockwisePalette,
    inwardPalette,
    outwardPalette,
    counterClockwisePalette,
    inwardPalette,
    outwardPalette
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

// Gradient palette "bhw1_33_gp" renamed purple, originally from
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
    purple,           // 3  // Purple - just purple. Pretty alright. Idle color?
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

void loop()
{
  if ( stopDetected < IDLE_TIMER && stopDetected < SLEEP_TIMER ) {

    if ( wakingUp > IDLE_TIMER || wakingUp > SLEEP_TIMER ) {
      wakeUp();
    }

    // Call the current pattern function once, updating the 'leds' array
    patterns[currentPatternIndex]();

    offset = beat8(speed);

    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);

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
  // This function will send a light pulse to the 'nerves' CircuitPlayground to have it begin its idle loop.
  // Then this function will loop through the eyes idle animations when bool `idling` is true

  idling = true;
  tired = false;

  /*
  // Flash light to trigger idle mode in nerves
  Serial.println("Flashing light");
  for (int i=0; i<10; ++i) {
      CircuitPlayground.strip.setPixelColor(i, 255, 0, 255);
  }
  CircuitPlayground.strip.show();
  delay(100);
  CircuitPlayground.strip.clear();
  CircuitPlayground.strip.show();
  */

  // Store the previous state
  resumePaletteIndex = currentPaletteIndex;
  resumePatternIndex = currentPatternIndex;

  // Setup the idle animation

  currentPalette = purple;

  //currentPalette = palettes[3];
  currentPatternIndex = 3;
  speed = 15;
  FastLED.setBrightness(30);

  // Loop until idling = false || tired = true
  // Check for motion -- if detected, trigger wake up with noSleepForYou
  // If motion is not detected, goToSleep
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
  /*
  // Flash the LEDs once to wake up again
  Serial.println("Flashing light");
  for (int i=0; i<10; ++i) {
      CircuitPlayground.strip.setPixelColor(i, 255, 0, 255);
  }
  CircuitPlayground.strip.show();
  delay(200);
  CircuitPlayground.strip.clear();
  CircuitPlayground.strip.show();
  */

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
  // This function will fade out all lights after idle() has been running for some time

  /*
  // Flash the LEDs twice to trigger goToSleep in nerves
  Serial.println("Flashing light");
  for (int i=0; i<10; ++i) {
      CircuitPlayground.strip.setPixelColor(i, 255, 0, 255);
  }
  CircuitPlayground.strip.show();
  delay(200);
  CircuitPlayground.strip.clear();
  CircuitPlayground.strip.show();
  delay(250);
  Serial.println("Flashing light");
  for (int i=0; i<10; ++i) {
      CircuitPlayground.strip.setPixelColor(i, 255, 0, 255);
  }
  CircuitPlayground.strip.show();
  delay(400);
  CircuitPlayground.strip.clear();
  CircuitPlayground.strip.show();
  */

  // Do sleepy things

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
  // This function checks for motion. If motion is detected, call wakeUp()
  FastLED.clear(true);

  if ( stopDetected < IDLE_TIMER && stopDetected < SLEEP_TIMER ) {
    wakeUp();
  }
}

void wakeUp() {
  // This function will play a random sound clip and then send a light signal to the 'nerves' to resume its normal loop

  // Play sound by calling soundClip() if we've been sleeping longer than 30 minutes, otherwise do nothing
  if ( wakingUp > 1800 ) {
    soundClip();
    wakingUp = 0;
  }

  // Flash light to trigger idle mode in nerves
  /*
  Serial.println("Flashing light");
  for (int i=0; i<10; ++i) {
      CircuitPlayground.strip.setPixelColor(i, 255, 0, 255);
  }
  CircuitPlayground.strip.show();
  delay(200);
  CircuitPlayground.strip.clear();
  CircuitPlayground.strip.show();
  */

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

void soundClip() {
  if ( fileSelect = 0 ) {
    AudioPlayer.play(filename0, 0);
    Serial.println("Playing fackinGooglyEyes-8bit.wav");
    delay(1845);
    AudioPlayer.stopChannel(0);
  }
  else if ( fileSelect = 1 ) {
    AudioPlayer.play(filename1, 0);
    Serial.println("Playing oiClarence.wav");
    delay(2000);
    AudioPlayer.stopChannel(0);
  }
  else if ( fileSelect = 2 ) {
    AudioPlayer.play(filename2, 0);
    Serial.println("Playing astralPlanes.wav");
    delay(5270);
    AudioPlayer.stopChannel(0);
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
// This IS the code!
// 2D map examples:

void clockwisePalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    L_leds[i] = ColorFromPalette(currentPalette, offset + L_angles[i]);
    R_leds[i] = ColorFromPalette(currentPalette, offset + R_angles[i]);
  }
}

void L_clockwisePalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    L_leds[i] = ColorFromPalette(currentPalette, offset + L_angles[i]);
  }
}

void counterClockwisePalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    L_leds[i] = ColorFromPalette(currentPalette, offset - L_angles[i]);
    R_leds[i] = ColorFromPalette(currentPalette, offset - R_angles[i]);
  }
}

void R_counterClockwisePalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    R_leds[i] = ColorFromPalette(currentPalette, offset - R_angles[i]);
  }
}

void outwardPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    L_leds[i] = ColorFromPalette(currentPalette, offset - L_radii[i]);
    R_leds[i] = ColorFromPalette(currentPalette, offset - R_radii[i]);
  }
}

void inwardPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    L_leds[i] = ColorFromPalette(currentPalette, offset + L_radii[i]);
    R_leds[i] = ColorFromPalette(currentPalette, offset + R_radii[i]);
  }
}

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