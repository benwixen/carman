#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Serial.println("We have contact!");
}

void loop() {
    char command[2];
    Serial.readBytes(command, 2);
    Serial.print("Got command: ");
    Serial.println(command);
}