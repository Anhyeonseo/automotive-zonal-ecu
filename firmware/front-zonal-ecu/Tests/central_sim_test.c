#include "central_sim.h"

#include <assert.h>
#include <stdio.h>

static CentralSimulatorInput MakeValidInput(
    uint16_t distance_mm,
    uint32_t last_rx_time_ms)
{
    CentralSimulatorInput input = {0};

    input.distance_mm = distance_mm;
    input.range_status = 0U;
    input.distance_valid = 1U;
    input.distance_sensor_ready = 1U;
    input.last_status_rx_time_ms = last_rx_time_ms;

    return input;
}

static void TestScheduleAndHysteresis(void)
{
    CentralSimulator simulator;
    CentralSimulatorInput input;
    CentralControlCommand command = {0};

    CentralSimulator_Init(&simulator, 0U);
    input = MakeValidInput(300U, 100U);

    assert(CentralSimulator_BuildCommandIfDue(
               &simulator,
               &input,
               199U,
               &command) == 0U);

    assert(CentralSimulator_BuildCommandIfDue(
               &simulator,
               &input,
               200U,
               &command) == 1U);
    assert(simulator.distance_sensor_fault == 0U);
    assert(command.warning_led_request == 0U);

    CentralSimulator_ConfirmCommandTransmitted(&simulator);
    assert(simulator.command_alive_counter == 1U);

    input = MakeValidInput(150U, 350U);
    assert(CentralSimulator_BuildCommandIfDue(
               &simulator,
               &input,
               400U,
               &command) == 1U);
    assert(command.warning_led_request == 1U);
    assert(simulator.warning_activation_count == 1U);

    input = MakeValidInput(220U, 550U);
    assert(CentralSimulator_BuildCommandIfDue(
               &simulator,
               &input,
               600U,
               &command) == 1U);
    assert(command.warning_led_request == 1U);

    input = MakeValidInput(300U, 750U);
    assert(CentralSimulator_BuildCommandIfDue(
               &simulator,
               &input,
               800U,
               &command) == 1U);
    assert(command.warning_led_request == 0U);
}

static void TestFaultAndEnableControl(void)
{
    CentralSimulator simulator;
    CentralSimulatorInput input;
    CentralControlCommand command = {0};

    CentralSimulator_Init(&simulator, 0U);
    input = MakeValidInput(150U, 0U);

    assert(CentralSimulator_BuildCommandIfDue(
               &simulator,
               &input,
               400U,
               &command) == 1U);
    assert(simulator.distance_sensor_fault == 1U);
    assert(command.warning_led_request == 0U);

    simulator.enabled = 0U;
    assert(CentralSimulator_BuildCommandIfDue(
               &simulator,
               &input,
               800U,
               &command) == 0U);

    simulator.enabled = 1U;
    input = MakeValidInput(150U, 800U);
    assert(CentralSimulator_BuildCommandIfDue(
               &simulator,
               &input,
               800U,
               &command) == 1U);
    assert(simulator.distance_sensor_fault == 0U);
    assert(command.warning_led_request == 1U);
}

int main(void)
{
    TestScheduleAndHysteresis();
    TestFaultAndEnableControl();

    puts("central_sim tests: PASS");
    return 0;
}
