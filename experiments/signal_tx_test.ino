#include <Adafruit_Circuit_Playground.h>
#include <FastLED.h>

///////////////////////////////////////////////////////////
// Goal: Test turning on LEDs on trigger sent by 
// another circuit playground.
//
// This is the transmitting board.
///////////////////////////////////////////////////////////

bool switch_status = CircuitPlayground.slideSwitch();

void setup() {
    pinMode(A3, OUTPUT);
}

void loop() {
    switch_status = CircuitPlayground.slideSwitch();
    if (switch_status = true) {
        analogWrite(A3, 255);
    } 
    else {
        analogWrite(A3, 0);
    }
}