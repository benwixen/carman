#include "PS4Controller.h"

#include <stdexcept>

PS4Controller::PS4Controller() {
    if (hid_init() != 0) {
        throw std::runtime_error("hid_init() failed.");
    }

    PS4Controller::hid_list = hid_enumerate(0, 0);
    hid_device_info *device = hid_list;
    hid_device_info *ps4_controller = nullptr;
    while (device) {
        if (device->product_id == 2508 && device->vendor_id == 1356) {
            ps4_controller = device;
            break;
        }
        device = device->next;
    }

    if (ps4_controller == nullptr) {
        throw std::runtime_error("No controller found.");
    }

    PS4Controller::handle = hid_open(
            ps4_controller->vendor_id,
            ps4_controller->product_id,
            ps4_controller->serial_number
    );
}

PS4Controller::~PS4Controller() {
    hid_free_enumeration(hid_list);
    hid_exit();
}

std::optional<ControllerState> PS4Controller::readControllerState() {
    uint8_t controller_data[6];
    int num_bytes_read = hid_read(handle, controller_data, 6);
    if (num_bytes_read == 6) {
        return ControllerState {
            .left_stick_x = static_cast<int8_t>(controller_data[1] - 127),
            .left_stick_y = static_cast<int8_t>(controller_data[2] - 127),
            .o_button_pressed = controller_data[5] == 72
        };
    } else {
        return std::nullopt;
    }
}
