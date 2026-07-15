#ifndef INPUT_SERVICE_H
#define INPUT_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    uint32_t adc_raw;
    uint32_t adc_millivolts;
    uint32_t adc_percent;
    uint8_t adc_valid;
    uint32_t adc_error_count;

    uint8_t zone_switch_raw;
    uint8_t zone_switch_debounced;
    uint32_t zone_switch_press_count;
} InputServiceStatus;

typedef struct
{
    InputServiceStatus status;
    uint8_t zone_switch_previous_raw;
    uint32_t zone_switch_change_time_ms;
    uint32_t adc_last_sample_time_ms;
} InputService;

void InputService_Init(
    InputService *service,
    uint32_t current_time_ms);

void InputService_UpdateButton(
    InputService *service,
    uint8_t raw_pressed,
    uint32_t current_time_ms);

uint8_t InputService_IsAdcSampleDue(
    InputService *service,
    uint32_t current_time_ms);

void InputService_RecordAdcSample(
    InputService *service,
    uint32_t adc_raw);

void InputService_RecordAdcFailure(
    InputService *service);

void InputService_GetSnapshot(
    const InputService *service,
    InputServiceStatus *status);

#ifdef __cplusplus
}
#endif

#endif /* INPUT_SERVICE_H */
