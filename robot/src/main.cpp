#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-deprecated-headers"
#pragma ide diagnostic ignored "EndlessLoop"

#include <avr_utils.h>
#include <usart.h>
#include <stdio.h>

constexpr uint8_t LEFT_FORWARD_PIN { PD5 };
constexpr uint8_t LEFT_REVERSE_PIN { PD6 };
constexpr uint8_t RIGHT_FORWARD_PIN { PB2 };
constexpr uint8_t RIGHT_REVERSE_PIN { PB1 };

inline void drive(uint8_t forward_pin, uint8_t reverse_pin, int8_t speed) {
    if (speed >= 0) {
        avr::setPwmValue(forward_pin, speed * 2);
        avr::setPwmValue(reverse_pin, 0);
    } else {
        avr::setPwmValue(forward_pin, 0);
        avr::setPwmValue(reverse_pin, (-speed) * 2);
    }
}

/*
 * speed in range 1-255:
 * 128 is stop.
 * 127-1 is backwards.
 * 129-255 is forwards
 */
uint8_t command[] { 128, 128 };
char message_buffer[80];

int main() {
    avr::usart::init(115200);
    avr::usart::printString("Vehicle ready for commands!\n");
    avr::timer0::enable8bitPhaseCorrectPwmMode();
    avr::timer0::enableOutputOnPD5andPD6();
    avr::timer0::setPrescaleBy64();
    avr::timer1::enable8bitPhaseCorrectPwmMode();
    avr::timer1::enableOutputOnPB1andPB2();
    avr::timer1::setPrescaleBy64();

    while (true) {
        uint8_t bytes_read = avr::usart::readBytes(
                command,
                0,
                2
        );
        if (bytes_read == 2) {
            sprintf(message_buffer, "Got command: %i,%i\n", command[0], command[1]);
            avr::usart::printString(message_buffer);

            int8_t left_speed { static_cast<int8_t>(command[0] + INT8_MIN) };
            int8_t right_speed { static_cast<int8_t>(command[1] + INT8_MIN) };
            sprintf(message_buffer, "Parsed speed: %i,%i\n", left_speed, right_speed);
            avr::usart::printString(message_buffer);
            drive(LEFT_FORWARD_PIN, LEFT_REVERSE_PIN, left_speed);
            drive(RIGHT_FORWARD_PIN, RIGHT_REVERSE_PIN, right_speed);
        }
    }
}

#pragma clang diagnostic pop
