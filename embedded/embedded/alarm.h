#ifndef ALARM_H
#define ALARM_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include "config.h"

#ifdef CFG_ALARM_USE_GPIO
    #include <wiringPi.h>
#endif


enum class AlarmState {
    Off,
    On,
    OnPriorityLow,
    OnPriorityMedium,
    OnPriorityHigh
};

class Alarm : public QObject
{
    Q_OBJECT

private:
    QTimer m_beepTimer;
    QTimer m_changePriorityTimer;
    AlarmState m_state;
    int m_ledState;
    int m_buzzerState;
    int m_lowPriorityTimeMs;
    int m_mediumPriorityTimeMs;
    int m_highPriorityTimeMs;

    static const int BeepTimerMediumPriorityTimeout = 1000;
    static const int BeepTimerHighPriorityTimeout = 500;

    void gpioWrite(int gpio, int state);
    void setState(AlarmState state);

public:
    explicit Alarm(int lowPriorityTimeMs, int mediumPriorityTimeMs, int highPriorityTimeMs, QObject *parent = 0);

    void start();
    void stop();
    bool isRunning();
    void setLowPriorityTimeMs(int timeMs);
    void setMediumPriorityTimeMs(int timeMs);
    void setHighPriorityTimeMs(int timeMs);

private slots:
    void onBeepTimerTimeout();
    void onChangePriorityTimerTimeout();
};

#endif // ALARM_H
