#include "input_service_stm32.h"

#include "adc.h"
#include "main.h"

uint8_t InputServiceStm32_Init(
    InputService *service,
    uint32_t current_time_ms)
{
    InputService_Init(service, current_time_ms);

    if (HAL_ADCEx_Calibration_Start(
            &hadc1,
            ADC_SINGLE_ENDED) != HAL_OK)
    {
        return 0U;
    }

    return 1U;
}

void InputServiceStm32_Process(
    InputService *service,
    uint32_t current_time_ms)
{
    uint8_t raw_pressed;

    if (service == 0)
    {
        return;
    }

    raw_pressed =
        (uint8_t)(HAL_GPIO_ReadPin(
                      ZONE_SWITCH_GPIO_Port,
                      ZONE_SWITCH_Pin) == GPIO_PIN_SET);

    InputService_UpdateButton(
        service,
        raw_pressed,
        current_time_ms);

    if (InputService_IsAdcSampleDue(
            service,
            current_time_ms) == 0U)
    {
        return;
    }

    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        InputService_RecordAdcFailure(service);
        return;
    }

    if (HAL_ADC_PollForConversion(
            &hadc1,
            10U) == HAL_OK)
    {
        InputService_RecordAdcSample(
            service,
            HAL_ADC_GetValue(&hadc1));
    }
    else
    {
        InputService_RecordAdcFailure(service);
    }

    (void)HAL_ADC_Stop(&hadc1);
}
