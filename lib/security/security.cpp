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
  //  #include "reBeep.h"   /* !!! reAlarm.cpp:20:10  */


static const char* logTAG = "ALARM";

// ------------------------------------------------------------------------
//                          Проводные входы 
// ------------------------------------------------------------------------

// Объекты reGPIO для обработки прерываний по проводным входым ОПС
static reGPIO gpioAlarm1(CONFIG_GPIO_ALARM_ZONE_1, CONFIG_GPIO_ALARM_LEVEL, false, true, CONFIG_BUTTON_DEBOUNCE_TIME_US, nullptr);
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

  // Создаем светодиоды, сирену и флешер
  ledQueue_t ledAlarm = nullptr;
  #if defined(CONFIG_GPIO_ALARM_LED) && (CONFIG_GPIO_ALARM_LED > -1)
    ledAlarm = ledTaskCreate(CONFIG_GPIO_ALARM_LED, true, true, "led_alarm", CONFIG_LED_TASK_STACK_SIZE, nullptr);
    ledTaskSend(ledAlarm, lmOff, 0, 0, 0);
  #endif // CONFIG_GPIO_ALARM_LED

  ledQueue_t siren = nullptr;
  #if defined(CONFIG_GPIO_ALARM_SIREN) && (CONFIG_GPIO_ALARM_SIREN > -1)
    siren = ledTaskCreate(CONFIG_GPIO_ALARM_SIREN, true, false, "siren", CONFIG_LED_TASK_STACK_SIZE, nullptr);
    ledTaskSend(siren, lmOff, 0, 0, 0);
  #endif // CONFIG_GPIO_ALARM_SIREN

  ledQueue_t flasher = nullptr;
  #if defined(CONFIG_GPIO_ALARM_FLASH) && (CONFIG_GPIO_ALARM_FLASH > -1)
    flasher = ledTaskCreate(CONFIG_GPIO_ALARM_FLASH, true, true, "flasher", CONFIG_LED_TASK_STACK_SIZE, nullptr);
    ledTaskSend(flasher, lmBlinkOn, 1, 100, 5000);
  #endif // CONFIG_GPIO_ALARM_FLASH
  
  // Замена пассивной пищалки на активную
  ledQueue_t buzzer = nullptr;
  #if defined(CONFIG_GPIO_BUZZER_ACTIVE) && (CONFIG_GPIO_BUZZER_ACTIVE > -1)
    buzzer = ledTaskCreate(CONFIG_GPIO_BUZZER_ACTIVE, true, false, "buzzer", CONFIG_LED_TASK_STACK_SIZE, nullptr);
    ledTaskSend(buzzer, lmOff, 0, 0, 0);
  #endif // CONFIG_GPIO_ALARM_FLASH
  
  // Запускаем задачу
  alarmTaskCreate(siren, flasher, buzzer, ledAlarm, ledAlarm, nullptr);

  // Запускаем приемник RX 433 MHz
  #if defined (CONFIG_GPIO_RX433) && (CONFIG_GPIO_RX433 > -1)
    rx433_Init(CONFIG_GPIO_RX433, alarmTaskQueue());
    rx433_Enable();
  #endif // CONFIG_GPIO_RX433
}

void alarmInitSensors()
{
  rlog_i(logTAG, "Initialization of AFS zones");
  // ------------------------------------------------------------------------
  // Настраиваем зоны охраны
  // ------------------------------------------------------------------------

  // Двери (периметр) :: создаем зону охраны
  alarmZoneHandle_t azDoors = alarmZoneAdd(
    "Двери",           // Понятное название зоны
    "doors",           // MQTT-топик зоны
    nullptr            // Функция управления реле, при необходимости
  );
  // Настраиваем реакции для данной зоны в разных режимах
  alarmResponsesSet(
    azDoors,           // Ссылка на зону охраны
    ASM_DISABLED,      // Настраиваем реакции для режима ASM_DISABLED - "охрана отключена"
    ASRS_REGISTER,     // Реакция на события тревоги: только регистрация (фактически это приводит к публикации его на MQTT)
    ASRS_REGISTER      // Реакция на отмену тревоги: только регистрация (фактически это приводит к публикации его на MQTT)
  );
  alarmResponsesSet(
    azDoors,           // Ссылка на зону охраны
    ASM_ARMED,         // Настраиваем реакции для режима ASM_ARMED - "полная охрана"
    ASRS_ALARM_SIREN,  // Реакция на события тревоги: включить сирену и отправить уведомления
    ASRS_REGISTER      // Реакция на отмену тревоги: только регистрация (фактически это приводит к публикации его на MQTT)
  );
  alarmResponsesSet(
    azDoors,           // Ссылка на зону охраны
    ASM_PERIMETER,     // Настраиваем реакции для режима ASM_PERIMETER - "только периметр (дома)" 
    ASRS_ALARM_SIREN,  // Реакция на события тревоги: включить сирену и отправить уведомления
    ASRS_REGISTER      // Реакция на отмену тревоги: только регистрация (фактически это приводит к публикации его на MQTT)
  );
  alarmResponsesSet(
    azDoors,           // Ссылка на зону охраны
    ASM_OUTBUILDINGS,  // Настраиваем реакции для режима ASM_OUTBUILDINGS - "внешние помещения" 
    ASRS_ALARM_NOTIFY, // Реакция на события тревоги: тихая тревога - отправить уведомления, но сирену не включать
    ASRS_REGISTER      // Реакция на отмену тревоги: только регистрация (фактически это приводит к публикации его на MQTT)
  );

  // Датчики дыма и пламени - тревога 24*7
  alarmZoneHandle_t azFire = alarmZoneAdd(
    "Пожар", 
    "fire", 
    nullptr);
  alarmResponsesSet(
    azFire, 
    ASM_DISABLED, 
    ASRS_ALARM_SILENT, 
    ASRS_ALARM_NOTIFY);
  alarmResponsesSet(
    azFire, 
    ASM_ARMED, 
    ASRS_ALARM_SIREN, 
    ASRS_ALARM_NOTIFY);
  alarmResponsesSet(
    azFire, 
    ASM_PERIMETER, 
    ASRS_ALARM_SIREN, 
    ASRS_ALARM_NOTIFY);
  alarmResponsesSet(
    azFire, 
    ASM_OUTBUILDINGS, 
    ASRS_ALARM_SIREN, 
    ASRS_ALARM_NOTIFY);


  // Контроль сетевого напряжения на X6
  #if defined(CONFIG_GPIO_ALARM_ZONE_3) && (CONFIG_GPIO_ALARM_ZONE_3 > -1)
    alarmZoneHandle_t azPower1 = alarmZoneAdd(
      "Контроль питания", 
      "power1", 
      nullptr);
    alarmResponsesSet(
      azPower1, 
      ASM_DISABLED, 
      ASRS_REGISTER, 
      ASRS_REGISTER);
    alarmResponsesSet(
      azPower1, 
      ASM_ARMED, 
      ASRS_REGISTER, 
      ASRS_REGISTER);
    alarmResponsesSet(
      azPower1, 
      ASM_PERIMETER, 
      ASRS_REGISTER, 
      ASRS_REGISTER);
    alarmResponsesSet(
      azPower1, 
      ASM_OUTBUILDINGS, 
      ASRS_REGISTER, 
      ASRS_REGISTER);
  #endif  // CONFIG_GPIO_ALARM_ZONE_3

  // Контроль сетевого напряжения на X7
  #if defined(CONFIG_GPIO_ALARM_ZONE_4) && (CONFIG_GPIO_ALARM_ZONE_4 > -1)
    alarmZoneHandle_t azPower2 = alarmZoneAdd(
      "Контроль питания", 
      "power2", 
      nullptr);
    alarmResponsesSet(
      azPower2, 
      ASM_DISABLED, 
      ASRS_REGISTER, 
      ASRS_REGISTER);
    alarmResponsesSet(
      azPower2, 
      ASM_ARMED, 
      ASRS_REGISTER, 
      ASRS_REGISTER);
    alarmResponsesSet(
      azPower2, 
      ASM_PERIMETER, 
      ASRS_REGISTER, 
      ASRS_REGISTER);
    alarmResponsesSet(
      azPower2, 
      ASM_OUTBUILDINGS, 
      ASRS_REGISTER, 
      ASRS_REGISTER);
  #endif // CONFIG_GPIO_ALARM_ZONE_4

    // Контроль ИК
    // ...

  rlog_i(logTAG, "Initialization of AFS sensors");

  // ------------------------------------------------------------------------
  // Проводные входы для встроенных GPIO
  // ------------------------------------------------------------------------
  // Проводная зона 1: входная дверь
  gpioAlarm1.initGPIO();
  alarmSensorHandle_t asWired1 = alarmSensorAdd(
    AST_WIRED,                                      // Тип датчика: проводные датчики
    "Входная дверь",                                // Понятное имя датчика
    "door",                                         // Топик датчика
    CONFIG_ALARM_LOCAL_PUBLISH,                     // Использовать локальные топики для передачи сигналов на другие устройства, в примере = TRUE (0x01)
    CONFIG_GPIO_ALARM_ZONE_1                        // Номер вывода или адрес датчика
  );
  if (asWired1) {
    alarmEventSet(asWired1, azDoors, 0, ASE_ALARM, 
      1, CONFIG_ALARM_EVENT_MESSAGE_DOOROPEN,       // Сообщение при сигнале тревоги: "🚨 Открыта дверь"
      0, NULL,                                      // Сообщение при отмене тревоги: отсутствует
      1,                                            // Порог срабатывания (нужен только для беспроводных датчиков, для остальных = 1)
      0,                                            // Время автосброса тревоги по таймеру, 0 = отключено
      60,                                           // Период публикации на MQTT брокере
      false);                                       // Тревога без подтверждения с других датчиков
  };









  rlog_i(logTAG, "Initialization of AFS completed");

}

// ------------------------------------------------------------------------
//                             Внешние датчики 
// ------------------------------------------------------------------------

  










void alarmStart()
{
  alarmInitDevices();
  //alarmExternalSensorsInit();
  alarmInitSensors();
}
