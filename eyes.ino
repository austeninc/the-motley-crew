#include <Adafruit_Circuit_Playground.h>
#include <FastLED.h>

// Attempting to program eyes with matrices to match LED location
//
// Anticipating issues with this as I am not sure how to run two different 
// programs on two different data pins unless they are run in parallel
//
// Success or failure here will make or break this project as currently designed
//
// Best of luck, soldier.

////////////////////////////////////////////////////////////
// Set up the board & strips

#define DATA_PIN_L    A1
#define DATA_PIN_R    A2

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// Global variables that impact LED behavior
#define BRIGHTNESS 120          // Brightness
#define FRAMES_PER_SECOND 120   // Speed

// Other setup
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

uint8_t offset = 0; // rotating "base color" used by many of the patterns
uint8_t speed = 30;

boolean autoplay = true;
uint8_t autoplaySeconds = 2;

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
// The setup function....

void setup()
{
  //  delay(3000); // 3 second delay for recovery

  Serial.begin(9600);

  // tell FastLED about the LED strip configuration
  // LEFT
  FastLED.addLeds<LED_TYPE, DATA_PIN_L, COLOR_ORDER>(L_leds, NUM_LEDS_L);

  // RIGHT
  FastLED.addLeds<LED_TYPE, DATA_PIN_R, COLOR_ORDER>(R_leds, NUM_LEDS_R);

  // I don't think I need this line??? CircuitPlayground should handle appropriately.
  //FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000); // 1A

  FastLED.setBrightness(BRIGHTNESS);
}

////////////////////////////////////////////////////////////
// Define color & motion? palettes

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

uint8_t currentPatternIndex = 0; // Index number of which pattern is current

CRGBPalette16 IceColors_p = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

//typedef void (*SimplePaletteList[])
const CRGBPalette16 palettes[] = {
    RainbowColors_p,        // 0
    //RainbowStripeColors_p,  // 1
    CloudColors_p,          // 2
    LavaColors_p,           // 3
    OceanColors_p,          // 4
    ForestColors_p,         // 5
    PartyColors_p,          // 6
    HeatColors_p,           // 7
    IceColors_p            // 8
};

const uint8_t paletteCount = ARRAY_SIZE(palettes);

// This actually sets the current color palette
uint8_t currentPaletteIndex = 0;
CRGBPalette16 currentPalette = palettes[currentPaletteIndex];


////////////////////////////////////////////////////////////
// This runs the code!

void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  patterns[currentPatternIndex]();

  offset = beat8(speed);

  //L_clockwisePalette();
  //R_counterClockwisePalette();

  //counterClockwisePalette();

  //outwardPalette();

  // send the 'leds' array out to the actual LED strip
  // FastLED.show(); called automatically, internally by FastLED.delay below:

  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  EVERY_N_SECONDS( 5 ) { nextPattern(); }
  EVERY_N_SECONDS( 10 ) { 
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