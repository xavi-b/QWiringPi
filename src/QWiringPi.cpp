#include "QWiringPi.h"

QWiringPi::QWiringPi(QObject *parent)
    : QThread(parent)
{
    initialize();
}

bool QWiringPi::initialize()
{
    if(geteuid() == 0)
    {
        wiringPiSetup();
        this->initialized = true;
        connect(this, &QThread::finished, this, &QObject::deleteLater);
        start();
    }

    return this->initialized;
}

bool QWiringPi::hasInitialized()
{
    return this->initialized;
}

void QWiringPi::run()
{
    if(!this->initialized)
        return;

    while(true)
    {
        for(auto pin = this->pins.begin(); pin != this->pins.end(); pin++)
        {
            if(pin->first == INPUT)
            {
                int newValue = digitalRead(this->pins.key(*pin));
                if(newValue != pin->second)
                {
                    pin->second = newValue;
                    emit pinValueChanged(this->pins.key(*pin), newValue);
                }
            }
        }
    }
}

QWiringPi* QWiringPi::instance(QObject* parent)
{
    static QWiringPi instance(parent);
    return &instance;
}

bool QWiringPi::setPinMode(Pin pin, Mode mode, bool firstSignal)
{
    if(!this->initialized)
        return false;

    pinMode(pin, mode);

    int value = 0;
    if(mode == INPUT)
    {
        value = digitalRead(pin);
        if(firstSignal)
            emit pinValueChanged(pin, value);
    }

    this->pins.insert(pin, QPair<Mode, int>(mode, value));

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
    if(!this->initialized)
        return false;

    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].first != INPUT)
        return false;

    pullUpDnControl(pin, PUD_OFF);
    return true;
}

bool QWiringPi::setPullUp(Pin pin)
{
    if(!this->initialized)
        return false;

    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].first != INPUT)
        return false;

    pullUpDnControl(pin, PUD_UP);
    return true;
}

bool QWiringPi::setPullDown(Pin pin)
{
    if(!this->initialized)
        return false;

    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].first != INPUT)
        return false;

    pullUpDnControl(pin, PUD_DOWN);
    return true;
}

bool QWiringPi::write(Pin pin, bool value)
{
    if(!this->initialized)
        return false;

    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].first != OUTPUT)
        return false;

    digitalWrite(pin, value);
    return true;
}

bool QWiringPi::write(Pin pin, unsigned short value)
{
    if(!this->initialized)
        return false;

    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].first != PWM_OUTPUT)
        return false;

    if(value > 1024)
        return false;

    pwmWrite(pin, value);
    return true;
}

bool QWiringPi::read(Pin pin, bool& value)
{
    if(!this->initialized)
        return false;

    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].first != INPUT)
        return false;

    value = digitalRead(pin);
    return true;
}
