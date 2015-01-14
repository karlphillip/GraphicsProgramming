#include "serialportreader.h"
#include <iostream>

#include <QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


int main(int argc, char *argv[])
{
    QCoreApplication coreApplication(argc, argv);

    std::cout << "* Number of serial ports available: " << QSerialPortInfo::availablePorts().count() << std::endl;

    QString port("COM6");
    std::cout << "* Trying to find " << port.toStdString() << " ..." << std::endl;


    for (int i = 0; i < QSerialPortInfo::availablePorts().count(); i++)
    {
        QSerialPortInfo serial_info = QSerialPortInfo::availablePorts()[i];
        if (port == serial_info.portName())
        {
            std::cout << "* OK, found it: " << serial_info.portName().toStdString() << std::endl;
            std::cout << "* Location: " << serial_info.systemLocation().toStdString() << std::endl;
            std::cout << "* Description: " << serial_info.description().toStdString() << std::endl;
            std::cout << "* Manufacturer: " << serial_info.manufacturer().toStdString() << std::endl;
            std::cout << "* Vendor Identifier: " << (serial_info.hasVendorIdentifier() ?
                                                    QString::number(serial_info.vendorIdentifier()).toStdString() :
                                                    "") << std::endl;
            std::cout << "* Product Identifier: " << (serial_info.hasProductIdentifier() ?
                                                     QString::number(serial_info.productIdentifier()).toStdString() :
                                                     "") << std::endl;
            std::cout << "* Busy: "  << (serial_info.isBusy() ? "Yes" : "No") << std::endl;
        }
    }

    QSerialPort serial;
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
    std::cout << "* Output:" << std::endl;

    return coreApplication.exec();
}
