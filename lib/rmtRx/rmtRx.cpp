/*
* 2024.12.20 Не закончено
* 
*/


#include "rmtRx.h"
//#include "reRx433.h"
#include <stdint.h>
#include <stdio.h>
#include "time.h"
#include "esp_err.h"
#include "esp_timer.h"
//#include <driver/gpio.h>
#include "rLog.h"     // https://kotyara12.ru/iot/esp_log/  
#include "mTypes.h"

// #include <stdio.h>
 #include <inttypes.h>
 #include <string.h>
 #include "sdkconfig.h"
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
// #include "esp_log.h" 
                        /* https://kotyara12.ru/iot/esp_log/
                        ESP_LOGE – ошибка (минимальный уровень детализации)
                        ESP_LOGW – предупреждение
                        ESP_LOGI – информация
                        ESP_LOGD – отладочные сообщения
                        ESP_LOGV – всё подряд (максимальный уровень детализации) 
                        */
 #include "driver/rmt.h"
 //#include "driver/rmt_rx.h"
 #include "ir_tools.h"

#define CONFIG_RMT_RX_CHANNEL (rmt_channel_t) 4
#define CONFIG_RMT_RX_GPIO (gpio_num_t) CONFIG_GPIO_RX_IR   //35

#define CONFIG_IR_PROTOCOL_NEC 1    // moro
//
//static const char* logTAG = "RXIR";  // static const char* TAG = “MyModule“;
static const char* TAG = "PULT";

#define ERR_CHECK(err, str) if (err != ESP_OK) rlog_e(TAG, "%s: #%d %s", str, err, esp_err_to_name(err));
#define ERR_GPIO_SET_MODE "Failed to set GPIO mode"
#define ERR_GPIO_SET_ISR  "Failed to set ISR handler"

static volatile uint32_t _receivedValue = 0;
static volatile uint16_t _receivedBitlength = 0;
static volatile uint16_t _receivedDelay = 0;
static volatile uint16_t _receivedProtocol = 0;

// ------------------------------------------------------------------------
//            ISR handler (Integrated Services Router handler?)
// ------------------------------------------------------------------------

static gpio_num_t _gpioRx = GPIO_NUM_MAX;

// static void IRAM_ATTR rxIsrHandler(void* arg)
static void ir_rx_task(void *arg)
{
  rlog_d(pcTaskGetName(0), "Start");
  uint32_t addr = 0;
  uint32_t cmd = 0;
  size_t length = 0;

  // Объявление переменной для хранения булевского значения, повторяется ли пакет
  bool repeat = false;

  // Указатель на кольцевой буфер
  RingbufHandle_t rb = NULL;

  rmt_item32_t *items = NULL;

  rmt_channel_t rx_channel = CONFIG_RMT_RX_CHANNEL;

  // Инициализация конфигурационных параметров RMT по умолчанию для порта и канала
  rmt_config_t rmt_rx_config = RMT_DEFAULT_CONFIG_RX(CONFIG_RMT_RX_GPIO, rx_channel);
  rmt_config(&rmt_rx_config);

    /* Назначить размер буфера, ноль в третьем параметре гласит о том, что флаги 
   прерываний используются по умолчанию. */
  rmt_driver_install(rx_channel, 1000, 0);

    /* По умолчанию инициализируется парсер для входящих пакетов */
  ir_parser_config_t ir_parser_config = IR_PARSER_DEFAULT_CONFIG((ir_dev_t)rx_channel);

    /* Флаг использования расширенных ИК-протоколов для NEC и RC5 */
  ir_parser_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT;

    /* Указатель для ИК-парсера */
  ir_parser_t *ir_parser = NULL;

    /* Инициализируется парсер в зависимости от используемого протокола */
  #if CONFIG_IR_PROTOCOL_NEC
    ir_parser = ir_parser_rmt_new_nec(&ir_parser_config);
  #elif CONFIG_IR_PROTOCOL_RC5
    ir_parser = ir_parser_rmt_new_rc5(&ir_parser_config);
  #endif

    /* get RMT RX ringbuffer */
  rmt_get_ringbuf_handle(rx_channel, &rb);

    /* Проверка буфера на ошибки */
  assert(rb != NULL);

    /* Запуск модуля */
  rmt_rx_start(rx_channel, true);

    /* В бесконечном цикле начнём принимать наши пакеты. */
  while (1)
  {
      /* Попытка принять пакет */
    items = (rmt_item32_t *) xRingbufferReceive(rb, &length, portMAX_DELAY);

    /* Если пакет непустой, то изменим уровень ножки светодиода */
    if (items) 
    {
      // Вставить индикацию или ...

        /* Получить остаток от деления на 4 значения длины пакета */
      length /= 4; // one RMT = 4 Bytes

        /* Точка входа в пакет */
      if (ir_parser->input(ir_parser, items, length) == ESP_OK) 
      {
          /* Скан-код после декодирования */
        if (ir_parser->get_scan_code(ir_parser, &addr, &cmd, &repeat) == ESP_OK) 
        {
            /* Вывод в терминал адреса и команды */
          rlog_i(TAG, "Scan Code %s --- addr: 0x%04" PRIx32" cmd: 0x%04" PRIx32, repeat ? "(repeat)" : "", addr, cmd);




          //           #if CONFIG_IR_PROTOCOL_NEC
          //           if(((uint8_t)addr == (uint8_t)~(addr>>8)) && ((uint8_t)cmd == (uint8_t)~(cmd>>8)))
          //           {
          // // ============ moro
          //             _receivedValue = cmd;
          //             _receivedBitlength = length;
          //             // static volatile uint16_t _receivedDelay = 0;
          // // ============ 
          //             rlog_i(TAG, "Scan Code %s --- addr: 0x%02x, cmd: 0x%02x", repeat ? "(repeat)" : "",
          //                   (uint8_t)addr, (uint8_t)cmd);
          //           }
          //           else
          //           {
          //             rlog_e(TAG, "Scan Code XOR Error!!!");
          //             rlog_i(TAG, "Scan Code %s --- addr: 0x%04x cmd: 0x%04x", repeat ? "(repeat)" : "", addr, cmd);
          //           }
          //           #elif CONFIG_IR_PROTOCOL_RC5
          //             rlog_i(TAG, "Scan Code %s --- addr: 0x%04x cmd: 0x%04x", repeat ? "(repeat)" : "", addr, cmd);
          //           #endif
        }
      }
        /* Выйдем из двух условий и после синтаксического анализа данных возвратим пробелы
        в кольцевой буфер */
        //after parsing the data, return spaces to ringbuffer.
      vRingbufferReturnItem(rb, (void *) items);
    }
  }
  /* Если произойдёт выход из бесконечного цикла (что вряд ли произойдёт, так как мы нигде
   не использовали выход из него), удаляется парсер, производится деинициализацию канала RMT
   и уничтожение задачи. */
  ir_parser->del(ir_parser);
  rmt_driver_uninstall(rx_channel);
  vTaskDelete(NULL);
}




// // Инициализация порта ИК приёмника
// void rxIR_Init(const uint8_t gpioRx, QueueHandle_t queueProc)
// {
//   _gpioRx = static_cast<gpio_num_t>(gpioRx);
  
//   rlog_i(TAG, "Initialization of IR receiver on gpio #%d", _gpioRx);

//   // ERR_CHECK(gpio_install_isr_service(0), "Failed to install ISR service");

//   gpio_reset_pin(_gpioRx);
//   ERR_CHECK(gpio_set_direction(_gpioRx, GPIO_MODE_INPUT), ERR_GPIO_SET_MODE);
//   ERR_CHECK(gpio_set_pull_mode(_gpioRx, GPIO_FLOATING), ERR_GPIO_SET_MODE);
//   ERR_CHECK(gpio_set_intr_type(_gpioRx, GPIO_INTR_ANYEDGE), ERR_GPIO_SET_ISR);
//   //ERR_CHECK(gpio_isr_handler_add(_gpioRx, rxIsrHandler, queueProc), ERR_GPIO_SET_ISR);
//   ERR_CHECK(gpio_isr_handler_add(_gpioRx, rxIsrHandler, queueProc), ERR_GPIO_SET_ISR);
// }

void irTaskStart()
{
  // led ...

  xTaskCreate(ir_rx_task, "ir_rx_task", 2048, NULL, 10, NULL);
}




// // Разрешение прерываний порта ИК приёмника
// void rxIR_Enable()
// {
//   esp_err_t err = gpio_intr_enable(_gpioRx);
//   if (err == ESP_OK) {
//     rlog_i(TAG, "Receiver IR started");
//   } else {
//     rlog_e(TAG, "Failed to start IR receiver");
//   };
// }

// void rxIR_Disable()
// {
//   esp_err_t err = gpio_intr_disable(_gpioRx);
//   if (err == ESP_OK) {
//     rlog_i(logTAG, "Receiver IR stopped");
//   } else {
//     rlog_e(logTAG, "Failed to stop IR receiver");
//   };
// }

// ------------------------------------------------------------------------
//                            Public functions
// ------------------------------------------------------------------------
// bool rxIR_IsAvailable()
// {
//   return _receivedValue != 0;
// }

// void rxIR_ResetAvailable()
// {
//   _receivedValue = 0;
// }

uint32_t rxIR_GetReceivedValue()
{
  return _receivedValue;
}

uint16_t rxIR_GetReceivedBitLength()
{
  return _receivedBitlength;
}

uint16_t rxIR_GetReceivedDelay()
{
  return _receivedDelay;
}

uint16_t rxIR_GetReceivedProtocol()
{
  return _receivedProtocol;
}
