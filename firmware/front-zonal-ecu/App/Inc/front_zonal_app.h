#ifndef FRONT_ZONAL_APP_H
#define FRONT_ZONAL_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "can_protocol.h"

typedef struct
{
    uint8_t command_received;
    uint32_t last_command_rx_time_ms;
    uint8_t command_led_request;
    uint8_t command_warning_led_request;

    uint8_t distance_sensor_ready;
    uint8_t distance_valid;
    uint8_t distance_range_status;
    uint8_t distance_sensor_fault_injection;
} FrontZonalAppInput;

typedef struct
{
    uint8_t alive_counter;
    uint32_t uptime_ms;
    uint16_t adc_raw;
    uint8_t adc_valid;
    uint8_t zone_switch;
    uint16_t distance_mm;
    uint8_t distance_range_status;
    uint8_t distance_valid;
    uint8_t distance_sensor_ready;
    uint8_t distance_sensor_fault_injection;
} FrontZonalStatusInput;

typedef struct
{
    uint8_t command_timeout_fault;
    uint32_t command_timeout_count;
    uint8_t applied_led_output;
    uint8_t applied_warning_led_output;
    uint8_t ecu_state;
} FrontZonalApp;

void FrontZonalApp_Init(FrontZonalApp *app);

void FrontZonalApp_Process(
    FrontZonalApp *app,
    const FrontZonalAppInput *input,
    uint32_t current_time_ms);

void FrontZonalApp_BuildStatus(
    const FrontZonalApp *app,
    const FrontZonalStatusInput *input,
    FrontZonalStatus *status);

#ifdef __cplusplus
}
#endif

#endif /* FRONT_ZONAL_APP_H */
