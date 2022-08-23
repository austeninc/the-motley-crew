#include <Adafruit_Circuit_Playground.h>
#include <FastLED.h>

///////////////////////////////////////////////////////////
// Goal: Test turning on LEDs on trigger sent by 
// another circuit playground.
//
// This is the transmitting board.
///////////////////////////////////////////////////////////

bool switch_status = false;

void setup() {
    CircuitPlayground.begin();
    pinMode(A7, OUTPUT);
    Serial.begin (9600);
}

void loop() {
    switch_status = CircuitPlayground.slideSwitch();
    Serial.println(switch_status);
    if (switch_status = true) {
        digitalWrite(A7, HIGH);
    } 
    else {
        digitalWrite(A7, LOW);
    }
    delay(1000);
}