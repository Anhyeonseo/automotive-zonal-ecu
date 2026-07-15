#include "front_zonal_app.h"
#include "time_utils.h"

#include <string.h>

#define FRONT_ZONAL_COMMAND_TIMEOUT_MS 500U
#define FRONT_ZONAL_ADC_MAX_RAW        4095U

void FrontZonalApp_Init(FrontZonalApp *app)
{
    if (app == 0)
    {
        return;
    }

    memset(app, 0, sizeof(*app));
    app->command_timeout_fault = 1U;
    app->ecu_state = ZONAL_ECU_STATE_INIT;
}

void FrontZonalApp_Process(
    FrontZonalApp *app,
    const FrontZonalAppInput *input,
    uint32_t current_time_ms)
{
    uint8_t command_is_fresh;

    if ((app == 0) || (input == 0))
    {
        return;
    }

    command_is_fresh =
        (uint8_t)((input->command_received != 0U) &&
                  (TimeUtils_ElapsedMs(
                       current_time_ms,
                       input->last_command_rx_time_ms) <=
                   FRONT_ZONAL_COMMAND_TIMEOUT_MS));

    if (command_is_fresh != 0U)
    {
        app->command_timeout_fault = 0U;
        app->applied_led_output =
            (uint8_t)(input->command_led_request != 0U);
        app->applied_warning_led_output =
            (uint8_t)(input->command_warning_led_request != 0U);
    }
    else
    {
        if (app->command_timeout_fault == 0U)
        {
            app->command_timeout_count++;
        }

        app->command_timeout_fault = 1U;
        app->applied_led_output = 0U;
        app->applied_warning_led_output = 0U;
    }

    if (input->distance_sensor_ready == 0U)
    {
        app->ecu_state = ZONAL_ECU_STATE_INIT;
    }
    else if (app->command_timeout_fault != 0U)
    {
        app->ecu_state = ZONAL_ECU_STATE_SAFE;
    }
    else if ((input->distance_valid == 0U) ||
             (input->distance_range_status != 0U) ||
             (input->distance_sensor_fault_injection != 0U))
    {
        app->ecu_state = ZONAL_ECU_STATE_DEGRADED;
    }
    else
    {
        app->ecu_state = ZONAL_ECU_STATE_NORMAL;
    }
}

void FrontZonalApp_BuildStatus(
    const FrontZonalApp *app,
    const FrontZonalStatusInput *input,
    FrontZonalStatus *status)
{
    if ((app == 0) || (input == 0) || (status == 0))
    {
        return;
    }

    memset(status, 0, sizeof(*status));

    status->alive_counter = input->alive_counter;
    status->ecu_state = app->ecu_state;
    status->uptime_ms = input->uptime_ms;
    status->adc_raw = input->adc_raw;
    status->position_permille =
        (uint16_t)(((uint32_t)input->adc_raw * 1000U) /
                   FRONT_ZONAL_ADC_MAX_RAW);
    status->adc_valid = input->adc_valid;
    status->zone_switch = input->zone_switch & 0x01U;
    status->distance_mm = input->distance_mm;
    status->range_status = input->distance_range_status;
    status->distance_valid = input->distance_valid;
    status->vl53l0x_ready = input->distance_sensor_ready;

    if (input->distance_sensor_fault_injection != 0U)
    {
        status->distance_mm = 0U;
        status->range_status = 255U;
        status->distance_valid = 0U;
        status->vl53l0x_ready = 0U;
    }
}
