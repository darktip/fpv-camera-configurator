#include <Arduino.h>

const int INC = 18;
const int UD  = 17;
const int CS  = 16;

int g_currentPosition = 0;

void pulseInc() 
{
    digitalWrite(INC, LOW);
    delay(2);
    digitalWrite(INC, HIGH);
    delay(2);
}

void csExistWithoutEepromSaving()
{
    digitalWrite(INC, LOW);
    delayMicroseconds(2);
    digitalWrite(CS, HIGH);
    digitalWrite(INC, HIGH);
}

void moveToZero() 
{
    digitalWrite(CS, LOW);
    digitalWrite(UD, LOW);
    delayMicroseconds(2);

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

    digitalWrite(CS, LOW);
    int increments = abs(targetPosition - g_currentPosition);
    int direction = (targetPosition > g_currentPosition) ? HIGH : LOW;
    digitalWrite(UD, direction);
    Serial.printf("Current=%d Target=%d Steps=%d\n", g_currentPosition, targetPosition, increments);
    delayMicroseconds(2);

    for (int i = 0; i < increments; i++) 
    {
        pulseInc();
    }

    csExistWithoutEepromSaving();
    g_currentPosition = targetPosition;
}

void setupPins() 
{
    pinMode(LED_BUILTIN, OUTPUT);

    digitalWrite(INC, HIGH);
    digitalWrite(CS, HIGH);
    digitalWrite(UD, LOW);
    
    pinMode(INC, OUTPUT);
    pinMode(CS, OUTPUT);
    pinMode(UD, OUTPUT);
}

void setupDefalts() 
{
    neopixelWrite(RGB_BUILTIN, 255, 0, 0);
}

void setup() 
{
    Serial.begin(115200);
    setupPins();
    setupDefalts();
    moveToZero();
}

void loop() 
{
    if (Serial.available()) 
    {
        String input = Serial.readStringUntil('\n');
        int targetPosition = input.toInt();
        
        // if (targetPosition == 0)
        // {
        //     moveToZero();
        // }
        // else if (targetPosition > 0 && targetPosition <= 99) 
        if (targetPosition >= 0 && targetPosition <= 99)
        {
            Serial.println("Moving to position: " + String(targetPosition));
            moveToTarget(targetPosition);
        } 
        else 
        {
            Serial.println("Invalid position! Enter 0-99");
        }
    }
    
    delay(100);
}