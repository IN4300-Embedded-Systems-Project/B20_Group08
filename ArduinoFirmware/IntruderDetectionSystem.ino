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

void sendDataToAPI(int laserNumber) {
    Serial.println(F("Initializing API request..."));

    // Setup GPRS and APN Configuration
    sim800.println(F("AT+CIPSHUT"));
    delay(2000);
    printSIM800Response();

    sim800.println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""));
    delay(1000);
    printSIM800Response();

    sim800.println(F("AT+SAPBR=3,1,\"APN\",\"ppwap\""));
    delay(1000);
    printSIM800Response();

    sim800.println(F("AT+SAPBR=1,1"));
    delay(3000);
    printSIM800Response();

    sim800.println(F("AT+SAPBR=2,1"));
    delay(1000);
    printSIM800Response();

    sim800.println(F("AT+HTTPINIT"));
    delay(1000);
    printSIM800Response();

    sim800.println(F("AT+HTTPPARA=\"CID\",1"));
    delay(1000);
    printSIM800Response();

    sim800.println(F("AT+HTTPPARA=\"URL\",\"http://3.24.196.92:3000/postData\""));
    delay(1000);
    printSIM800Response();

    sim800.println(F("AT+HTTPPARA=\"CONTENT\",\"application/json\""));
    delay(1000);
    printSIM800Response();

    // Prepare JSON data
    String jsonData = "{\"lasertripped\": " + String(laserNumber) + "}";
    int dataLength = jsonData.length();

    // Send HTTP Data
    sim800.println("AT+HTTPDATA=" + String(dataLength) + ",10000");
    delay(1000);
    sim800.print(jsonData);
    delay(1000);
    sim800.write(26);

    delay(5000);
    printSIM800Response();

    // Send HTTP POST
    sim800.println(F("AT+HTTPACTION=1"));
    delay(5000);
    printSIM800Response();

    // Read response
    sim800.println(F("AT+HTTPREAD"));
    delay(1000);
    printSIM800Response();

    // Close HTTP session
    sim800.println(F("AT+HTTPTERM"));
    delay(1000);
    printSIM800Response();
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