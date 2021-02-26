cmake_minimum_required(VERSION 3.17)
project(server)
set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.40.0 REQUIRED system)

include_directories(SYSTEM /usr/local/include/hidapi)
link_directories(/usr/local/lib)

add_executable(
        server
        main.cpp
        BluetoothSerialPort.h
        BluetoothSerialPort.cpp
        PS4Controller.h
        PS4Controller.cpp
)
target_link_libraries(server hidapi ${Boost_LIBRARIES})
