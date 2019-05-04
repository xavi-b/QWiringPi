#include "QWiringPi.h"

QWiringPi::QWiringPi(QObject *parent)
    : QThread(parent),
      _isSetUp(false)
{
    if(geteuid() == 0)
    {
        wiringPiSetup();
        _isSetUp = true;
    }
}

void QWiringPi::run()
{
    if(!_isSetUp)
        return;

    while(true)
    {
        for(auto pin = _pins.begin(); pin != _pins.end(); pin++)
        {
            if(pin->first == INPUT)
            {
                int newValue = digitalRead(_pins.key(*pin));
                if(newValue != pin->second)
                {
                    pin->second = newValue;
                    emit pinValueChanged(_pins.key(*pin), newValue);
                }
            }
        }
    }
}

QWiringPi* QWiringPi::Instance(QObject* parent)
{
    static QWiringPi instance(parent);
    return &instance;
}

bool QWiringPi::setPinMode(Pin pin, Mode mode, bool firstSignal)
{
    if(!_isSetUp)
        return false;

    pinMode(pin, mode);

    int value = 0;
    if(mode == INPUT)
    {
        value = digitalRead(pin);
        if(firstSignal)
            emit pinValueChanged(pin, value);
    }

    _pins.insert(pin, QPair<Mode, int>(mode, value));

    return true;
}

bool QWiringPi::setPinInputMode(Pin pin, bool firstSignal)
{
    return setPinMode(pin, INPUT, firstSignal);
}

bool QWiringPi::setPinOutputMode(Pin pin, bool firstSignal)
{
    return setPinMode(pin, OUTPUT, firstSignal);
}

bool QWiringPi::setPinPwmOutputMode(Pin pin, bool firstSignal)
{
    if(pin != 1)
        return false;

    return setPinMode(pin, PWM_OUTPUT, firstSignal);
}

bool QWiringPi::setPinClockOutputMode(Pin pin, bool firstSignal)
{
    if(pin != 7)
        return false;

    return setPinMode(pin, GPIO_CLOCK, firstSignal);
}

bool QWiringPi::setPullOff(Pin pin)
{
    if(!_isSetUp)
        return false;

    if(_pins[pin].first != INPUT)
        return false;

    pullUpDnControl(pin, PUD_OFF);
    return true;
}

bool QWiringPi::setPullUp(Pin pin)
{
    if(!_isSetUp)
        return false;

    if(_pins[pin].first != INPUT)
        return false;

    pullUpDnControl(pin, PUD_UP);
    return true;
}

bool QWiringPi::setPullDown(Pin pin)
{
    if(!_isSetUp)
        return false;

    if(_pins[pin].first != INPUT)
        return false;

    pullUpDnControl(pin, PUD_DOWN);
    return true;
}

bool QWiringPi::write(Pin pin, bool value)
{
    if(!_isSetUp)
        return false;

    if(_pins[pin].first != OUTPUT)
        return false;

    digitalWrite(pin, value);
    return true;
}

bool QWiringPi::write(Pin pin, unsigned short value)
{
    if(!_isSetUp)
        return false;

    if(_pins[pin].first != PWM_OUTPUT)
        return false;

    if(value > 1024)
        return false;

    pwmWrite(pin, value);
    return true;
}

bool QWiringPi::read(Pin pin, bool& value)
{
    if(!_isSetUp)
        return false;

    if(_pins[pin].first != INPUT)
        return false;

    value = digitalRead(pin);
    return true;
}
