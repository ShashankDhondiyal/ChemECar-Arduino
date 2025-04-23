#include <SPI.h>
#include <SD.h>
#include "max6675.h"

const int THERMO_DO = 4;														    // Data Out ThermoCouple
const int THERMO_CS = 5;														    // Chip Select ThermoCouple
const int THERMO_CLK = 6;;														  // Clock ThermoCouple
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO);							// ThermoCouple Configuration

const float TEMP_THRESHOLD = 50.0;												// ***---> Calibratable Temprature Threshold <---***

const int SD_CS = 8;															// Chip Select MicroSD Card

const int RELAY_PIN = 7;														// Relay
bool hasSwitched = false;														// Relay Switch Condition

const unsigned long SAMPLE_INTERVAL = 250;										// For tracking milliseconds


void setup() {
  Serial.begin(9600);															// init
  pinMode(RELAY_PIN, OUTPUT);													// Define relay signal type
  digitalWrite(RELAY_PIN, HIGH);												// Set signal to high

  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card error!");
    while(1);
  }
}
myFile = SD.open("datalog.csv", FILE_WRITE);

void loop() {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= SAMPLE_INTERVAL) {						// If the delta (between current ms and reference ms) is >= 250
    previousMillis = currentMillis;												// update reference ms
    
    float temperature = thermocouple.readCelsius();
    
    switchRelay(temperature);													// Checks if relay needs to be switched off
    logData(currentMillis, temperature);								// Write to microSD card
  }
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
    file.print(" | ");
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
