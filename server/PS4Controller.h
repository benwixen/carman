#ifndef SERVER_PS4CONTROLLER_H
#define SERVER_PS4CONTROLLER_H

#include <cinttypes>
#include <optional>
#include <hidapi.h>

struct ControllerState {
    int8_t left_stick_x;
    int8_t left_stick_y;
    bool o_button_pressed;
};

class PS4Controller {
public:
    PS4Controller();
    ~PS4Controller();
    std::optional<ControllerState> readControllerState();

private:
    hid_device_info *hid_list;
    hid_device* handle;
};

#endif //SERVER_PS4CONTROLLER_H
