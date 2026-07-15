#include <assert.h>
#include <stdio.h>

#include "input_service.h"

static void TestAdcTimingAndConversion(void)
{
    InputService service;
    InputServiceStatus status;

    InputService_Init(&service, 100U);
    assert(InputService_IsAdcSampleDue(&service, 109U) == 0U);
    assert(InputService_IsAdcSampleDue(&service, 110U) == 1U);
    assert(InputService_IsAdcSampleDue(&service, 110U) == 0U);

    InputService_RecordAdcSample(&service, 2048U);
    InputService_GetSnapshot(&service, &status);
    assert(status.adc_raw == 2048U);
    assert(status.adc_millivolts == 1650U);
    assert(status.adc_percent == 50U);
    assert(status.adc_valid == 1U);

    InputService_RecordAdcFailure(&service);
    InputService_GetSnapshot(&service, &status);
    assert(status.adc_valid == 0U);
    assert(status.adc_error_count == 1U);

    InputService_RecordAdcSample(&service, 4095U);
    InputService_GetSnapshot(&service, &status);
    assert(status.adc_millivolts == 3300U);
    assert(status.adc_percent == 100U);
}

static void TestButtonDebounce(void)
{
    InputService service;
    InputServiceStatus status;

    InputService_Init(&service, 0U);

    InputService_UpdateButton(&service, 1U, 10U);
    InputService_UpdateButton(&service, 1U, 29U);
    InputService_GetSnapshot(&service, &status);
    assert(status.zone_switch_raw == 1U);
    assert(status.zone_switch_debounced == 0U);
    assert(status.zone_switch_press_count == 0U);

    InputService_UpdateButton(&service, 1U, 30U);
    InputService_GetSnapshot(&service, &status);
    assert(status.zone_switch_debounced == 1U);
    assert(status.zone_switch_press_count == 1U);

    InputService_UpdateButton(&service, 0U, 40U);
    InputService_UpdateButton(&service, 0U, 59U);
    InputService_GetSnapshot(&service, &status);
    assert(status.zone_switch_debounced == 1U);

    InputService_UpdateButton(&service, 0U, 60U);
    InputService_GetSnapshot(&service, &status);
    assert(status.zone_switch_debounced == 0U);
    assert(status.zone_switch_press_count == 1U);
}

int main(void)
{
    TestAdcTimingAndConversion();
    TestButtonDebounce();
    puts("input_service tests: PASS");
    return 0;
}
