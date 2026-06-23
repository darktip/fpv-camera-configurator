#include <Arduino.h>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // 1. Switch to RED (Red=255, Green=0, Blue=0)
  neopixelWrite(RGB_BUILTIN, 255, 0, 0);
  delay(1000);

  // 2. Switch to GREEN (Red=0, Green=255, Blue=0)
  neopixelWrite(RGB_BUILTIN, 0, 255, 0);
  delay(1000);

  // 3. Switch to BLUE (Red=0, Green=0, Blue=255)
  neopixelWrite(RGB_BUILTIN, 0, 0, 255);
  delay(1000);
}