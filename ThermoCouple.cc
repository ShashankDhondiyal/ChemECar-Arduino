#include <SPI.h>
#include <SD.h>
#include "max6675.h"

// ThermoCouple pins
const int THERMO_DO = 4;
const int THERMO_CS = 5;
const int THERMO_CLK = 6;
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO);


// MicroSD card pins
const int SD_CS = 10;                  // Orange                            
// Additional wiring for micro SD 
// pin 11 -> mosi                      // Green
// pin 12 -> miso                      // Yellow
// pin 13 -> sck                       // blue

// relay pins and conditions
const int RELAY_PIN = 2;
bool hasSwitched = false;

// ThermoCouple Temprature threshold *Adjustable*
const float TEMP_THRESHOLD = 45.0;
const float TEMP_BOUND = 0.0;


// Time stuff
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  if (!SD.begin(10)) {
    Serial.println("Error: 1");
    return;
  }
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
  File myFile = SD.open("/datalog.csv", FILE_WRITE);
  if (myFile) {
    writeToMyFile(myFile, 1);
  } else {
    Serial.println("Error type: 2");
  }
}

void writeToMyFile(File myFile, int type) {
  if (type == 1) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 250) {
      previousMillis = currentMillis;

      // Format time
      unsigned long totalSeconds = currentMillis / 1000;
      int minutes = (totalSeconds / 60) % 60;
      int seconds = totalSeconds % 60;
      int milliseconds = currentMillis % 1000;
      char timeBuffer[16];
      sprintf(timeBuffer, "%d:%02d.%03d", minutes, seconds, milliseconds);

      // Write data to CSV
      myFile.print(timeBuffer);
      myFile.print(",");
      float temperature = thermocouple.readCelsius();
      myFile.print(temperature);
      Serial.println(temperature);
      checkRelayStatus(myFile, temperature);
      myFile.println();
    }
    // Removed else-block to avoid non-CSV content
    myFile.close(); // Close after writing
  }
}

void checkRelayStatus(File myFile, float temperature) {
  bool isAtCritical = temperature > (TEMP_THRESHOLD - TEMP_BOUND);
  digitalWrite(RELAY_PIN, isAtCritical ? LOW : HIGH);
  if (isAtCritical) {
    myFile.print("");
  } else {
    myFile.print(",OFF");
  }
}


