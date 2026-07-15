#ifndef CAN_PROTOCOL_H
#define CAN_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CAN_PROTOCOL_FRONT_ZONAL_STATUS_ID       0x180U
#define CAN_PROTOCOL_CENTRAL_COMMAND_ID          0x200U

#define CAN_PROTOCOL_FRONT_ZONAL_STATUS_LENGTH   16U
#define CAN_PROTOCOL_CENTRAL_COMMAND_LENGTH      8U

typedef enum
{
    ZONAL_ECU_STATE_INIT = 0U,
    ZONAL_ECU_STATE_NORMAL = 1U,
    ZONAL_ECU_STATE_DEGRADED = 2U,
    ZONAL_ECU_STATE_SAFE = 3U
} ZonalEcuState;

typedef enum
{
    CAN_PROTOCOL_OK = 0,
    CAN_PROTOCOL_ERROR_NULL,
    CAN_PROTOCOL_ERROR_LENGTH,
    CAN_PROTOCOL_ERROR_PAYLOAD
} CanProtocolResult;

typedef struct
{
    uint8_t alive_counter;
    uint8_t ecu_state;
    uint32_t uptime_ms;
    uint16_t adc_raw;
    uint16_t position_permille;
    uint8_t adc_valid;
    uint8_t zone_switch;
    uint16_t distance_mm;
    uint8_t range_status;
    uint8_t distance_valid;
    uint8_t vl53l0x_ready;
} FrontZonalStatus;

typedef struct
{
    uint8_t alive_counter;
    uint8_t led_request;
    uint8_t warning_led_request;
} CentralControlCommand;

CanProtocolResult CanProtocol_EncodeFrontZonalStatus(
    const FrontZonalStatus *status,
    uint8_t *data,
    uint32_t length);

CanProtocolResult CanProtocol_DecodeFrontZonalStatus(
    const uint8_t *data,
    uint32_t length,
    FrontZonalStatus *status);

CanProtocolResult CanProtocol_EncodeCentralControlCommand(
    const CentralControlCommand *command,
    uint8_t *data,
    uint32_t length);

CanProtocolResult CanProtocol_DecodeCentralControlCommand(
    const uint8_t *data,
    uint32_t length,
    CentralControlCommand *command);

#ifdef __cplusplus
}
#endif

#endif /* CAN_PROTOCOL_H */
