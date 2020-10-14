#include "batterycontroller.h"

#include <random>

#include <QDateTime>
#include <QTimer>
#include <QDebug>

// defines how often (milisec) the charge changes
#define TIME_INTERVAL 2000

BatteryController::BatteryController(QObject *parent)
: QObject(parent), _charge(100)
{
    // start generating random values for _charge
    QTimer::singleShot(TIME_INTERVAL, this, SLOT(_tick()));
}

void BatteryController::_tick()
{
    // generate a random number between 0-100 every 5 seconds
    unsigned int ms = static_cast<unsigned>(QDateTime::currentMSecsSinceEpoch());
    std::mt19937 gen(ms);
    std::uniform_int_distribution<> uid(0, 100); // min, max
    _charge = uid(gen);

    qDebug() << "_tick: charge=" << _charge;
    emit chargeValueChanged();

    QTimer::singleShot(TIME_INTERVAL, this, SLOT(_tick()));
}

int BatteryController::chargeValue()
{
    return _charge;
}

void BatteryController::setChargeValue(int value)
{
    _charge = value;
    emit chargeValueChanged();
}
