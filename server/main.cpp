#include <vector>
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

    const auto isOButtonPressed = [](std::optional<ControllerState> state) {
        return state.has_value() && state->o_button_pressed;
    };

    std::vector<uint8_t> prev_command;
    for (auto state = std::optional<ControllerState>();
         !isOButtonPressed(state);
         state = ps4_controller.readControllerState()) {

        if (state.has_value()) {
            auto left_x = state->left_stick_x;
            auto left_y = state->left_stick_y;
            const uint8_t speed {
                    static_cast<uint8_t>(
                            std::max(std::abs(state->left_stick_x), std::abs(left_y))
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
            const std::vector<uint8_t> command { left_motor_cmd, right_motor_cmd, 0 };

            if (command != prev_command) {
                bt_port.sendData(command);
                prev_command = command;
                printf("%i,%i\n", command.at(0), command.at(1));
            }
        }
        usleep(10'000);
    }

    return 0;
}
