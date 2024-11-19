#include "security.h"
#include <stdbool.h>
#include "project_config.h"
#include "common_config.h"
#include "def_alarm.h"
#include "def_consts.h"
#include "rTypes.h"
#include "reGpio.h"
#include "reLed.h"
#include "reEvents.h"
#include "reParams.h"
#include "rLog.h"
    #include "reBeep.h"   /* !!! reAlarm.cpp:20:10  */


static const char* logTAG = "ALARM";

// ------------------------------------------------------------------------
//                          Проводные входы 
// ------------------------------------------------------------------------

// Объекты reGPIO для обработки прерываний по проводным входым ОПС
// static reGPIO gpioAlarm1(CONFIG_GPIO_ALARM_ZONE_1, CONFIG_GPIO_ALARM_LEVEL, false, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, nullptr);
// static reGPIO gpioAlarm2(CONFIG_GPIO_ALARM_ZONE_2, CONFIG_GPIO_ALARM_LEVEL, false, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, nullptr);
// static reGPIO gpioAlarm3(CONFIG_GPIO_ALARM_ZONE_3, CONFIG_GPIO_ALARM_LEVEL, false, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, nullptr);
// static reGPIO gpioAlarm4(CONFIG_GPIO_ALARM_ZONE_4, CONFIG_GPIO_ALARM_LEVEL, false, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, nullptr);
// static reGPIO gpioAlarm5(CONFIG_GPIO_ALARM_ZONE_5, CONFIG_GPIO_ALARM_LEVEL, false, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, nullptr);

// ------------------------------------------------------------------------
//                             Инициализация 
// ------------------------------------------------------------------------

void alarmInitDevices()
{
  rlog_i(logTAG, "Initialization of AFS devices");

  // // Создаем светодиоды, сирену и флешер
  // ledQueue_t ledAlarm = nullptr;
  // #if defined(CONFIG_GPIO_ALARM_LED) && (CONFIG_GPIO_ALARM_LED > -1)
  //   ledAlarm = ledTaskCreate(CONFIG_GPIO_ALARM_LED, true, true, "led_alarm", CONFIG_LED_TASK_STACK_SIZE, nullptr);
  //   ledTaskSend(ledAlarm, lmOff, 0, 0, 0);
  // #endif // CONFIG_GPIO_ALARM_LED
  // ledQueue_t siren = nullptr;
  // #if defined(CONFIG_GPIO_ALARM_SIREN) && (CONFIG_GPIO_ALARM_SIREN > -1)
  //   siren = ledTaskCreate(CONFIG_GPIO_ALARM_SIREN, true, false, "siren", CONFIG_LED_TASK_STACK_SIZE, nullptr);
  //   ledTaskSend(siren, lmOff, 0, 0, 0);
  // #endif // CONFIG_GPIO_ALARM_SIREN
  // ledQueue_t flasher = nullptr;
  // #if defined(CONFIG_GPIO_ALARM_FLASH) && (CONFIG_GPIO_ALARM_FLASH > -1)
  //   flasher = ledTaskCreate(CONFIG_GPIO_ALARM_FLASH, true, true, "flasher", CONFIG_LED_TASK_STACK_SIZE, nullptr);
  //   ledTaskSend(flasher, lmBlinkOn, 1, 100, 5000);
  // #endif // CONFIG_GPIO_ALARM_FLASH
  
  // // Замена пассивной пищалки на активную
  // ledQueue_t buzzer = nullptr;
  // #if defined(CONFIG_GPIO_BUZZER_ACTIVE) && (CONFIG_GPIO_BUZZER_ACTIVE > -1)
  //   buzzer = ledTaskCreate(CONFIG_GPIO_BUZZER_ACTIVE, true, false, "buzzer", CONFIG_LED_TASK_STACK_SIZE, nullptr);
  //   ledTaskSend(buzzer, lmOff, 0, 0, 0);
  // #endif // CONFIG_GPIO_ALARM_FLASH
  
  // // Запускаем задачу
  // alarmTaskCreate(siren, flasher, buzzer, ledAlarm, ledAlarm, nullptr);

  // // Запускаем приемник RX 433 MHz
  // #ifdef CONFIG_GPIO_RX433
  //   rx433_Init(CONFIG_GPIO_RX433, alarmTaskQueue());
  //   rx433_Enable();
  // #endif // CONFIG_GPIO_RX433
}




void alarmStart()
{
  //alarmInitDevices();
  //alarmExternalSensorsInit();
  //alarmInitSensors();
}
