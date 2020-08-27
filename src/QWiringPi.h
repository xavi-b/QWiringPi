#ifndef QWIRINGPI_H
#define QWIRINGPI_H

#include <wiringPi.h>
#include <unistd.h>

#include <QObject>
#include <QMap>
#include <QSocketNotifier>
#include <QFile>

class QWiringPi : public QObject
{
    Q_OBJECT
    typedef int Pin;
    typedef int Mode;

private:
    struct PinInfo
    {
        ~PinInfo()
        {
            if(file)
                file->deleteLater();

            if(socketNotifier)
                socketNotifier->deleteLater();
        }

        Mode mode;
        QFile* file = nullptr;
        QSocketNotifier* socketNotifier = nullptr;
    };

    QMap<Pin, PinInfo> pins;

    QWiringPi(QObject* parent = nullptr);

public:
    static QWiringPi* instance(QObject* parent = nullptr);

    bool setPinMode(Pin pin, Mode mode, bool firstSignal = false);
    bool setPinInputMode(Pin pin, bool firstSignal = false);
    bool setPinOutputMode(Pin pin, bool firstSignal = false);
    bool setPinPwmOutputMode(Pin pin, bool firstSignal = false);
    bool setPinClockOutputMode(Pin pin, bool firstSignal = false);
    bool setPullOff(Pin pin);
    bool setPullUp(Pin pin);
    bool setPullDown(Pin pin);
    bool write(Pin pin, bool value);
    bool write(Pin pin, unsigned short value);
    bool read(Pin pin, bool& value);

signals:
    void pinValueChanged(Pin, int);

};

#endif // QWIRINGPI_H
