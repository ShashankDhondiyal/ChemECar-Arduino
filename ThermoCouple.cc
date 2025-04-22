#include <max6675.h>

const int THERMO_DO = 4;
const int THERMO_CS = 5;
const int THERMO_CLK = 6;
const int RELAY_PIN = 2;
const float CRITICAL_TEMP = 50.0;
const unsigned long SAMPLE_INTERVAL = 250;
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO);
unsigned long previousMillis = 0;
bool isNearCritical = false;

void setup() {
    Serial.begin(9600);
    Serial.println("MAX6675 initializing");
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH);
    delay(500);
    Serial.println("__________Ready__________");
    
}

void loop() {
    unsigned long currentMillis = millis();
    
    if (currentMillis - previousMillis >= SAMPLE_INTERVAL) {
        previousMillis = currentMillis;
        
        unsigned long totalSeconds = currentMillis / 1000;
        int minutes = (totalSeconds / 60) % 60;
        int seconds = totalSeconds % 60;
        int milliseconds = currentMillis % 1000;
        
        float temperature = thermocouple.readCelsius();
        
        char timeBuffer[16];
        sprintf(timeBuffer, "%d:%02d.%03d, ", minutes, seconds, milliseconds);
        Serial.print(timeBuffer);
        if (!isNearCritical) {
            isNearCritical = temperature > CRITICAL_TEMP;
        }
        digitalWrite(RELAY_PIN, isNearCritical ? LOW : HIGH);
        if (isNearCritical) {
            Serial.print(F("**___Switched OFF___**"));
        }
        
        Serial.print("Temp: ");
        Serial.println(temperature);
    }
}
