#include <Arduino.h>

constexpr uint8_t LEFT_FORWARD_PIN { 5 };
constexpr uint8_t LEFT_REVERSE_PIN { 6 };
constexpr uint8_t RIGHT_FORWARD_PIN { 10 };
constexpr uint8_t RIGHT_REVERSE_PIN { 9 };

void setup() {
    Serial.begin(115200);
    Serial.println("Vehicle ready for commands!");
}

inline uint8_t convert_command_to_num(char command) {
    if (isdigit(command)) {
        return command - '0';
    } else {
        return 5;
    }
}

inline uint8_t calculate_speed(uint8_t command, uint8_t bias) {
    switch (command) {
        case 4:
        case 6:
            return 50 + bias;
        case 3:
        case 7:
            return 60 + bias;
        case 2:
        case 8:
            return 70 + bias;
        case 1:
        case 9:
            return 80 + bias;
        case 5:
        default:
            return 0;
    }
}

inline void drive(uint8_t forward_pin, uint8_t reverse_pin, char command, uint8_t bias) {
    const uint8_t num_command = convert_command_to_num(command);
    const uint8_t speed = calculate_speed(num_command, bias);
    if (num_command > 5) {
        analogWrite(forward_pin, speed);
        analogWrite(reverse_pin, 0);
    } else {
        analogWrite(forward_pin, 0);
        analogWrite(reverse_pin, speed);
    }
}

char command[] { '5', '5' };

void loop() {
    Serial.readBytes(command, 2);

    Serial.print("Got command: ");
    Serial.println(command);

    drive(LEFT_FORWARD_PIN, LEFT_REVERSE_PIN, command[0], 20);
    drive(RIGHT_FORWARD_PIN, RIGHT_REVERSE_PIN, command[1], 0);
}