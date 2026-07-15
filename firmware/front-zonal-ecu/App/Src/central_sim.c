#include "central_sim.h"
#include "time_utils.h"

#include <string.h>

#define CENTRAL_SIM_COMMAND_PERIOD_MS          200U
#define CENTRAL_SIM_DISTANCE_TIMEOUT_MS        300U
#define CENTRAL_SIM_WARNING_ON_DISTANCE_MM     200U
#define CENTRAL_SIM_WARNING_OFF_DISTANCE_MM    250U

void CentralSimulator_Init(
    CentralSimulator *simulator,
    uint32_t current_time_ms)
{
    if (simulator == 0)
    {
        return;
    }

    memset(simulator, 0, sizeof(*simulator));
    simulator->enabled = 1U;
    simulator->distance_sensor_fault = 1U;
    simulator->last_command_time_ms = current_time_ms;
}

uint8_t CentralSimulator_BuildCommandIfDue(
    CentralSimulator *simulator,
    const CentralSimulatorInput *input,
    uint32_t current_time_ms,
    CentralControlCommand *command)
{
    if ((simulator == 0) ||
        (input == 0) ||
        (command == 0) ||
        (simulator->enabled == 0U))
    {
        return 0U;
    }

    if (TimeUtils_ElapsedMs(
            current_time_ms,
            simulator->last_command_time_ms) <
        CENTRAL_SIM_COMMAND_PERIOD_MS)
    {
        return 0U;
    }

    simulator->last_command_time_ms = current_time_ms;

    if ((TimeUtils_ElapsedMs(
             current_time_ms,
             input->last_status_rx_time_ms) >
         CENTRAL_SIM_DISTANCE_TIMEOUT_MS) ||
        (input->distance_sensor_ready == 0U) ||
        (input->distance_valid == 0U) ||
        (input->range_status != 0U))
    {
        simulator->distance_sensor_fault = 1U;
        simulator->obstacle_warning_request = 0U;
    }
    else
    {
        simulator->distance_sensor_fault = 0U;

        if ((simulator->obstacle_warning_request == 0U) &&
            (input->distance_mm <=
             CENTRAL_SIM_WARNING_ON_DISTANCE_MM))
        {
            simulator->obstacle_warning_request = 1U;
            simulator->warning_activation_count++;
        }
        else if ((simulator->obstacle_warning_request != 0U) &&
                 (input->distance_mm >=
                  CENTRAL_SIM_WARNING_OFF_DISTANCE_MM))
        {
            simulator->obstacle_warning_request = 0U;
        }
    }

    command->alive_counter =
        simulator->command_alive_counter;
    command->led_request = 1U;
    command->warning_led_request =
        simulator->obstacle_warning_request;

    return 1U;
}

void CentralSimulator_ConfirmCommandTransmitted(
    CentralSimulator *simulator)
{
    if (simulator != 0)
    {
        simulator->command_alive_counter =
            (simulator->command_alive_counter + 1U) & 0x0FU;
    }
}
