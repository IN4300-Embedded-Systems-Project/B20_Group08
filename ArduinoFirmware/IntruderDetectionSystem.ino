#include <Wire.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// SIM800L Module
SoftwareSerial sim800(10, 11); // RX, TX

const int numLasers = 2;
const int laserPins[numLasers] = {2, 3};
const int ldrPins[numLasers] = {A0, A1};
const int buzzerPin = 6;
const int ledRed = 7;
const int ledGreen = 8;
const int resetButton = 9;
int threshold;

bool intrusionDetected = false;
bool smsSent = false;
int interruptedLaser = -1;
unsigned long intrusionTime = 0;

void setup() {
    Serial.begin(9600);
    sim800.begin(9600);
    
    Serial.println(F("System Booting..."));

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("OLED init failed!"));
        for (;;);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println(F("System Initializing..."));
    display.display();
    delay(2000);

    for (int i = 0; i < numLasers; i++) {
        pinMode(laserPins[i], OUTPUT);
        digitalWrite(laserPins[i], HIGH);
    }

    pinMode(buzzerPin, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(resetButton, INPUT_PULLUP);

    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledRed, LOW);
    digitalWrite(buzzerPin, LOW);

    Serial.println(F("System Armed!"));

    display.clearDisplay();
    display.setCursor(0, 10);
    display.println(F("System Armed"));
    display.display();
}

void loop() {
    if (digitalRead(resetButton) == LOW) {
        resetSystem();
    }

    for (int i = 0; i < numLasers; i++) {
        int ldrValue = analogRead(ldrPins[i]);

        if (ldrValue < threshold && !intrusionDetected) {
            intrusionDetected = true;
            interruptedLaser = i;
            intrusionTime = millis();
            Serial.print(F("Intrusion detected at Laser: "));
            Serial.println(interruptedLaser + 1);
        }
    }

    if (intrusionDetected && !smsSent) {
        triggerAlarm();
        sendDataToAPI(interruptedLaser + 1);
        smsSent = true;
    }
}

void triggerAlarm() {
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);
    digitalWrite(buzzerPin, HIGH);

    display.clearDisplay();
    display.setCursor(0, 10);
    display.println(F("Intrusion Detected!"));
    display.setCursor(0, 20);
    display.print(F("Laser: "));
    display.println(interruptedLaser + 1);
    display.setCursor(0, 30);
    display.print(F("Time: "));
    display.print(intrusionTime / 1000);
    display.println(F("s"));
    display.display();

    Serial.println(F("Alarm Triggered!"));
}


void resetSystem() {
    Serial.println(F("System Reset"));

    display.clearDisplay();
    display.setCursor(0, 10);
    display.println(F("System Resetting..."));
    display.display();
    delay(500);

    intrusionDetected = false;
    smsSent = false;
    interruptedLaser = -1;
    intrusionTime = 0;

    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);
    digitalWrite(buzzerPin, LOW);

    display.clearDisplay();
    display.setCursor(0, 10);
    display.println(F("System Armed"));
    display.display();
}

void printSIM800Response() {
    while (sim800.available()) {
        Serial.write(sim800.read());
    }
    Serial.println();
}