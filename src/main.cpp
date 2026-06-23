#include <Arduino.h>

const int INC = 18;
const int UD  = 17;
const int CS  = 16;

void pulse() {
    digitalWrite(INC, LOW);
    delayMicroseconds(50);
    digitalWrite(INC, HIGH);
    delayMicroseconds(50);
}

void moveToZero() {
    digitalWrite(CS, LOW);
    digitalWrite(UD, LOW);

    for (int i = 0; i < 120; i++) {
        pulse();
    }

    digitalWrite(CS, HIGH);
}

void moveTo50() {
    digitalWrite(CS, LOW);
    digitalWrite(UD, HIGH);

    for (int i = 0; i < 50; i++) {
        pulse();
    }

    digitalWrite(CS, HIGH);
}

void moveTo99() {
    digitalWrite(CS, LOW);
    digitalWrite(UD, HIGH);

    for (int i = 0; i < 120; i++)
        pulse();

    digitalWrite(CS, HIGH);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(INC, OUTPUT);
    pinMode(UD, OUTPUT);
    pinMode(CS, OUTPUT);

    // 1. Switch to RED (Red=255, Green=0, Blue=0)
    neopixelWrite(RGB_BUILTIN, 255, 0, 0);
    digitalWrite(INC, HIGH);

    moveToZero();
}

void loop() {
    moveToZero();
    // 2. Switch to GREEN (Red=0, Green=255, Blue=0)
    neopixelWrite(RGB_BUILTIN, 0, 255, 0);
    delay(5000);

    moveTo99();
    // 3. Switch to BLUE (Red=0, Green=0, Blue=255)
    neopixelWrite(RGB_BUILTIN, 0, 0, 255);
    delay(5000);
}