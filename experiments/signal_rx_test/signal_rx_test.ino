#include <Adafruit_Circuit_Playground.h>
#include <FastLED.h>

///////////////////////////////////////////////////////////
// Goal: Test turning on LEDs on trigger sent by 
// another circuit playground.
//
// This is the recieving board
///////////////////////////////////////////////////////////

int val = 0;

void setup() {
    CircuitPlayground.begin();
    pinMode(A1, INPUT);
    Serial.begin (9600);
}

void loop() {
    val = digitalRead(A1);
    Serial.println(val);
    if (val = 0) {
        for (int n = 0; n < 10; n++) {
            CircuitPlayground.setPixelColor(n, 255, 255, 255);
        }
    }
    else {
        CircuitPlayground.clearPixels();
    }
    delay(1000);
}