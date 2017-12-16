#ifndef CONFIG_H
#define CONFIG_H

#ifdef __unix
    #define CFG_USE_RASPICAM
    #define CFG_ALARM_USE_GPIO
#endif

#define CFG_ALARM_BUZZER_GPIO 2
#define CFG_ALARM_LED_GPIO 3

#endif // CONFIG_H
