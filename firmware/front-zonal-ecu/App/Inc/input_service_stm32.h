#ifndef INPUT_SERVICE_STM32_H
#define INPUT_SERVICE_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "input_service.h"

uint8_t InputServiceStm32_Init(
    InputService *service,
    uint32_t current_time_ms);

void InputServiceStm32_Process(
    InputService *service,
    uint32_t current_time_ms);

#ifdef __cplusplus
}
#endif

#endif /* INPUT_SERVICE_STM32_H */
