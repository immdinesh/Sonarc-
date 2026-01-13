#pragma once
#include <windows.h>

class SerialPort {
private:
    HANDLE handler;
    bool connected;
    COMSTAT status;
    DWORD errors;

public:
    SerialPort(const char* portName);
    ~SerialPort();
    int readData(char* buffer, unsigned int nbChar);
    bool isConnected();
};
