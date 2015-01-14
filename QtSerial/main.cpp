#include "serialportreader.h"
#include <iostream>

#include <QCoreApplication>
#include <QtSerialPort/QSerialPort>


int main(int argc, char *argv[])
{
    QCoreApplication coreApplication(argc, argv);

    QSerialPort serial;
    QString port("COM6");
    serial.setPortName(port);
    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setParity(QSerialPort::NoParity);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!serial.open(QIODevice::ReadOnly))
    {
        std::cout << "!!! Failed to open port " << port.toStdString() <<
                     ". Error: " << serial.errorString().toStdString() << std::endl;
        return 1;
    }

    SerialPortReader reader(&serial);

    return coreApplication.exec();
}
