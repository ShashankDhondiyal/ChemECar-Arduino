#include <SPI.h>
#include <SD.h>
#include "max6675.h"

const int THERMO_DO = 4;														// Data Out ThermoCouple
const int THERMO_CS = 5;														// Chip Select ThermoCouple
const int THERMO_CLK = 6;;														// Clock ThermoCouple
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO);							// ThermoCouple Configuration

const float TEMP_THRESHOLD = 50.0;												// ***---> Calibratable Temprature Threshold <---***

const int SD_CS = 8;															// MicroSD Card

const int RELAY_PIN = 7;														// Relay
bool hasSwitched = false;														// Relay Switch Condition

const int VOLTAGE_SENSOR = A0;													// Voltage Sensor

const int LED_R = 3;															// RGB_LED Red
const int LED_G = 9;															// RGB_LED Green
const int LED_B = 10;															// RGB_LED Blue
const float VOLTAGE_BLUE_MAX = 6.99;											// Battery : Voltage Sensor -> Low Threshold
const float VOLTAGE_GREEN_MAX = 9.99;											// Battery : Voltage Sensor -> Good Threshold
const float VOLTAGE_ORANGE_MAX = 12.00;											// Battery : Voltage Sensor -> High Threshold 


const unsigned long SAMPLE_INTERVAL = 250;										// For tracking milliseconds


void setup() {
  Serial.begin(9600);															// init
  pinMode(RELAY_PIN, OUTPUT);													// Define relay signal type
  digitalWrite(RELAY_PIN, HIGH);												// Set signal to high

  pinMode(LED_R, OUTPUT);														// Define rgb_red signal type
  pinMode(LED_G, OUTPUT);														// Define rgb_green signal type
  pinMode(LED_B, OUTPUT);														// Define rgb_blue signal type

  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card error!");
    while(1);
  }
}

void loop() {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= SAMPLE_INTERVAL) {						// If the delta (between current ms and reference ms) is >= 250
    previousMillis = currentMillis;												// update reference ms
    
    float temperature = thermocouple.readCelsius();
    float voltage = getVoltage();
    
    switchRelay(temperature);													// Checks if relay needs to be switched off
    logData(currentMillis, temperature, voltage);								// Write to microSD card
    updateLED(voltage);															// Updates the RGB-LED color
  }
}

float getVoltage() {
  int voltageInput = analogRead(VOLTAGE_SENSOR);
  return voltageInput * (5.0 / 1023.0) * 5.0;
}

void switchRelay(float temp) {													// Switches relay if not already switched
  if (!hasSwitched && temp > TEMP_THRESHOLD) {
    digitalWrite(RELAY_PIN, LOW);
    hasSwitched = true;
  }
}

void logData(unsigned long ts, float temp, float voltage, bool hasSwitched) {	
  File file = SD.open("datalog.csv", FILE_WRITE);
  if (file) {
    char timeBuffer[16];
    formatTime(ts, timeBuffer);													// Calls time formatter
    
    file.print(timeBuffer);
    file.print(", temp: ");
    file.print(temp, 2);														// prints temp with 2 decimal places
    file.print(", voltage: ");
    file.print(voltage, 2);														// prints voltage with 2 decimal places
    file.print(hasSwitched ? " Relay OFF" : "");
    file.println();
    file.close();
  }
}

void formatTime(unsigned long ts, char* buffer) {								// Formats the time for SD card
  unsigned long totalSeconds = ts / 1000;
  unsigned long mins = (totalSeconds / 60) % 60;
  unsigned long secs = totalSeconds % 60;
  unsigned long millis = ts % 1000;
  
  sprintf(buffer, "%lu:%02lu.%03lu", mins, secs, millis);
}

void updateLED(float voltage) {													// Updates LED colors relative to battery voltage
  analogWrite(LED_R, 0);
  analogWrite(LED_G, 0);
  analogWrite(LED_B, 0);

  if (voltage <= VOLTAGE_BLUE_MAX) {											// if below 7V (exclusive) -> Blue : Too Low
    analogWrite(LED_B, 255);
  } else if (voltage <= VOLTAGE_GREEN_MAX) {									// if between (7V inclusive and 10V exclusive) -> Green	: Good Range
    analogWrite(LED_G, 255);
  } else if (voltage <= VOLTAGE_ORANGE_MAX) {									// if between (10V inclusive and 12V inlclusive) -> Orange	: A Tad High
    analogWrite(LED_R, 255);
    analogWrite(LED_G, 40);
  } else {
    analogWrite(LED_R, 255);													// if above 12V (exlusive) -> Red : Too High
  }
}
