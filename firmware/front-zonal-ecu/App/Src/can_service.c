#include "can_service.h"
#include "time_utils.h"

#include <string.h>

#define CAN_SERVICE_FRONT_STATUS_PERIOD_MS       100U
#define CAN_SERVICE_LOOPBACK_RX_TIMEOUT_MS       300U

static void ConfigureTxHeader(
    FDCAN_TxHeaderTypeDef *header,
    uint32_t identifier,
    uint32_t data_length)
{
    memset(header, 0, sizeof(*header));
    header->Identifier = identifier;
    header->IdType = FDCAN_STANDARD_ID;
    header->TxFrameType = FDCAN_DATA_FRAME;
    header->DataLength = data_length;
    header->ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    header->BitRateSwitch = FDCAN_BRS_ON;
    header->FDFormat = FDCAN_FD_CAN;
    header->TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    header->MessageMarker = 0U;
}

static uint8_t ConfigureExactStandardIdFilter(
    CanService *service,
    uint32_t filter_index,
    uint32_t identifier)
{
    FDCAN_FilterTypeDef filter = {0};

    filter.IdType = FDCAN_STANDARD_ID;
    filter.FilterIndex = filter_index;
    filter.FilterType = FDCAN_FILTER_MASK;
    filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    filter.FilterID1 = identifier;
    filter.FilterID2 = 0x7FFU;

    return (uint8_t)(HAL_FDCAN_ConfigFilter(
                         service->hfdcan,
                         &filter) == HAL_OK);
}

uint8_t CanService_Init(
    CanService *service,
    FDCAN_HandleTypeDef *hfdcan,
    uint32_t current_time_ms)
{
    if ((service == 0) || (hfdcan == 0))
    {
        return 0U;
    }

    memset(service, 0, sizeof(*service));
    service->hfdcan = hfdcan;
    service->last_front_status_tx_time_ms = current_time_ms;
    service->status.last_front_status.range_status = 255U;

    ConfigureTxHeader(
        &service->front_status_tx_header,
        CAN_PROTOCOL_FRONT_ZONAL_STATUS_ID,
        FDCAN_DLC_BYTES_16);
    ConfigureTxHeader(
        &service->central_command_tx_header,
        CAN_PROTOCOL_CENTRAL_COMMAND_ID,
        FDCAN_DLC_BYTES_8);

    if ((ConfigureExactStandardIdFilter(
             service,
             0U,
             CAN_PROTOCOL_FRONT_ZONAL_STATUS_ID) == 0U) ||
        (ConfigureExactStandardIdFilter(
             service,
             1U,
             CAN_PROTOCOL_CENTRAL_COMMAND_ID) == 0U))
    {
        return 0U;
    }

    if (HAL_FDCAN_Start(service->hfdcan) != HAL_OK)
    {
        return 0U;
    }

    if (HAL_FDCAN_ActivateNotification(
            service->hfdcan,
            FDCAN_IT_RX_FIFO0_NEW_MESSAGE,
            0U) != HAL_OK)
    {
        return 0U;
    }

    return 1U;
}

uint8_t CanService_IsFrontStatusTxDue(
    CanService *service,
    uint32_t current_time_ms)
{
    if (service == 0)
    {
        return 0U;
    }

    if (TimeUtils_ElapsedMs(
            current_time_ms,
            service->last_front_status_tx_time_ms) <
        CAN_SERVICE_FRONT_STATUS_PERIOD_MS)
    {
        return 0U;
    }

    service->last_front_status_tx_time_ms = current_time_ms;
    return 1U;
}

uint8_t CanService_GetNextFrontStatusAliveCounter(
    const CanService *service)
{
    if (service == 0)
    {
        return 0U;
    }

    return service->next_front_status_tx_alive_counter;
}

uint8_t CanService_SendFrontStatus(
    CanService *service,
    const FrontZonalStatus *status)
{
    FrontZonalStatus tx_status;

    if ((service == 0) || (status == 0))
    {
        return 0U;
    }

    tx_status = *status;
    tx_status.alive_counter =
        service->next_front_status_tx_alive_counter;

    if (CanProtocol_EncodeFrontZonalStatus(
            &tx_status,
            service->front_status_tx_data,
            CAN_PROTOCOL_FRONT_ZONAL_STATUS_LENGTH) !=
        CAN_PROTOCOL_OK)
    {
        service->status.front_status_encode_error_count++;
        service->status.last_error_reason =
            CAN_SERVICE_ERROR_FRONT_STATUS_ENCODE;
        service->status.loopback_result = 2U;
        return 0U;
    }

    if (HAL_FDCAN_AddMessageToTxFifoQ(
            service->hfdcan,
            &service->front_status_tx_header,
            service->front_status_tx_data) != HAL_OK)
    {
        service->status.front_status_hal_tx_error_count++;
        service->status.last_error_reason =
            CAN_SERVICE_ERROR_FRONT_STATUS_HAL_TX;
        service->status.loopback_result = 2U;
        return 0U;
    }

    service->status.front_status_tx_count++;
    service->next_front_status_tx_alive_counter =
        (service->next_front_status_tx_alive_counter + 1U) & 0x0FU;
    return 1U;
}

uint8_t CanService_SendCentralCommand(
    CanService *service,
    const CentralControlCommand *command)
{
    if ((service == 0) || (command == 0))
    {
        return 0U;
    }

    if (CanProtocol_EncodeCentralControlCommand(
            command,
            service->central_command_tx_data,
            CAN_PROTOCOL_CENTRAL_COMMAND_LENGTH) !=
        CAN_PROTOCOL_OK)
    {
        service->status.central_command_encode_error_count++;
        service->status.last_error_reason =
            CAN_SERVICE_ERROR_CENTRAL_COMMAND_ENCODE;
        service->status.loopback_result = 2U;
        return 0U;
    }

    if (HAL_FDCAN_AddMessageToTxFifoQ(
            service->hfdcan,
            &service->central_command_tx_header,
            service->central_command_tx_data) != HAL_OK)
    {
        service->status.central_command_hal_tx_error_count++;
        service->status.last_error_reason =
            CAN_SERVICE_ERROR_CENTRAL_COMMAND_HAL_TX;
        service->status.loopback_result = 2U;
        return 0U;
    }

    service->status.central_command_tx_count++;
    return 1U;
}

void CanService_Process(
    CanService *service,
    uint32_t current_time_ms)
{
    if (service == 0)
    {
        return;
    }

    if ((service->status.front_status_tx_count > 0U) &&
        (TimeUtils_ElapsedMs(
             current_time_ms,
             service->status.last_front_status_rx_time_ms) >
         CAN_SERVICE_LOOPBACK_RX_TIMEOUT_MS))
    {
        if (service->front_status_rx_timeout_active == 0U)
        {
            service->status.front_status_rx_timeout_count++;
        }

        service->front_status_rx_timeout_active = 1U;
        service->status.last_error_reason =
            CAN_SERVICE_ERROR_FRONT_STATUS_RX_TIMEOUT;
        service->status.loopback_result = 2U;
    }
    else
    {
        service->front_status_rx_timeout_active = 0U;
    }
}

void CanService_GetSnapshot(
    const CanService *service,
    CanServiceStatus *status)
{
    uint32_t previous_primask;

    if ((service == 0) || (status == 0))
    {
        return;
    }

    previous_primask = __get_PRIMASK();
    __disable_irq();
    *status = service->status;

    if (previous_primask == 0U)
    {
        __enable_irq();
    }
}

void CanService_OnRxFifo0Interrupt(
    CanService *service,
    FDCAN_HandleTypeDef *hfdcan,
    uint32_t rx_fifo0_interrupts,
    uint32_t current_time_ms)
{
    if ((service == 0) ||
        (hfdcan == 0) ||
        (service->hfdcan != hfdcan) ||
        ((rx_fifo0_interrupts &
          FDCAN_IT_RX_FIFO0_NEW_MESSAGE) == 0U))
    {
        return;
    }

    if (HAL_FDCAN_GetRxMessage(
            hfdcan,
            FDCAN_RX_FIFO0,
            &service->rx_header,
            service->rx_data) != HAL_OK)
    {
        return;
    }

    service->status.loopback_rx_count++;

    if ((service->rx_header.Identifier ==
         CAN_PROTOCOL_FRONT_ZONAL_STATUS_ID) &&
        (service->rx_header.DataLength ==
         FDCAN_DLC_BYTES_16))
    {
        FrontZonalStatus received_status = {0};
        CanProtocolResult decode_result;

        decode_result = CanProtocol_DecodeFrontZonalStatus(
            service->rx_data,
            CAN_PROTOCOL_FRONT_ZONAL_STATUS_LENGTH,
            &received_status);

        service->status.last_front_status = received_status;

        if (decode_result != CAN_PROTOCOL_OK)
        {
            service->status.front_status_payload_error_count++;
            service->status.last_error_reason =
                CAN_SERVICE_ERROR_FRONT_STATUS_PAYLOAD;
        }

        if (service->status.front_status_rx_count > 0U)
        {
            uint8_t expected_alive_counter;

            expected_alive_counter =
                (service->last_front_status_rx_alive_counter + 1U) &
                0x0FU;

            if (received_status.alive_counter !=
                expected_alive_counter)
            {
                service->status.front_status_sequence_error_count++;
            }
        }

        service->last_front_status_rx_alive_counter =
            received_status.alive_counter;
        service->status.last_front_status_rx_time_ms =
            current_time_ms;
        service->front_status_rx_timeout_active = 0U;
        service->status.front_status_rx_count++;
        service->status.loopback_result = 1U;
    }
    else if ((service->rx_header.Identifier ==
              CAN_PROTOCOL_CENTRAL_COMMAND_ID) &&
             (service->rx_header.DataLength ==
              FDCAN_DLC_BYTES_8))
    {
        CentralControlCommand received_command = {0};
        CanProtocolResult decode_result;

        decode_result = CanProtocol_DecodeCentralControlCommand(
            service->rx_data,
            CAN_PROTOCOL_CENTRAL_COMMAND_LENGTH,
            &received_command);

        if (decode_result != CAN_PROTOCOL_OK)
        {
            service->status.central_command_payload_error_count++;
            service->status.last_error_reason =
                CAN_SERVICE_ERROR_CENTRAL_COMMAND_PAYLOAD;
            service->status.loopback_result = 2U;
            return;
        }

        if (service->status.central_command_rx_count > 0U)
        {
            uint8_t expected_alive_counter;

            expected_alive_counter =
                (service->last_central_command_rx_alive_counter + 1U) &
                0x0FU;

            if (received_command.alive_counter !=
                expected_alive_counter)
            {
                service->status.central_command_sequence_error_count++;
            }
        }

        service->last_central_command_rx_alive_counter =
            received_command.alive_counter;
        service->status.last_central_command = received_command;
        service->status.last_central_command_rx_time_ms =
            current_time_ms;
        service->status.central_command_rx_count++;
        service->status.loopback_result = 1U;
    }
    else
    {
        service->status.unknown_rx_frame_count++;
        service->status.last_error_reason =
            CAN_SERVICE_ERROR_UNKNOWN_RX_FRAME;
        service->status.loopback_result = 2U;
    }
}
