/*
* 2024.11.26 Не закончено
* 
*/


#include "rxir.h"
//#include "reRx433.h"
#include <stdint.h>
//#include <stdio.h>
#include "time.h"
#include "esp_err.h"
#include "esp_timer.h"
#include <driver/gpio.h>
#include "rLog.h"
#include "mTypes.h"

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "ir_tools.h"

#define CONFIG_RMT_RX_CHANNEL 4
#define CONFIG_RMT_RX_GPIO 35

static const char* logTAG = "RXIR}";

#define ERR_CHECK(err, str) if (err != ESP_OK) rlog_e(logTAG, "%s: #%d %s", str, err, esp_err_to_name(err));
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


static void IRAM_ATTR rxIsrHandler(void* arg)   //static void example_ir_rx_task(void *arg)
{
  ESP_LOGD(pcTaskGetName(0), "Start");
  uint32_t addr = 0;
  uint32_t cmd = 0;
  size_t length = 0;
  bool repeat = false;
  RingbufHandle_t rb = NULL;
  rmt_item32_t *items = NULL;

  rmt_channel_t rx_channel = CONFIG_RMT_RX_CHANNEL;
  rmt_config_t rmt_rx_config = RMT_DEFAULT_CONFIG_RX(CONFIG_RMT_RX_GPIO, rx_channel);
  rmt_config(&rmt_rx_config);
  rmt_driver_install(rx_channel, 1000, 0);
  ir_parser_config_t ir_parser_config = IR_PARSER_DEFAULT_CONFIG((ir_dev_t)rx_channel);
  ir_parser_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT; // Using extended IR protocols (both NEC and RC5 have extended version)
  ir_parser_t *ir_parser = NULL;
  ir_parser = ir_parser_rmt_new_nec(&ir_parser_config);

  //get RMT RX ringbuffer
  rmt_get_ringbuf_handle(rx_channel, &rb);
  assert(rb != NULL);
  // Start receive
  rmt_rx_start(rx_channel, true);
  while (1) {
      items = (rmt_item32_t *) xRingbufferReceive(rb, &length, portMAX_DELAY);
      if (items) {
          length /= 4; // one RMT = 4 Bytes
          if (ir_parser->input(ir_parser, items, length) == ESP_OK) {
              if (ir_parser->get_scan_code(ir_parser, &addr, &cmd, &repeat) == ESP_OK) {
                  ESP_LOGI(TAG, "Scan Code %s --- addr: 0x%04"PRIx32" cmd: 0x%04"PRIx32, repeat ? "(repeat)" : "", addr, cmd);
              }
          }
          //after parsing the data, return spaces to ringbuffer.
          vRingbufferReturnItem(rb, (void *) items);
      }
  }
  ir_parser->del(ir_parser);
  rmt_driver_uninstall(rx_channel);
  vTaskDelete(NULL);
}





void rxIR_Init(const uint8_t gpioRx, QueueHandle_t queueProc)
{
  _gpioRx = static_cast<gpio_num_t>(gpioRx);
  
  rlog_i(logTAG, "Initialization of IR receiver on gpio #%d", _gpioRx);

  // ERR_CHECK(gpio_install_isr_service(0), "Failed to install ISR service");

  gpio_reset_pin(_gpioRx);
  ERR_CHECK(gpio_set_direction(_gpioRx, GPIO_MODE_INPUT), ERR_GPIO_SET_MODE);
  ERR_CHECK(gpio_set_pull_mode(_gpioRx, GPIO_FLOATING), ERR_GPIO_SET_MODE);
  ERR_CHECK(gpio_set_intr_type(_gpioRx, GPIO_INTR_ANYEDGE), ERR_GPIO_SET_ISR);
  ERR_CHECK(gpio_isr_handler_add(_gpioRx, rxIsrHandler, queueProc), ERR_GPIO_SET_ISR);
}

void rxIR_Enable()
{
  esp_err_t err = gpio_intr_enable(_gpioRx);
  if (err == ESP_OK) {
    rlog_i(logTAG, "Receiver IR started");
  } else {
    rlog_e(logTAG, "Failed to start IR receiver");
  };
}

void rxIR_Disable()
{
  esp_err_t err = gpio_intr_disable(_gpioRx);
  if (err == ESP_OK) {
    rlog_i(logTAG, "Receiver IR stopped");
  } else {
    rlog_e(logTAG, "Failed to stop IR receiver");
  };
}

// ------------------------------------------------------------------------
//                            Public functions
// ------------------------------------------------------------------------
bool rxIR_IsAvailable()
{
  return _receivedValue != 0;
}

void rxIR_ResetAvailable()
{
  _receivedValue = 0;
}

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
