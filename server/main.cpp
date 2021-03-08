#include <iostream>
#include <vector>
#include <unistd.h>

#include "BluetoothSerialPort.h"
#include "PS4Controller.h"

constexpr uint8_t COMMAND_END_BYTE { 0 };
constexpr uint8_t MAX_REVERSE_VELOCITY { 1 };
constexpr uint8_t ZERO_VELOCITY { 128 };
constexpr uint8_t MIN_REVERSE_VELOCITY { ZERO_VELOCITY - 20 };
constexpr uint8_t MIN_FORWARD_VELOCITY { ZERO_VELOCITY + 20 };

inline uint8_t encodeVelocity(int8_t velocity) {
    uint8_t unsigned_velocity = velocity + (-std::numeric_limits<int8_t>::min());
    if (unsigned_velocity == COMMAND_END_BYTE) {
        return MAX_REVERSE_VELOCITY;
    } else if (unsigned_velocity > MIN_REVERSE_VELOCITY && unsigned_velocity < MIN_FORWARD_VELOCITY) {
        return ZERO_VELOCITY;
    } else {
        return unsigned_velocity;
    }
}

inline std::vector<uint8_t> generateCommand(int8_t left_velocity, int8_t right_velocity) {
    return std::vector<uint8_t> {
        encodeVelocity(left_velocity),
        encodeVelocity(right_velocity),
        COMMAND_END_BYTE
    };
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
            const auto command = generateCommand(left_velocity, right_velocity);
            if (command != prev_command) {
                bt_port.sendData(command);
                prev_command = command;
                std::cout << command.at(0) << ',' << command.at(1) << '\n';
            }
        }
        usleep(10'000);
    }

    return 0;
}
