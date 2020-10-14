#pragma once

#include <QObject>

class BatteryController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int chargeValue READ chargeValue WRITE setChargeValue NOTIFY chargeValueChanged)
    //Q_PROPERTY(int chargeValue READ chargeValue WRITE setChargeValue)

public:
    explicit BatteryController(QObject *parent = nullptr);

protected slots:
    void _tick();

signals:
    void chargeValueChanged();

private:
    int chargeValue();
    void setChargeValue(int value);

    int _charge;
};
