#include "QWiringPi.h"

QWiringPi::QWiringPi(QObject *parent)
    : QObject(parent)
{
    if(geteuid() == 0)
    {
        if(wiringPiSetup() < 0)
            throw std::runtime_error("wiringPiSetup() error");
    }
    else
    {
        throw std::runtime_error("wiringPiSetup() not called with root privileges");
    }
}

QWiringPi* QWiringPi::instance(QObject* parent)
{
    static QWiringPi instance(parent);
    return &instance;
}

bool QWiringPi::setPinMode(Pin pin, Mode mode, bool firstSignal)
{
    pinMode(pin, mode);

    int value = 0;
    if(mode == INPUT)
    {
        value = digitalRead(pin);
        if(firstSignal)
            emit pinValueChanged(pin, value);
    }

    QFile* file = new QFile(QString("/sys/class/gpio/gpio%1/value").arg(pin));
    QSocketNotifier* socketNotifier = new QSocketNotifier(file->handle(), QSocketNotifier::Read);
    connect(socketNotifier, &QSocketNotifier::activated, this, [=]()
    {
        file->readAll();
        emit pinValueChanged(pin, digitalRead(pin));
    });
    PinInfo pinInfo = {mode, file, socketNotifier};
    this->pins.insert(pin, pinInfo);

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
    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].mode != INPUT)
        return false;

    pullUpDnControl(pin, PUD_OFF);
    return true;
}

bool QWiringPi::setPullUp(Pin pin)
{
    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].mode != INPUT)
        return false;

    pullUpDnControl(pin, PUD_UP);
    return true;
}

bool QWiringPi::setPullDown(Pin pin)
{
    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].mode != INPUT)
        return false;

    pullUpDnControl(pin, PUD_DOWN);
    return true;
}

bool QWiringPi::write(Pin pin, bool value)
{

    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].mode != OUTPUT)
        return false;

    digitalWrite(pin, value);
    return true;
}

bool QWiringPi::write(Pin pin, unsigned short value)
{
    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].mode != PWM_OUTPUT)
        return false;

    if(value > 1024)
        return false;

    pwmWrite(pin, value);
    return true;
}

bool QWiringPi::read(Pin pin, bool& value)
{
    if(this->pins.find(pin) == this->pins.end())
        return false;

    if(this->pins[pin].mode != INPUT)
        return false;

    value = digitalRead(pin);
    return true;
}
