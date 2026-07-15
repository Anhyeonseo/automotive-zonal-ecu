#ifndef CAN_SERVICE_H
#define CAN_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "can_protocol.h"
#include "fdcan.h"

typedef enum
{
    CAN_SERVICE_ERROR_NONE = 0U,
    CAN_SERVICE_ERROR_FRONT_STATUS_ENCODE = 1U,
    CAN_SERVICE_ERROR_FRONT_STATUS_HAL_TX = 2U,
    CAN_SERVICE_ERROR_CENTRAL_COMMAND_ENCODE = 3U,
    CAN_SERVICE_ERROR_CENTRAL_COMMAND_HAL_TX = 4U,
    CAN_SERVICE_ERROR_FRONT_STATUS_RX_TIMEOUT = 5U,
    CAN_SERVICE_ERROR_CENTRAL_COMMAND_PAYLOAD = 6U,
    CAN_SERVICE_ERROR_UNKNOWN_RX_FRAME = 7U,
    CAN_SERVICE_ERROR_FRONT_STATUS_PAYLOAD = 8U
} CanServiceErrorReason;

typedef struct
{
    uint32_t loopback_result;
    uint32_t loopback_rx_count;

    uint32_t front_status_tx_count;
    uint32_t front_status_rx_count;
    uint32_t front_status_sequence_error_count;
    uint32_t front_status_payload_error_count;
    uint32_t front_status_encode_error_count;
    uint32_t front_status_hal_tx_error_count;
    uint32_t front_status_rx_timeout_count;
    uint32_t last_front_status_rx_time_ms;
    FrontZonalStatus last_front_status;

    uint32_t central_command_tx_count;
    uint32_t central_command_rx_count;
    uint32_t central_command_sequence_error_count;
    uint32_t central_command_payload_error_count;
    uint32_t central_command_encode_error_count;
    uint32_t central_command_hal_tx_error_count;
    uint32_t last_central_command_rx_time_ms;
    CentralControlCommand last_central_command;

    uint32_t unknown_rx_frame_count;
    uint8_t last_error_reason;
} CanServiceStatus;

typedef struct
{
    FDCAN_HandleTypeDef *hfdcan;
    FDCAN_TxHeaderTypeDef front_status_tx_header;
    FDCAN_TxHeaderTypeDef central_command_tx_header;
    FDCAN_RxHeaderTypeDef rx_header;

    uint8_t front_status_tx_data[CAN_PROTOCOL_FRONT_ZONAL_STATUS_LENGTH];
    uint8_t central_command_tx_data[CAN_PROTOCOL_CENTRAL_COMMAND_LENGTH];
    uint8_t rx_data[CAN_PROTOCOL_FRONT_ZONAL_STATUS_LENGTH];

    uint32_t last_front_status_tx_time_ms;
    uint8_t next_front_status_tx_alive_counter;
    uint8_t last_front_status_rx_alive_counter;
    uint8_t last_central_command_rx_alive_counter;
    uint8_t front_status_rx_timeout_active;

    CanServiceStatus status;
} CanService;

uint8_t CanService_Init(
    CanService *service,
    FDCAN_HandleTypeDef *hfdcan,
    uint32_t current_time_ms);

uint8_t CanService_IsFrontStatusTxDue(
    CanService *service,
    uint32_t current_time_ms);

uint8_t CanService_GetNextFrontStatusAliveCounter(
    const CanService *service);

uint8_t CanService_SendFrontStatus(
    CanService *service,
    const FrontZonalStatus *status);

uint8_t CanService_SendCentralCommand(
    CanService *service,
    const CentralControlCommand *command);

void CanService_Process(
    CanService *service,
    uint32_t current_time_ms);

void CanService_GetSnapshot(
    const CanService *service,
    CanServiceStatus *status);

void CanService_OnRxFifo0Interrupt(
    CanService *service,
    FDCAN_HandleTypeDef *hfdcan,
    uint32_t rx_fifo0_interrupts,
    uint32_t current_time_ms);

#ifdef __cplusplus
}
#endif

#endif /* CAN_SERVICE_H */
