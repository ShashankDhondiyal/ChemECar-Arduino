#include <max6675.h>
// https://randomnerdtutorials.com/arduino-k-type-thermocouple-max6675/

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
                Serial.println(minutes + ":" + seconds + ", " + thermocouple.readCelsius() + "    ON -> OFF");
            }
            else{
                Serial.println(minutes + ":" + seconds + ", " + thermocouple.readCelsius());
            }
        }
    }
    
}