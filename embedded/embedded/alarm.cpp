#include "alarm.h"

Alarm::Alarm(int lowPriorityTimeMs, int mediumPriorityTimeMs, int highPriorityTimeMs, QObject *parent) :
    QObject(parent),
    m_ledState(false),
    m_buzzerState(false),
    m_lowPriorityTimeMs(lowPriorityTimeMs),
    m_mediumPriorityTimeMs(mediumPriorityTimeMs),
    m_highPriorityTimeMs(highPriorityTimeMs),
    m_state(AlarmState::Off)
{
#ifdef CFG_ALARM_USE_GPIO
    wiringPiSetupGpio();
    pinMode(CFG_ALARM_LED_GPIO, OUTPUT);
    pinMode(CFG_ALARM_BUZZER_GPIO, OUTPUT);
#endif

    connect(&m_beepTimer, &QTimer::timeout,
            this, &Alarm::onBeepTimerTimeout);

    m_changePriorityTimer.setSingleShot(true);
    connect(&m_changePriorityTimer, &QTimer::timeout,
            this, &Alarm::onChangePriorityTimerTimeout);

    gpioWrite(CFG_ALARM_LED_GPIO, false);
    gpioWrite(CFG_ALARM_BUZZER_GPIO, false);
}

void Alarm::gpioWrite(int gpio, int state)
{
#ifdef CFG_ALARM_USE_GPIO
    digitalWrite(gpio, state);
#else
    //qDebug() << "gpio " << gpio << " = " << state;
#endif
}

void Alarm::setLowPriorityTimeMs(int timeMs)
{
    m_lowPriorityTimeMs = timeMs;
}

void Alarm::setMediumPriorityTimeMs(int timeMs)
{
    m_mediumPriorityTimeMs = timeMs;
}

void Alarm::setHighPriorityTimeMs(int timeMs)
{
    m_highPriorityTimeMs = timeMs;
}

void Alarm::setState(AlarmState state)
{
    qDebug() << "Alarm state" << int(state);
    m_state = state;
    m_ledState = false;
    m_buzzerState = false;
    gpioWrite(CFG_ALARM_LED_GPIO, false);
    gpioWrite(CFG_ALARM_BUZZER_GPIO, false);

    switch (m_state)
    {
    case AlarmState::OnPriorityLow:
        gpioWrite(CFG_ALARM_LED_GPIO, true);
        m_beepTimer.stop();
        break;
    case AlarmState::OnPriorityMedium:
        m_beepTimer.start(BeepTimerMediumPriorityTimeout);
        onBeepTimerTimeout();
        break;
    case AlarmState::OnPriorityHigh:
        m_beepTimer.start(BeepTimerHighPriorityTimeout);
        onBeepTimerTimeout();
        break;
    default:
        m_beepTimer.stop();
        break;
    }
}

void Alarm::start()
{
    setState(AlarmState::On);
    m_changePriorityTimer.start(m_lowPriorityTimeMs);
}

void Alarm::stop()
{
    m_changePriorityTimer.stop();
    setState(AlarmState::Off);
}

bool Alarm::isRunning()
{
    return m_state != AlarmState::Off;
}

void Alarm::onBeepTimerTimeout()
{
    switch (m_state)
    {
    case AlarmState::OnPriorityLow:
        m_ledState = true;
        m_buzzerState = false;
        gpioWrite(CFG_ALARM_LED_GPIO, true);
        gpioWrite(CFG_ALARM_BUZZER_GPIO, false);
        break;
    case AlarmState::OnPriorityMedium:
        m_ledState = !m_ledState;
        m_buzzerState = false;
        gpioWrite(CFG_ALARM_LED_GPIO, m_ledState);
        gpioWrite(CFG_ALARM_BUZZER_GPIO, false);
        break;
    case AlarmState::OnPriorityHigh:
        m_ledState = !m_ledState;
        m_buzzerState = !m_buzzerState;
        gpioWrite(CFG_ALARM_LED_GPIO, m_ledState);
        gpioWrite(CFG_ALARM_BUZZER_GPIO, m_buzzerState);
        break;
    default:
        m_ledState = false;
        m_buzzerState = false;
        gpioWrite(CFG_ALARM_LED_GPIO, false);
        gpioWrite(CFG_ALARM_BUZZER_GPIO, false);
        break;
    }
}

void Alarm::onChangePriorityTimerTimeout()
{
    switch (m_state)
    {
    case AlarmState::On:
        setState(AlarmState::OnPriorityLow);
        m_changePriorityTimer.start(m_mediumPriorityTimeMs);
        break;
    case AlarmState::OnPriorityLow:
        setState(AlarmState::OnPriorityMedium);
        m_changePriorityTimer.start(m_highPriorityTimeMs);
        break;
    case AlarmState::OnPriorityMedium:
        setState(AlarmState::OnPriorityHigh);
        break;
    }
}
