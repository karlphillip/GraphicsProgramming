/* Copyright (C) 2013-2015 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#include "serialportreader.h"
#include <iostream>

#include <QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


int main(int argc, char *argv[])
{
    QCoreApplication coreApplication(argc, argv);

    if (!QSerialPortInfo::availablePorts().count())
    {
        std::cout << "!!! No serial ports found!" << std::endl;
        return 0;
    }

    int option = -1;
    while (1)
    {
        std::cout << "===== List of available serial ports (" <<
                     QSerialPortInfo::availablePorts().count() << ") =====" << std::endl;
        std::cout << "[0] - Exit" << std::endl;

        for (int i = 0; i < QSerialPortInfo::availablePorts().count(); i++)
        {
            QSerialPortInfo serial_info = QSerialPortInfo::availablePorts()[i];
            std::cout << "[" << i+1 << "] - " << serial_info.portName().toStdString() << std::endl;

            std::cout << "\tLocation: " << serial_info.systemLocation().toStdString() << std::endl;
            std::cout << "\tDescription: " << serial_info.description().toStdString() << std::endl;
            std::cout << "\tManufacturer: " << serial_info.manufacturer().toStdString() << std::endl;
            std::cout << "\tVendor Identifier: " << (serial_info.hasVendorIdentifier() ?
                                                    QString::number(serial_info.vendorIdentifier()).toStdString() :
                                                    "") << std::endl;
            std::cout << "\tProduct Identifier: " << (serial_info.hasProductIdentifier() ?
                                                     QString::number(serial_info.productIdentifier()).toStdString() :
                                                     "") << std::endl;
            std::cout << "\tBusy: "  << (serial_info.isBusy() ? "Yes" : "No") << std::endl;
        }

        std::cout << "\nYour choice: ";
        std::cin >> option;
        if (option >= 0 && option <= QSerialPortInfo::availablePorts().count())
            break;
        std::cout << std::endl;
    }

    if (option == 0)
    {
        std::cout << "* OK, bye bye!" << std::endl;
        return 0;
    }

    QSerialPortInfo serial_info = QSerialPortInfo::availablePorts()[option-1];
    std::cout << "* OK, " << serial_info.portName().toStdString() <<
                 " selected!" << std::endl;

    QSerialPort serial;
    serial.setPortName(serial_info.portName());
    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setParity(QSerialPort::NoParity);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!serial.open(QIODevice::ReadOnly))
    {
        std::cout << "!!! Failed to open port " << serial_info.portName().toStdString() <<
                     ". Error: " << serial.errorString().toStdString() << std::endl;
        return 1;
    }

    SerialPortReader reader(&serial);
    std::cout << "===== Output =====" << std::endl;

    return coreApplication.exec();
}
