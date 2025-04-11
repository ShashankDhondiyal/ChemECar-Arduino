#include <max6675.h>

const int THERMO_DO = 4;
const int THERMO_CS = 5;
const int THERMO_CLK = 6;
const int RELAY_PIN = 3;
const float CRITICAL_TEMP = 50.0;
const float TEMP_THRESHOLD = 0.5;
const unsigned long SAMPLE_INTERVAL = 250;
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO);
unsigned long previousMillis = 0;

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
        
        float temperature = thermocouple.readCelsius();
        
        String timeString = String(minutes) + ":" + (seconds < 10 ? "0" : "") + String(seconds) + ", ";
        Serial.print(timeString);
        
        bool isNearCritical = abs(temperature - CRITICAL_TEMP) < TEMP_THRESHOLD;
        digitalWrite(RELAY_PIN, isNearCritical ? LOW : HIGH);
        isNearCritical ? Serial.print(F("**___Switched OFF___**")) : 0;
        
        Serial.print("Temp: ");
        Serial.println(temperature);
    }
}
