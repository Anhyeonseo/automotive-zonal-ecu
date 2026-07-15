#ifndef CENTRAL_SIM_H
#define CENTRAL_SIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "can_protocol.h"

#include <stdint.h>

typedef struct
{
    uint16_t distance_mm;
    uint8_t range_status;
    uint8_t distance_valid;
    uint8_t distance_sensor_ready;
    uint32_t last_status_rx_time_ms;
} CentralSimulatorInput;

typedef struct
{
    uint8_t enabled;
    uint8_t command_alive_counter;
    uint8_t obstacle_warning_request;
    uint8_t distance_sensor_fault;
    uint32_t warning_activation_count;
    uint32_t last_command_time_ms;
} CentralSimulator;

void CentralSimulator_Init(
    CentralSimulator *simulator,
    uint32_t current_time_ms);

uint8_t CentralSimulator_BuildCommandIfDue(
    CentralSimulator *simulator,
    const CentralSimulatorInput *input,
    uint32_t current_time_ms,
    CentralControlCommand *command);

void CentralSimulator_ConfirmCommandTransmitted(
    CentralSimulator *simulator);

#ifdef __cplusplus
}
#endif

#endif /* CENTRAL_SIM_H */
