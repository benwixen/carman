#ifndef SERVER_BLUETOOTHSERIALPORT_H
#define SERVER_BLUETOOTHSERIALPORT_H

#include <boost/asio.hpp>
#include <iostream>

class BluetoothSerialPort {
public:
    BluetoothSerialPort(const char device[], int baud_rate);
    void sendData(const uint8_t data[], uint8_t size);
    ~BluetoothSerialPort();

private:
    boost::asio::io_service io;
    boost::asio::serial_port port;
    std::thread thread;
};


#endif //SERVER_BLUETOOTHSERIALPORT_H
