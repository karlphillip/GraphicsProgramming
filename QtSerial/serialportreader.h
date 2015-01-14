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
