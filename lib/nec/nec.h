/* 
  Модуль для приема данных с ИК пульта по протоколу NEC.  
*/

#ifndef _NEC_H_
#define _NEC_H_

// #include <stdbool.h>
// #include "reLed.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/queue.h"

/** 
 * Number of maximum high/Low changes per packet.
 * We can handle up to (unsigned long) => 32 bit * 2 H/L changes per bit + 2 for sync
 */
 #define MENEC_SWITCH_MIN_CHANGES 7
 #define MENEC_SWITCH_MAX_CHANGES 67 

#ifdef __cplusplus
extern "C" {
#endif


void necTaskStart();


// void meNEC_Init(const uint8_t gpioRx, QueueHandle_t queueProc);
// void meNEC_Enable();
// void meNEC_Disable();

// bool meNEC_IsAvailable();
// void meNEC_ResetAvailable();
// uint32_t meNEC_GetReceivedValue();
// uint16_t meNEC_GetReceivedBitLength();
// uint16_t meNEC_GetReceivedDelay();
// uint16_t meNEC_GetReceivedProtocol();

#ifdef __cplusplus
}
#endif

#endif // _NEC_H_
