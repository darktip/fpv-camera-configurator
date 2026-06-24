#include <Arduino.h>

const int INC    = 18;
const int UD     = 17;
const int CS     = 16;
const int BUTTON = 20;

const int ENTER = 14;
const int LEFT  = 21;
const int RIGHT = 27;
const int UP    = 25;
const int DOWN  = 99; // DOWN is not supported because potentiometer cannot produce 0kOm

int g_currentPosition = 0;
bool g_buttonWasDown = false;

int sequenceMista1800[] =
{
    ENTER,
    UP,UP,UP,
    ENTER,ENTER,
    LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,
    UP,UP,UP,
    RIGHT,
    UP,UP,
    ENTER,
    UP,UP,UP,UP,UP,UP,
    ENTER,
    UP,UP,UP,UP,UP,
    RIGHT,
    UP,UP,UP,UP,UP,UP,
    LEFT,LEFT,LEFT,LEFT,
    UP,UP,UP,UP,UP,UP,
    LEFT,
    UP,UP,UP,UP,
    ENTER,
    UP,UP,UP,UP,UP,
    ENTER
};

void pulseInc() 
{
    digitalWrite(INC, LOW);
    delayMicroseconds(50);
    digitalWrite(INC, HIGH);
    delayMicroseconds(50);
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

bool isButtonPressed() 
{
    bool pressed = digitalRead(BUTTON) == LOW;

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
    delay(25);
    moveToTarget(targetPosition);
    delay(100);
    moveToZero();
}

void executeSequence()
{
    neopixelWrite(RGB_BUILTIN, 0, 0, 255);

    for (int i = 0; i < sizeof(sequenceMista1800) / sizeof(sequenceMista1800[0]); i++) 
    {
        int targetPosition = sequenceMista1800[i];
        pulsePotentiometerPosition(targetPosition);
        delay(50);
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
    Serial.begin(115200);
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
    pinMode(BUTTON, INPUT_PULLUP);
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

    delay(100);
}
