/* Copyright (C) 2013-2015 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#pragma once
#include <QByteArray>
#include <QObject>
#include <QtSerialPort/QSerialPort>


class SerialPortReader : public QObject
{
    Q_OBJECT

public:
    SerialPortReader(QSerialPort* serial, QObject* parent = 0);
    ~SerialPortReader();

private slots:
    void _read_callback();
    void _error_callback(QSerialPort::SerialPortError error);

private:
    QSerialPort* _serial;
};
