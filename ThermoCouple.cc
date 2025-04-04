// https://randomnerdtutorials.com/arduino-k-type-thermocouple-max6675/

#include <max6675.h>
int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;
const int RELAY = 3;
int criticalValue = 0;

#define ON  HIGH
#define OFF LOW

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {

    Serial.begin(9600);
    Serial.println("MAX6675 init");
    delay(500);
    pinMode(RELAY, OUTPUT);

}

void loop() {
    for (int minutes = 0; minutes < 60; minutes++) {
        for (double seconds = 0.00; seconds < 60.00; seconds+= 0.25) {
            if (thermocouple.readCelsius() == criticalValue){
                digitalWrite(RELAY, OFF);
                Serial.print(minutes);
                Serial.print(":");
                Serial.print(seconds);
                Serial.print(", switch ");
                Serial.println(thermocouple.readCelsius());
            }
            else {
                Serial.print(minutes);
                Serial.print(":");
                Serial.print(seconds);
                Serial.print(", ");
                Serial.println(thermocouple.readCelsius());
            }
            delay(250);
        }
    }
}
