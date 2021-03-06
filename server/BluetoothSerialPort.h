#ifndef SERVER_BLUETOOTHSERIALPORT_H
#define SERVER_BLUETOOTHSERIALPORT_H

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <vector>

class BluetoothSerialPort {
public:
    BluetoothSerialPort(const std::string& device, int baud_rate);
    void sendData(const std::vector<uint8_t>& data);
    ~BluetoothSerialPort();

private:
    boost::asio::io_service io;
    boost::asio::serial_port port;
    std::thread thread;
};


#endif //SERVER_BLUETOOTHSERIALPORT_H
