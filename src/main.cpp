#include <Arduino.h>
#include "sequence.h"

const int INC_PIN    = 18;
const int UD_PIN     = 17;
const int CS_PIN     = 16;
const int BUTTON_PIN = 20;

const int SERIAL_BAUD_RATE = 115200;

const int INC_PULSE_DELAY = 50; // microseconds
const int POTENTIOMETER_MIN_DELAY = 2; // microseconds
const int ZERO_PULSE_DELAY = 50; // miliseconds
const int ENTER_PULSE_DELAY = 115; // miliseconds
const int MOVE_PULSE_DELAY = 75; // miliseconds
const int SEQUENCE_DELAY = 80; // miliseconds
const int LOOP_DELAY = 100; // miliseconds

int g_currentPosition = 0;
bool g_buttonWasDown = false;

void pulseInc() 
{
    digitalWrite(INC_PIN, LOW);
    delayMicroseconds(INC_PULSE_DELAY);
    digitalWrite(INC_PIN, HIGH);
    delayMicroseconds(INC_PULSE_DELAY);
}

void csExistWithoutEepromSaving()
{
    digitalWrite(INC_PIN, LOW);
    delayMicroseconds(POTENTIOMETER_MIN_DELAY);
    digitalWrite(CS_PIN, HIGH);
    digitalWrite(INC_PIN, HIGH);
}

void moveToZero() 
{
    digitalWrite(CS_PIN, LOW);
    digitalWrite(UD_PIN, LOW);
    delayMicroseconds(POTENTIOMETER_MIN_DELAY);

    for (int i = 0; i < 120; i++) {
        pulseInc();
    }

    csExistWithoutEepromSaving();

    g_currentPosition = 0;
}

void moveToTarget(int targetPosition) 
{
    if (targetPosition == g_currentPosition) 
    {
        return;
    }

    if (targetPosition < 0) 
    {
      targetPosition = 0;
    }
    else if (targetPosition > 99) 
    {
      targetPosition = 99;
    }

    digitalWrite(CS_PIN, LOW);
    int increments = abs(targetPosition - g_currentPosition);
    int direction = (targetPosition > g_currentPosition) ? HIGH : LOW;
    digitalWrite(UD_PIN, direction);
    Serial.printf("Current=%d Target=%d Steps=%d\n", g_currentPosition, targetPosition, increments);
    delayMicroseconds(POTENTIOMETER_MIN_DELAY);

    for (int i = 0; i < increments; i++) 
    {
        pulseInc();
    }

    csExistWithoutEepromSaving();
    g_currentPosition = targetPosition;
}

bool isButtonPressed() 
{
    bool pressed = digitalRead(BUTTON_PIN) == LOW;

    if (pressed && !g_buttonWasDown) 
    {
        g_buttonWasDown = true;
        return false;
    }

    if (!pressed && g_buttonWasDown) 
    {
        g_buttonWasDown = false;
        return true;
    }

    return false;
}

void pulsePotentiometerPosition(int targetPosition)
{
    Serial.println("Moving to position: " + String(targetPosition));
    moveToZero();
    delay(ZERO_PULSE_DELAY);
    
    moveToTarget(targetPosition);
    int delayTime = (targetPosition == ENTER) ? ENTER_PULSE_DELAY : MOVE_PULSE_DELAY;
    delay(delayTime);

    moveToZero();
}

void executeSequence()
{
    neopixelWrite(RGB_BUILTIN, 0, 0, 255);

    for (int i = 0; i < sizeof(sequenceMista1800) / sizeof(sequenceMista1800[0]); i++) 
    {
        int targetPosition = sequenceMista1800[i];
        pulsePotentiometerPosition(targetPosition);
        delay(SEQUENCE_DELAY);
    }

    neopixelWrite(RGB_BUILTIN, 0, 255, 0);
}

void readSerial()
{
    if (!Serial.available())
    {
        return;
    }
    String input = Serial.readStringUntil('\n');
    int targetPosition = 0;

    switch (input.charAt(0))
    {
    case 'w':
        targetPosition = UP;
        break;
    case 'a':
        targetPosition = LEFT;
        break;
    case 'd':
        targetPosition = RIGHT;
        break;
    case 's':
        targetPosition = DOWN;
        break;
    case 'e':
        targetPosition = ENTER;
        break;
    default:
        targetPosition = input.toInt();
    }

    if (targetPosition == 0)
    {
        moveToZero();
    }
    else if (targetPosition > 0 && targetPosition <= 99)
    {
        pulsePotentiometerPosition(targetPosition);
    }
    else
    {
        Serial.println("Invalid position! Enter 0-99");
    }
}

void setupSerial() 
{
    Serial.begin(SERIAL_BAUD_RATE);
}

void setupPins() 
{
    pinMode(LED_BUILTIN, OUTPUT);

    digitalWrite(INC_PIN, HIGH);
    digitalWrite(CS_PIN, HIGH);
    digitalWrite(UD_PIN, LOW);
    
    pinMode(INC_PIN, OUTPUT);
    pinMode(CS_PIN, OUTPUT);
    pinMode(UD_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void setupDefalts() 
{
    neopixelWrite(RGB_BUILTIN, 0, 255, 0);
}

void setup() 
{
    setupSerial();
    setupPins();
    setupDefalts();
    moveToZero();
}

void loop() 
{
    readSerial();

    if (isButtonPressed()) 
    {
        executeSequence();
    }

    delay(LOOP_DELAY);
}
