#include <Wire.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// SIM800L Module for GSM communication
SoftwareSerial sim800(10, 11); // RX, TX

// Laser and LDR sensor configuration
const int numLasers = 2;
const int laserPins[numLasers] = {2, 3}; // Laser transmitter pins
const int ldrPins[numLasers] = {A0, A1}; // LDR sensor pins

// Output device configuration
const int buzzerPin = 6;
const int ledRed = 7;      // Red LED indicates an alert
const int ledGreen = 8;    // Green LED indicates system is armed
const int resetButton = 9; // Reset button for manual reset
int threshold;             // LDR threshold value for detection

// Intrusion detection variables
bool intrusionDetected = false;
bool smsSent = false;
int interruptedLaser = -1;
unsigned long intrusionTime = 0;

void setup() {
    Serial.begin(9600);
    sim800.begin(9600);
    
    Serial.println(F("System Booting..."));

    // Initialize the OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("OLED init failed!"));
        for (;;); // Halt the system if the display fails
    }

    // Display system initialization message
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println(F("System Initializing..."));
    display.display();
    delay(2000);

    // Configure laser transmitters as outputs and turn them on
    for (int i = 0; i < numLasers; i++) {
        pinMode(laserPins[i], OUTPUT);
        digitalWrite(laserPins[i], HIGH);
    }

    // Configure output devices
    pinMode(buzzerPin, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(resetButton, INPUT_PULLUP);

    // Default state: system armed, green LED on, red LED and buzzer off
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledRed, LOW);
    digitalWrite(buzzerPin, LOW);

    // Calibrate the LDR threshold for intrusion detection
    calibrateThreshold();

    Serial.println(F("System Armed!"));

    // Display system armed message
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println(F("System Armed"));
    display.display();
}

void loop() {
    // Check if the reset button is pressed
    if (digitalRead(resetButton) == LOW) {
        resetSystem();
    }

    // Check laser sensors for any interruptions
    for (int i = 0; i < numLasers; i++) {
        int ldrValue = analogRead(ldrPins[i]);

        // If an intrusion is detected for the first time
        if (ldrValue < threshold && !intrusionDetected) {
            intrusionDetected = true;
            interruptedLaser = i;
            intrusionTime = millis();
            Serial.print(F("Intrusion detected at Laser: "));
            Serial.println(interruptedLaser + 1);
        }
    }

    // If an intrusion is detected and an SMS hasn't been sent yet
    if (intrusionDetected && !smsSent) {
        triggerAlarm();
        sendSMS();
        sendDataToAPI(interruptedLaser + 1);
        smsSent = true;
    }
}

// Function to activate alarm when intrusion is detected
void triggerAlarm() {
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);
    digitalWrite(buzzerPin, HIGH);

    // Display intrusion alert on OLED
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

// Function to send SMS alert using SIM800L
void sendSMS() {
    Serial.println(F("Sending SMS..."));
    display.setCursor(0, 40);
    display.println(F("Sending SMS..."));
    display.display();
    delay(500);

    sim800.println(F("AT+CMGF=1")); // Set SMS mode to text
    delay(500);
    sim800.println(F("AT+CMGS=\"+94763226659\"")); // Enter recipient's phone number
    delay(500);
    sim800.print(F("Intruder Alert! Laser "));
    sim800.print(interruptedLaser + 1);
    sim800.print(F(" interrupted at "));
    sim800.print(intrusionTime / 1000);
    sim800.println(F(" sec."));
    delay(500);
    sim800.write(26); // End SMS message

    Serial.println(F("SMS Sent!"));
}

// Function to send data to an API server using HTTP POST
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

    // Send HTTP POST request
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

// Function to reset the system state
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

// Function to calibrate LDR threshold for intrusion detection
void calibrateThreshold() {
    Serial.println(F("Calibrating Sensors..."));

    display.clearDisplay();
    display.setCursor(0, 10);
    display.println(F("Calibrating..."));
    display.display();

    int totalLdrValue = 0;
    int readings = 5;

    for (int i = 0; i < readings; i++) {
        for (int j = 0; j < numLasers; j++) {
            totalLdrValue += analogRead(ldrPins[j]);
        }
        delay(100);
    }

    threshold = (totalLdrValue / (readings * numLasers)) - 100;
    Serial.print(F("Threshold set: "));
    Serial.println(threshold);
}

// Function to print SIM800L responses for debugging
void printSIM800Response() {
    while (sim800.available()) {
        Serial.write(sim800.read());
    }
    Serial.println();
}
