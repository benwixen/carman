#include <iostream>
#include <unistd.h>

#include "BluetoothSerialPort.h"
#include "PS4Controller.h"

inline uint8_t generateCommand(int8_t velocity) {
    uint8_t command = velocity + (-std::numeric_limits<int8_t>::min());
    if (command == 0) {
        return 1;
    } else if (command > 108 && command < 148) {
        return 128;
    } else {
        return command;
    }
}

int main() {
    BluetoothSerialPort bt_port("/dev/cu.carman-DevB", 115200);
    PS4Controller ps4_controller;

    uint8_t prev_command[] { 128, 128 };
    while (true) {
        auto controller_state = ps4_controller.readControllerState();
        if (controller_state.has_value()) {
            if (controller_state->o_button_pressed) {
                break;
            }
            auto left_x = controller_state->left_stick_x;
            auto left_y = controller_state->left_stick_y;
            const uint8_t speed {
                    static_cast<uint8_t>(
                            std::max(std::abs(controller_state->left_stick_x), std::abs(left_y))
                    )
            };
            const int8_t velocity {
                    static_cast<int8_t>(left_y < 0 ? speed : -speed)
            };
            uint8_t points_left_of_origin {
                    static_cast<uint8_t>(std::abs(std::min(left_x, int8_t(0))))
            };
            uint8_t points_right_of_origin {
                    static_cast<uint8_t>(std::max(left_x, int8_t(0)))
            };
            double adjust_factor_left {
                    velocity > 0
                    ? 1 - (points_left_of_origin / 127.0)
                    : 1 - (points_right_of_origin / 127.0)
            };
            double adjust_factor_right {
                    velocity > 0
                    ? 1 - (points_right_of_origin / 127.0)
                    : 1 - (points_left_of_origin / 127.0)
            };
            int8_t left_velocity {
                    static_cast<int8_t>(velocity * adjust_factor_left)
            };
            int8_t right_velocity {
                    static_cast<int8_t>(velocity * adjust_factor_right)
            };

            const auto left_motor_cmd = generateCommand(left_velocity);
            const auto right_motor_cmd = generateCommand(right_velocity);
            const uint8_t command[] { left_motor_cmd, right_motor_cmd, 0 };

            if (command[0] != prev_command[0] || command[1] != prev_command[1]) {
                bt_port.sendData(command, 3);
                memcpy(prev_command, command, 2);
                printf("%i,%i\n", command[0], command[1]);
            }
        }
        usleep(10'000);
    }

    return 0;
}
