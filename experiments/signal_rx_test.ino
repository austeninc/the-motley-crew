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
    pinMode(A3, INPUT);
}

void loop() {
    val = analogRead(A3);
    if (val > 0) {
        CircuitPlayground.redLED(true);
    }
    else {
        CircuitPlayground.redLED(false);
    }
}