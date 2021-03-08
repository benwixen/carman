#include "BluetoothSerialPort.h"

#include <iostream>
#include <string>
#include <vector>

void listenToSerial(boost::asio::serial_port& port) {
    static boost::asio::streambuf read_buffer;
    boost::asio::async_read_until(
            port,
            read_buffer,
            '\n',
            [&](boost::system::error_code error, std::size_t bytes_read) {
                if (bytes_read > 0) {
                    if (!error)
                    {
                        std::istream is(&read_buffer);
                        std::string line;
                        std::getline(is, line);
                        std::cout << line << '\n';
                        listenToSerial(port);
                    }
                }
            }
    );
}

BluetoothSerialPort::BluetoothSerialPort(const std::string& device, int baud_rate) : port(io, device) {
    port.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    listenToSerial(port);
    thread = std::thread([&]{ io.run(); });
}

void BluetoothSerialPort::sendData(const std::vector<uint8_t>& data) {
    boost::asio::write(port, boost::asio::buffer(data));
}

BluetoothSerialPort::~BluetoothSerialPort() {
    port.close();
    thread.join();
}
