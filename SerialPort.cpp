#include "SerialPort.h"

SerialPort::SerialPort(const char* portName) {
    connected = false;

    handler = CreateFileA(portName,
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (handler == INVALID_HANDLE_VALUE) return;

    DCB dcbSerialParameters = {0};
    dcbSerialParameters.DCBlength = sizeof(dcbSerialParameters);

    if (!GetCommState(handler, &dcbSerialParameters)) return;

    dcbSerialParameters.BaudRate = CBR_9600;
    dcbSerialParameters.ByteSize = 8;
    dcbSerialParameters.StopBits = ONESTOPBIT;
    dcbSerialParameters.Parity   = NOPARITY;

    if (!SetCommState(handler, &dcbSerialParameters)) return;

    connected = true;
    PurgeComm(handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

SerialPort::~SerialPort() {
    if (connected) {
        connected = false;
        CloseHandle(handler);
    }
}

int SerialPort::readData(char* buffer, unsigned int nbChar) {
    DWORD bytesRead;
    unsigned int toRead = 0;

    ClearCommError(handler, &errors, &status);

    if (status.cbInQue > 0) {
        if (status.cbInQue > nbChar) toRead = nbChar;
        else toRead = status.cbInQue;
    }

    if (ReadFile(handler, buffer, toRead, &bytesRead, NULL))
        return bytesRead;

    return 0;
}

bool SerialPort::isConnected() {
    return connected;
}
