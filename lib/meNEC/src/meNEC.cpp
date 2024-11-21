#include "meNEC.h"
#include <stdint.h>
#include <stdio.h>
#include "time.h"
#include "esp_err.h"
#include "esp_timer.h"
#include <driver/gpio.h>
#include "rLog.h"
#include "rTypes.h"

static const char* logTAG = "RXNEC";

#define ERR_CHECK(err, str) if (err != ESP_OK) rlog_e(logTAG, "%s: #%d %s", str, err, esp_err_to_name(err));
#define ERR_GPIO_SET_MODE "Failed to set GPIO mode"
#define ERR_GPIO_SET_ISR  "Failed to set ISR handler"

//static gpio_num_t _gpioRx = GPIO_NUM_MAX;


static volatile uint32_t _receivedValue = 0;
static volatile uint16_t _receivedBitlength = 0;
static volatile uint16_t _receivedDelay = 0;
static volatile uint16_t _receivedProtocol = 0;





























































































































































// ------------------------------------------------------------------------
//                              ISR handler 
// ------------------------------------------------------------------------

static gpio_num_t _gpioRx = GPIO_NUM_MAX;

static void IRAM_ATTR rxIsrHandler(void* arg)
{
  static uint64_t usTimePrev = 0;
  static uint64_t usTimeCurr = 0;
  static uint16_t cntChanges = 0;
  static uint16_t cntRepeats = 0;

  // usTimeCurr = esp_timer_get_time();
  // uint16_t usDuration = usTimeCurr - usTimePrev;
  // if (usDuration > nSeparationLimit) {
  //   // A long stretch without signal level change occurred. 
  //   // This could be the gap between two transmission.
  //   if ((cntRepeats == 0) || (diff(usDuration, _timings[0]) < 200)) {
  //     // This long signal is close in length to the long signal which
  //     // started the previously recorded _timings; this suggests that
  //     // it may indeed by a a gap between two transmissions (we assume
  //     // here that a sender will send the signal multiple times,
  //     // with roughly the same gap between them).
  //     cntRepeats++;
  //     if (cntRepeats == 2) {
  //       for(uint8_t i = 1; i <= numProtocols; i++) {
  //         if (rxDetectProtocol(i, cntChanges)) {
  //           // receive succeeded for protocol i, post data to external queue
  //           QueueHandle_t queueProc = (QueueHandle_t)arg;
  //           if (queueProc) {
  //             input_data_t data;
  //             data.source = IDS_RX433;
  //             data.rx433.protocol = i;
  //             data.rx433.value = rx433_GetReceivedValue();
  //             data.count = rx433_GetReceivedBitLength();
  //             // reset recieved value
  //             rx433_ResetAvailable();
  //             // we have not woken a task at the start of the ISR
  //             BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  //             // post data
  //             xQueueSendFromISR(queueProc, &data, &xHigherPriorityTaskWoken);
  //             // now the buffer is empty we can switch context if necessary.
  //             if (xHigherPriorityTaskWoken) {
  //               portYIELD_FROM_ISR();
  //             };
  //           } else {
  //             // reset recieved value
  //             rx433_ResetAvailable();
  //           };
  //           break;
  //         };
  //       };
  //       cntRepeats = 0;
  //     };
  //   };
  //   cntChanges = 0;
  // };

  // // Detect overflow
  // if (cntChanges >= RX433_SWITCH_MAX_CHANGES) {
  //   cntChanges = 0;
  //   cntRepeats = 0;
  // };
  // _timings[cntChanges++] = usDuration;
  // usTimePrev = usTimeCurr;
}

void meNEC_Init(const uint8_t gpioRx, QueueHandle_t queueProc)
{
  _gpioRx = static_cast<gpio_num_t>(gpioRx);
  
  rlog_i(logTAG, "Initialization of NEC receiver on gpio #%d", _gpioRx);

  // ERR_CHECK(gpio_install_isr_service(0), "Failed to install ISR service");

  gpio_reset_pin(_gpioRx);
  ERR_CHECK(gpio_set_direction(_gpioRx, GPIO_MODE_INPUT), ERR_GPIO_SET_MODE);
  ERR_CHECK(gpio_set_pull_mode(_gpioRx, GPIO_FLOATING), ERR_GPIO_SET_MODE);
  ERR_CHECK(gpio_set_intr_type(_gpioRx, GPIO_INTR_ANYEDGE), ERR_GPIO_SET_ISR);
  ERR_CHECK(gpio_isr_handler_add(_gpioRx, rxIsrHandler, queueProc), ERR_GPIO_SET_ISR);
}

void meNEC_Enable()
{
  esp_err_t err = gpio_intr_enable(_gpioRx);
  if (err == ESP_OK) {
    rlog_i(logTAG, "Receiver NEC started");
  } else {
    rlog_e(logTAG, "Failed to start NEC receiver");
  };
}

void meNEC_Disable()
{
  esp_err_t err = gpio_intr_disable(_gpioRx);
  if (err == ESP_OK) {
    rlog_i(logTAG, "Receiver NEC stopped");
  } else {
    rlog_e(logTAG, "Failed to stop NEC receiver");
  };
}

// ------------------------------------------------------------------------
//                          Public functions
// ------------------------------------------------------------------------

bool meNEC_IsAvailable()
{
  return _receivedValue != 0;
}

void meNEC_ResetAvailable()
{
  _receivedValue = 0;
}

uint32_t meNEC_GetReceivedValue()
{
  return _receivedValue;
}

uint16_t meNEC_GetReceivedBitLength()
{
  return _receivedBitlength;
}

uint16_t meNEC_GetReceivedDelay()
{
  return _receivedDelay;
}

uint16_t meNEC_GetReceivedProtocol()
{
  return _receivedProtocol;
}
