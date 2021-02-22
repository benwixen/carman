#include <ctype.h>

#include <avr_utils.h>
#include <usart.h>

constexpr uint8_t LEFT_FORWARD_PIN { PD5 };
constexpr uint8_t LEFT_REVERSE_PIN { PD6 };
constexpr uint8_t RIGHT_FORWARD_PIN { PB2 };
constexpr uint8_t RIGHT_REVERSE_PIN { PB1 };

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
        avr::setPwmValue(forward_pin, speed);
        avr::setPwmValue(reverse_pin, 0);
    } else {
        avr::setPwmValue(forward_pin, 0);
        avr::setPwmValue(reverse_pin, speed);
    }
}

char command[] { '5', '5', '\0' };

#pragma ide diagnostic ignored "EndlessLoop"

int main() {
    avr::initUsart(115200);
    avr::printString("Vehicle ready for commands!\n");
    avr::timer0::enable8bitPhaseCorrectPwmMode();
    avr::timer0::enableOutputOnPD5andPD6();
    avr::timer0::setPrescaleBy64();
    avr::timer1::enable8bitPhaseCorrectPwmMode();
    avr::timer1::enableOutputOnPB1andPB2();
    avr::timer1::setPrescaleBy64();

    while (true) {
        avr::readString(command, 2);

        avr::printString("Got command: ");
        avr::printString(command);
        avr::printString("\n");

        drive(LEFT_FORWARD_PIN, LEFT_REVERSE_PIN, command[0], 0);
        drive(RIGHT_FORWARD_PIN, RIGHT_REVERSE_PIN, command[1], 0);
    }
}
