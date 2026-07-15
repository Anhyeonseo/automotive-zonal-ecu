#include "can_protocol.h"

static void WriteUint16LittleEndian(
    uint8_t *data,
    uint16_t value)
{
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8);
}

static void WriteUint32LittleEndian(
    uint8_t *data,
    uint32_t value)
{
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8);
    data[2] = (uint8_t)(value >> 16);
    data[3] = (uint8_t)(value >> 24);
}

static uint16_t ReadUint16LittleEndian(
    const uint8_t *data)
{
    return (uint16_t)data[0] |
           ((uint16_t)data[1] << 8);
}

static uint32_t ReadUint32LittleEndian(
    const uint8_t *data)
{
    return (uint32_t)data[0] |
           ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) |
           ((uint32_t)data[3] << 24);
}

static uint8_t IsFrontZonalStatusValid(
    const FrontZonalStatus *status)
{
    if ((status->ecu_state > (uint8_t)ZONAL_ECU_STATE_SAFE) ||
        (status->adc_raw > 4095U) ||
        (status->position_permille > 1000U) ||
        (status->adc_valid > 1U) ||
        (status->zone_switch > 1U) ||
        (status->distance_valid > 1U) ||
        (status->vl53l0x_ready > 1U))
    {
        return 0U;
    }

    if ((status->distance_valid != 0U) &&
        ((status->range_status != 0U) ||
         (status->vl53l0x_ready == 0U)))
    {
        return 0U;
    }

    return 1U;
}

CanProtocolResult CanProtocol_EncodeFrontZonalStatus(
    const FrontZonalStatus *status,
    uint8_t *data,
    uint32_t length)
{
    uint8_t distance_flags = 0U;

    if ((status == 0) || (data == 0))
    {
        return CAN_PROTOCOL_ERROR_NULL;
    }

    if (length != CAN_PROTOCOL_FRONT_ZONAL_STATUS_LENGTH)
    {
        return CAN_PROTOCOL_ERROR_LENGTH;
    }

    if (IsFrontZonalStatusValid(status) == 0U)
    {
        return CAN_PROTOCOL_ERROR_PAYLOAD;
    }

    data[0] = status->alive_counter & 0x0FU;
    data[1] = status->ecu_state;
    WriteUint32LittleEndian(&data[2], status->uptime_ms);
    WriteUint16LittleEndian(&data[6], status->adc_raw);
    WriteUint16LittleEndian(&data[8], status->position_permille);
    data[10] = status->adc_valid & 0x01U;
    data[11] = status->zone_switch & 0x01U;
    WriteUint16LittleEndian(&data[12], status->distance_mm);
    data[14] = status->range_status;

    if (status->distance_valid != 0U)
    {
        distance_flags |= 0x01U;
    }

    if (status->vl53l0x_ready != 0U)
    {
        distance_flags |= 0x02U;
    }

    data[15] = distance_flags;

    return CAN_PROTOCOL_OK;
}

CanProtocolResult CanProtocol_DecodeFrontZonalStatus(
    const uint8_t *data,
    uint32_t length,
    FrontZonalStatus *status)
{
    uint8_t distance_flags;

    if ((data == 0) || (status == 0))
    {
        return CAN_PROTOCOL_ERROR_NULL;
    }

    if (length != CAN_PROTOCOL_FRONT_ZONAL_STATUS_LENGTH)
    {
        return CAN_PROTOCOL_ERROR_LENGTH;
    }

    distance_flags = data[15];

    status->alive_counter = data[0] & 0x0FU;
    status->ecu_state = data[1];
    status->uptime_ms = ReadUint32LittleEndian(&data[2]);
    status->adc_raw = ReadUint16LittleEndian(&data[6]);
    status->position_permille = ReadUint16LittleEndian(&data[8]);
    status->adc_valid = data[10] & 0x01U;
    status->zone_switch = data[11];
    status->distance_mm = ReadUint16LittleEndian(&data[12]);
    status->range_status = data[14];
    status->distance_valid = distance_flags & 0x01U;
    status->vl53l0x_ready = (distance_flags >> 1) & 0x01U;

    if ((distance_flags & 0xFCU) != 0U)
    {
        return CAN_PROTOCOL_ERROR_PAYLOAD;
    }

    if (IsFrontZonalStatusValid(status) == 0U)
    {
        return CAN_PROTOCOL_ERROR_PAYLOAD;
    }

    return CAN_PROTOCOL_OK;
}

CanProtocolResult CanProtocol_EncodeCentralControlCommand(
    const CentralControlCommand *command,
    uint8_t *data,
    uint32_t length)
{
    uint32_t index;

    if ((command == 0) || (data == 0))
    {
        return CAN_PROTOCOL_ERROR_NULL;
    }

    if (length != CAN_PROTOCOL_CENTRAL_COMMAND_LENGTH)
    {
        return CAN_PROTOCOL_ERROR_LENGTH;
    }

    if ((command->led_request > 1U) ||
        (command->warning_led_request > 1U))
    {
        return CAN_PROTOCOL_ERROR_PAYLOAD;
    }

    data[0] = command->alive_counter & 0x0FU;
    data[1] = command->led_request;
    data[2] = command->warning_led_request;

    for (index = 3U;
         index < CAN_PROTOCOL_CENTRAL_COMMAND_LENGTH;
         index++)
    {
        data[index] = 0U;
    }

    return CAN_PROTOCOL_OK;
}

CanProtocolResult CanProtocol_DecodeCentralControlCommand(
    const uint8_t *data,
    uint32_t length,
    CentralControlCommand *command)
{
    if ((data == 0) || (command == 0))
    {
        return CAN_PROTOCOL_ERROR_NULL;
    }

    if (length != CAN_PROTOCOL_CENTRAL_COMMAND_LENGTH)
    {
        return CAN_PROTOCOL_ERROR_LENGTH;
    }

    command->alive_counter = data[0] & 0x0FU;
    command->led_request = data[1];
    command->warning_led_request = data[2];

    if ((command->led_request > 1U) ||
        (command->warning_led_request > 1U))
    {
        return CAN_PROTOCOL_ERROR_PAYLOAD;
    }

    return CAN_PROTOCOL_OK;
}
