/* 
  Модуль для приема данных с ИК пульта по протоколу NEC.  
*/

#ifndef _RMTRX_H_
#define _RMTRX_H_

#include <stdbool.h>
#include "reLed.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#ifdef __cplusplus
extern "C" {
#endif

void irTaskStart();


uint32_t rxIR_GetReceivedValue();
uint16_t rxIR_GetReceivedBitLength();
uint16_t rxIR_GetReceivedDelay();
uint16_t rxIR_GetReceivedProtocol();

#ifdef __cplusplus
}
#endif

#endif // _RMTRX_H_
