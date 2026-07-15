#include "input_service.h"

#include <string.h>

#define INPUT_SERVICE_ADC_PERIOD_MS          10U
#define INPUT_SERVICE_BUTTON_DEBOUNCE_MS     20U
#define INPUT_SERVICE_ADC_MAX_RAW          4095U
#define INPUT_SERVICE_ADC_REFERENCE_MV     3300U

void InputService_Init(
    InputService *service,
    uint32_t current_time_ms)
{
    if (service == 0)
    {
        return;
    }

    memset(service, 0, sizeof(*service));
    service->adc_last_sample_time_ms = current_time_ms;
    service->zone_switch_change_time_ms = current_time_ms;
}

void InputService_UpdateButton(
    InputService *service,
    uint8_t raw_pressed,
    uint32_t current_time_ms)
{
    uint8_t normalized_raw;

    if (service == 0)
    {
        return;
    }

    normalized_raw = (uint8_t)(raw_pressed != 0U);
    service->status.zone_switch_raw = normalized_raw;

    if (normalized_raw != service->zone_switch_previous_raw)
    {
        service->zone_switch_previous_raw = normalized_raw;
        service->zone_switch_change_time_ms = current_time_ms;
    }

    if (((current_time_ms - service->zone_switch_change_time_ms) >=
         INPUT_SERVICE_BUTTON_DEBOUNCE_MS) &&
        (service->status.zone_switch_debounced !=
         service->zone_switch_previous_raw))
    {
        service->status.zone_switch_debounced =
            service->zone_switch_previous_raw;

        if (service->status.zone_switch_debounced != 0U)
        {
            service->status.zone_switch_press_count++;
        }
    }
}

uint8_t InputService_IsAdcSampleDue(
    InputService *service,
    uint32_t current_time_ms)
{
    if (service == 0)
    {
        return 0U;
    }

    if ((current_time_ms - service->adc_last_sample_time_ms) <
        INPUT_SERVICE_ADC_PERIOD_MS)
    {
        return 0U;
    }

    service->adc_last_sample_time_ms = current_time_ms;
    return 1U;
}

void InputService_RecordAdcSample(
    InputService *service,
    uint32_t adc_raw)
{
    if (service == 0)
    {
        return;
    }

    service->status.adc_raw = adc_raw;
    service->status.adc_millivolts =
        (adc_raw * INPUT_SERVICE_ADC_REFERENCE_MV) /
        INPUT_SERVICE_ADC_MAX_RAW;
    service->status.adc_percent =
        (adc_raw * 100U) / INPUT_SERVICE_ADC_MAX_RAW;
    service->status.adc_valid = 1U;
}

void InputService_RecordAdcFailure(
    InputService *service)
{
    if (service == 0)
    {
        return;
    }

    service->status.adc_valid = 0U;
    service->status.adc_error_count++;
}

void InputService_GetSnapshot(
    const InputService *service,
    InputServiceStatus *status)
{
    if ((service == 0) || (status == 0))
    {
        return;
    }

    *status = service->status;
}
