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
#define BRIGHTNESS          55
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
// Set up the bike matrix
// Ref: https://jasoncoon.github.io/led-mapper/?l=JA1-AjATADF0DMCAspKIOxUQDhwJxQryQqTHTHJlpkCsxAbMZgFASdRnfZecd+ISLHyRxQroP4jGkqUJFV5CrgSIrVEAi01awBZHv3Bc2AvGOrM+XBVxor-NpiqY5ToWyosRxKPjQVNAsiPCIaHjAKFQoLPTw9Gj0+Ew+uipsFIxEGFDY0BTQjNBEiMhY0RQojChE9Mj02EwUTIxMGvJs8GhiVJAscMDQaND4iFSILCTRaCj49FT0LEzwTGhM+JnI2BKQjJBESMPYiBSIjIhEKMgo2PTZjPRETMhMfJmY8JhGns6YFEwaEwLEwRFwyj+4EEmGwuHguGQuEYuC2UJA0hAuBYuCIBAo6OAmNABEhUOJJMc5LUHj+FJJH089JABDEsEsTMUsAoIl+VmZ6FgdFgvlgnT0AsFuz84n6+2MkrA4hoA24fQJXWM4lyQSVxQQvmguzGCFyYRwPImODoFxwTCAA

#define NUM_LEDS 147

//byte coordsX[NUM_LEDS] = { 0, 0, 0, 0, 8, 8, 8, 8, 16, 16, 16, 16, 25, 25, 25, 25, 33, 33, 33, 33, 41, 41, 41, 41, 49, 49, 49, 49, 58, 58, 58, 58, 66, 66, 66, 66, 74, 74, 74, 74, 82, 82, 82, 82, 90, 90, 90, 90, 99, 99, 99, 99, 107, 107, 107, 107, 115, 115, 115, 115, 123, 123, 123, 123, 132, 132, 132, 132, 140, 140, 0, 0, 0, 8, 8, 8, 16, 16, 16, 25, 25, 25, 33, 33, 33, 41, 41, 41, 49, 49, 49, 58, 58, 58, 66, 66, 66, 74, 74, 74, 82, 82, 82, 90, 90, 90, 99, 99, 99, 107, 107, 107, 115, 115, 115, 123, 123, 123, 132, 132, 132, 140, 140, 140, 148, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255 };
//byte coordsY[NUM_LEDS] = { 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 137, 146, 91, 137, 146, 91, 146, 155, 82, 146, 155, 82, 146, 155, 82, 155, 164, 73, 155, 164, 73, 164, 173, 64, 182, 191, 55, 200, 209, 46, 209, 219, 36, 219, 228, 27, 228, 237, 18, 228, 237, 18, 237, 246, 9, 237, 246, 9, 246, 255, 0, 246, 255, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//byte angles[NUM_LEDS] = { 4, 6, 9, 1, 4, 7, 9, 1, 4, 7, 10, 1, 5, 8, 11, 2, 5, 8, 12, 2, 6, 9, 13, 2, 6, 10, 14, 2, 7, 11, 15, 2, 8, 12, 17, 3, 9, 14, 19, 3, 10, 16, 21, 3, 12, 19, 25, 4, 15, 23, 29, 5, 19, 28, 35, 7, 26, 36, 43, 10, 40, 48, 52, 19, 64, 64, 64, 64, 88, 79, 254, 251, 11, 254, 251, 12, 251, 248, 15, 250, 247, 16, 250, 247, 17, 246, 242, 22, 245, 241, 23, 240, 236, 28, 231, 227, 33, 222, 219, 38, 216, 214, 43, 211, 209, 47, 206, 205, 51, 203, 202, 54, 198, 198, 58, 195, 194, 61, 191, 191, 64, 188, 188, 67, 69, 177, 174, 172, 170, 167, 165, 164, 162, 160, 159, 157, 77, 80, 82, 84, 86, 88, 90, 92, 93, 95, 96 };
//byte radii[NUM_LEDS] = { 196, 198, 200, 196, 184, 186, 188, 183, 172, 174, 176, 171, 160, 162, 165, 159, 148, 150, 153, 147, 136, 138, 141, 135, 124, 126, 129, 122, 112, 114, 118, 110, 99, 102, 107, 98, 88, 91, 96, 86, 76, 79, 85, 74, 64, 68, 75, 61, 52, 58, 65, 49, 41, 48, 56, 37, 31, 39, 49, 25, 22, 33, 44, 14, 18, 31, 43, 6, 22, 33, 196, 196, 203, 183, 184, 191, 172, 174, 184, 160, 162, 173, 148, 150, 161, 138, 141, 156, 126, 129, 146, 118, 123, 143, 119, 126, 143, 125, 135, 144, 127, 137, 148, 131, 142, 153, 137, 149, 160, 133, 145, 157, 143, 155, 167, 141, 153, 165, 153, 165, 177, 153, 165, 178, 179, 176, 181, 186, 192, 198, 205, 213, 221, 229, 238, 247, 187, 192, 197, 202, 209, 215, 222, 230, 238, 246, 255 };

//// Attempting to move the center to the front of the bike...

//byte coordsX[NUM_LEDS] = { 0, 0, 0, 0, 8, 8, 8, 8, 16, 16, 16, 16, 25, 25, 25, 25, 33, 33, 33, 33, 41, 41, 41, 41, 49, 49, 49, 49, 58, 58, 58, 58, 66, 66, 66, 66, 74, 74, 74, 74, 82, 82, 82, 82, 90, 90, 90, 90, 99, 99, 99, 99, 107, 107, 107, 107, 115, 115, 115, 115, 123, 123, 123, 123, 132, 132, 132, 132, 140, 140, 0, 0, 0, 8, 8, 8, 16, 16, 16, 25, 25, 25, 33, 33, 33, 41, 41, 41, 49, 49, 49, 58, 58, 58, 66, 66, 66, 74, 74, 74, 82, 82, 82, 90, 90, 90, 99, 99, 99, 107, 107, 107, 115, 115, 115, 123, 123, 123, 132, 132, 132, 140, 140, 140, 148, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255 };
//byte coordsY[NUM_LEDS] = { 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 100, 128, 118, 109, 137, 146, 91, 137, 146, 91, 146, 155, 82, 146, 155, 82, 146, 155, 82, 155, 164, 73, 155, 164, 73, 164, 173, 64, 182, 191, 55, 200, 209, 46, 209, 219, 36, 219, 228, 27, 228, 237, 18, 228, 237, 18, 237, 246, 9, 237, 246, 9, 246, 255, 0, 246, 255, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//byte angles[NUM_LEDS] = { 64, 64, 64, 64, 88, 79, 75, 109, 101, 91, 85, 118, 109, 99, 93, 121, 113, 105, 98, 122, 116, 109, 103, 123, 118, 111, 106, 124, 119, 114, 109, 125, 120, 115, 111, 125, 121, 117, 112, 125, 121, 118, 114, 125, 122, 118, 115, 126, 122, 119, 116, 126, 123, 120, 117, 126, 123, 120, 118, 126, 123, 121, 118, 126, 124, 121, 119, 126, 124, 122, 191, 191, 64, 146, 167, 73, 154, 164, 78, 146, 156, 84, 142, 150, 89, 146, 152, 90, 144, 149, 94, 146, 151, 94, 152, 155, 94, 156, 158, 95, 156, 158, 95, 156, 158, 95, 157, 159, 95, 155, 157, 96, 155, 157, 96, 154, 156, 98, 154, 156, 98, 153, 155, 99, 100, 151, 150, 149, 148, 148, 147, 146, 146, 145, 145, 144, 103, 104, 105, 105, 106, 107, 107, 108, 109, 109, 110 };
//byte radii[NUM_LEDS] = { 11, 19, 26, 4, 13, 20, 27, 8, 19, 24, 30, 15, 25, 29, 34, 23, 32, 35, 40, 30, 39, 42, 45, 37, 46, 48, 52, 45, 53, 55, 58, 52, 61, 62, 65, 60, 68, 70, 72, 67, 75, 77, 79, 75, 83, 84, 86, 82, 90, 91, 93, 89, 98, 99, 100, 97, 105, 106, 108, 104, 112, 113, 115, 112, 120, 121, 122, 119, 127, 128, 4, 11, 34, 8, 13, 34, 19, 24, 44, 25, 29, 47, 32, 35, 51, 42, 45, 61, 48, 52, 66, 58, 62, 76, 72, 77, 87, 87, 92, 98, 98, 103, 108, 108, 113, 119, 119, 124, 129, 125, 129, 134, 135, 140, 145, 141, 145, 150, 151, 156, 161, 157, 162, 166, 172, 186, 192, 199, 205, 212, 218, 225, 232, 238, 245, 252, 190, 196, 203, 209, 215, 222, 228, 235, 242, 248, 255 };

//// Alternative layout...

//byte coordsX[NUM_LEDS] = { 0, 0, 0, 0, 8, 8, 8, 8, 16, 16, 16, 16, 25, 25, 25, 25, 33, 33, 33, 33, 41, 41, 41, 41, 49, 49, 49, 49, 58, 58, 58, 58, 66, 66, 66, 66, 74, 74, 74, 74, 82, 82, 82, 82, 90, 90, 90, 90, 99, 99, 99, 99, 107, 107, 107, 107, 115, 115, 115, 115, 123, 123, 123, 123, 132, 132, 132, 132, 140, 140, 0, 0, 0, 8, 8, 8, 16, 16, 16, 25, 25, 25, 33, 33, 33, 41, 41, 41, 49, 49, 49, 58, 58, 58, 66, 66, 66, 74, 74, 74, 82, 82, 82, 90, 90, 90, 90, 99, 99, 99, 107, 107, 107, 115, 115, 115, 123, 123, 123, 132, 132, 132, 140, 140, 140, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255 };
//byte coordsY[NUM_LEDS] = { 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 106, 132, 123, 114, 141, 149, 97, 141, 149, 97, 149, 158, 88, 149, 158, 88, 149, 158, 88, 158, 88, 79, 158, 167, 79, 167, 176, 70, 185, 193, 62, 202, 211, 53, 211, 220, 44, 220, 229, 35, 26, 237, 26, 18, 237, 26, 18, 246, 18, 9, 246, 18, 9, 255, 9, 0, 255, 9, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//byte angles[NUM_LEDS] = { 3, 5, 8, 0, 3, 5, 8, 0, 3, 6, 9, 0, 3, 6, 9, 0, 3, 7, 10, 0, 4, 7, 11, 0, 4, 8, 12, 0, 4, 9, 13, 0, 5, 10, 15, 0, 6, 11, 16, 0, 7, 13, 19, 0, 8, 15, 22, 0, 10, 19, 26, 0, 13, 24, 32, 0, 19, 32, 40, 0, 32, 45, 51, 0, 64, 64, 64, 0, 96, 83, 252, 250, 10, 252, 250, 11, 249, 246, 14, 249, 246, 15, 248, 245, 16, 244, 17, 20, 243, 240, 22, 238, 234, 27, 229, 226, 32, 220, 218, 37, 215, 213, 42, 210, 209, 46, 48, 204, 51, 52, 201, 54, 55, 197, 58, 58, 194, 61, 61, 191, 64, 64, 188, 67, 66, 177, 175, 172, 170, 168, 166, 164, 162, 161, 159, 158, 77, 79, 81, 84, 86, 88, 89, 91, 93, 94, 96 };
//byte radii[NUM_LEDS] = { 193, 194, 196, 192, 181, 182, 184, 180, 169, 170, 172, 168, 157, 158, 160, 156, 145, 146, 149, 144, 133, 134, 137, 132, 121, 123, 126, 120, 109, 111, 114, 108, 97, 99, 103, 96, 85, 88, 92, 84, 73, 76, 81, 72, 61, 65, 70, 60, 50, 54, 60, 48, 38, 43, 51, 36, 27, 34, 43, 24, 17, 27, 38, 12, 12, 24, 36, 0, 17, 27, 193, 194, 198, 181, 182, 187, 170, 172, 179, 158, 160, 167, 146, 149, 156, 137, 145, 151, 126, 129, 140, 118, 124, 137, 120, 128, 136, 128, 137, 137, 130, 140, 140, 134, 145, 145, 156, 152, 152, 164, 149, 149, 160, 158, 158, 170, 157, 157, 169, 168, 168, 180, 169, 169, 181, 179, 183, 188, 194, 200, 207, 214, 222, 230, 238, 247, 190, 194, 199, 204, 210, 217, 224, 231, 239, 247, 255 };

//// Alternative layout, updated center
// This layout WINS! Looks like inward & outward are coming from the heart

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
    wiki_knutux_gp,   // Orange to green, okay
    bhw1_33_gp,       // Purple - just purple. Pretty alright. Idle color?
    bhw4_057_gp,      // Pink & red, good. Love vibes
    bhw1_28_gp,       // Mermaid colors, good with front-to-back patterns
    Sunset_Real_gp,   // Heat map good
    purplefly_gp,     // Pretty good. It's a vibe.
    LavaColors_p,     // Lava, hot, pretty good. A little too scary maybe. Angry? Bumped?
    IceColors_p       // Better than CloudColors_p. Keep.

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

  if (CircuitPlayground.irReceiver.getResults()) {
    if (CircuitPlayground.irDecoder.decode()) {
        CircuitPlayground.irDecoder.dumpResults(false);
    }
    CircuitPlayground.irReceiver.enableIRIn();
  }

  // Call the current pattern function once, updating the 'leds' array
  patterns[currentPatternIndex]();

  offset = beat8(speed);

  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  EVERY_N_SECONDS( 20 ) { nextPattern(); }
  EVERY_N_SECONDS( 10 ) { nextPalette(); }
}

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