#include <SPI.h>
#include <SD.h>
#include "max6675.h"

// ThermoCouple pins
const int THERMO_DO = 4;
const int THERMO_CS = 5;
const int THERMO_CLK = 6;
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO);


// MicroSD card pins
const int SD_CS = 10;                                              
// Additional wiring for micro SD 
// pin 11 -> mosi
// pin 12 -> miso
// pin 13 -> sck

// relay pins and conditions
const int RELAY_PIN = 2;
bool hasSwitched = false;

// ThermoCouple Temprature threshold *Adjustable*
const float TEMP_THRESHOLD = 45.0;
const float TEMP_BOUND = 0.0;


// Time stuff
unsigned long previousMillis = 0;

void setup() {

  SPI.begin();

  // init and set relay on
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  File myFile = SD.open("/datalog.csv", FILE_WRITE);
  myFile.println();
  myFile.print("Using this threshold Value: ");
  myFile.print(TEMP_THRESHOLD);
  myFile.println();
}

void loop() {
  unsigned long currentMillis = millis();
  float temperature = thermocouple.readCelsius();
  if (currentMillis - previousMillis >= 250) {
    previousMillis = currentMillis;

    unsigned long totalSeconds = currentMillis / 1000;
    int minutes = (totalSeconds / 60) % 60;
    int seconds = totalSeconds % 60;
    int milliseconds = currentMillis % 1000;

    char timeBuffer[16];
    sprintf(timeBuffer, "%d:%02d.%03d", minutes, seconds, milliseconds);

    bool isAtCritical = temperature > (TEMP_THRESHOLD - TEMP_BOUND);
    digitalWrite(RELAY_PIN, isAtCritical ? LOW : HIGH);
    String status = isAtCritical ? "OFF" : "ON";

    File myFile = SD.open("/datalog.csv", FILE_WRITE);
    if (myFile) {
      myFile.print(timeBuffer);
      myFile.print(",");
      myFile.print(temperature);
      myFile.print(",");
      myFile.println(status);
      myFile.close();
    }
  
  }
}
