#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "front_zonal_app.h"

static FrontZonalAppInput MakeNormalInput(void)
{
    FrontZonalAppInput input = {0};

    input.command_received = 1U;
    input.last_command_rx_time_ms = 1000U;
    input.command_led_request = 1U;
    input.command_warning_led_request = 1U;
    input.distance_sensor_ready = 1U;
    input.distance_valid = 1U;
    input.distance_range_status = 0U;

    return input;
}

static void TestStateAndOutputTransitions(void)
{
    FrontZonalApp app;
    FrontZonalAppInput input;

    FrontZonalApp_Init(&app);
    assert(app.command_timeout_fault == 1U);
    assert(app.ecu_state == ZONAL_ECU_STATE_INIT);

    input = MakeNormalInput();
    FrontZonalApp_Process(&app, &input, 1100U);
    assert(app.command_timeout_fault == 0U);
    assert(app.applied_led_output == 1U);
    assert(app.applied_warning_led_output == 1U);
    assert(app.ecu_state == ZONAL_ECU_STATE_NORMAL);

    input.distance_valid = 0U;
    FrontZonalApp_Process(&app, &input, 1200U);
    assert(app.ecu_state == ZONAL_ECU_STATE_DEGRADED);

    input.distance_valid = 1U;
    input.distance_sensor_fault_injection = 1U;
    FrontZonalApp_Process(&app, &input, 1300U);
    assert(app.ecu_state == ZONAL_ECU_STATE_DEGRADED);

    input.distance_sensor_fault_injection = 0U;
    FrontZonalApp_Process(&app, &input, 1600U);
    assert(app.command_timeout_fault == 1U);
    assert(app.command_timeout_count == 1U);
    assert(app.applied_led_output == 0U);
    assert(app.applied_warning_led_output == 0U);
    assert(app.ecu_state == ZONAL_ECU_STATE_SAFE);

    FrontZonalApp_Process(&app, &input, 1700U);
    assert(app.command_timeout_count == 1U);

    input.last_command_rx_time_ms = 1700U;
    input.command_warning_led_request = 0U;
    FrontZonalApp_Process(&app, &input, 1750U);
    assert(app.command_timeout_fault == 0U);
    assert(app.applied_led_output == 1U);
    assert(app.applied_warning_led_output == 0U);
    assert(app.ecu_state == ZONAL_ECU_STATE_NORMAL);

    input.distance_sensor_ready = 0U;
    FrontZonalApp_Process(&app, &input, 1800U);
    assert(app.ecu_state == ZONAL_ECU_STATE_INIT);
}

static void TestStatusBuildAndFaultInjection(void)
{
    FrontZonalApp app;
    FrontZonalStatusInput input;
    FrontZonalStatus status;

    FrontZonalApp_Init(&app);
    app.ecu_state = ZONAL_ECU_STATE_NORMAL;

    memset(&input, 0, sizeof(input));
    input.alive_counter = 7U;
    input.uptime_ms = 1234U;
    input.adc_raw = 2048U;
    input.adc_valid = 1U;
    input.zone_switch = 1U;
    input.distance_mm = 321U;
    input.distance_range_status = 0U;
    input.distance_valid = 1U;
    input.distance_sensor_ready = 1U;

    FrontZonalApp_BuildStatus(&app, &input, &status);
    assert(status.alive_counter == 7U);
    assert(status.ecu_state == ZONAL_ECU_STATE_NORMAL);
    assert(status.uptime_ms == 1234U);
    assert(status.adc_raw == 2048U);
    assert(status.position_permille == 500U);
    assert(status.zone_switch == 1U);
    assert(status.distance_mm == 321U);
    assert(status.distance_valid == 1U);
    assert(status.vl53l0x_ready == 1U);

    input.distance_sensor_fault_injection = 1U;
    FrontZonalApp_BuildStatus(&app, &input, &status);
    assert(status.distance_mm == 0U);
    assert(status.range_status == 255U);
    assert(status.distance_valid == 0U);
    assert(status.vl53l0x_ready == 0U);
}

int main(void)
{
    TestStateAndOutputTransitions();
    TestStatusBuildAndFaultInjection();
    puts("front_zonal_app tests: PASS");
    return 0;
}
