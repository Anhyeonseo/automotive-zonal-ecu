#include "can_protocol.h"

#include <assert.h>
#include <stdio.h>

static void TestFrontZonalStatusRoundTrip(void)
{
    FrontZonalStatus source = {0};
    FrontZonalStatus decoded = {0};
    uint8_t data[CAN_PROTOCOL_FRONT_ZONAL_STATUS_LENGTH] = {0};

    source.alive_counter = 0x0AU;
    source.ecu_state = ZONAL_ECU_STATE_NORMAL;
    source.uptime_ms = 0x12345678UL;
    source.adc_raw = 2048U;
    source.position_permille = 500U;
    source.adc_valid = 1U;
    source.zone_switch = 1U;
    source.distance_mm = 350U;
    source.range_status = 0U;
    source.distance_valid = 1U;
    source.vl53l0x_ready = 1U;

    assert(CanProtocol_EncodeFrontZonalStatus(
               &source,
               data,
               sizeof(data)) == CAN_PROTOCOL_OK);

    assert(data[0] == 0x0AU);
    assert(data[1] == ZONAL_ECU_STATE_NORMAL);
    assert(data[2] == 0x78U);
    assert(data[3] == 0x56U);
    assert(data[4] == 0x34U);
    assert(data[5] == 0x12U);
    assert(data[6] == 0x00U);
    assert(data[7] == 0x08U);
    assert(data[8] == 0xF4U);
    assert(data[9] == 0x01U);
    assert(data[10] == 0x01U);
    assert(data[11] == 0x01U);
    assert(data[12] == 0x5EU);
    assert(data[13] == 0x01U);
    assert(data[14] == 0x00U);
    assert(data[15] == 0x03U);

    assert(CanProtocol_DecodeFrontZonalStatus(
               data,
               sizeof(data),
               &decoded) == CAN_PROTOCOL_OK);

    assert(decoded.alive_counter == source.alive_counter);
    assert(decoded.ecu_state == source.ecu_state);
    assert(decoded.uptime_ms == source.uptime_ms);
    assert(decoded.adc_raw == source.adc_raw);
    assert(decoded.position_permille == source.position_permille);
    assert(decoded.adc_valid == source.adc_valid);
    assert(decoded.zone_switch == source.zone_switch);
    assert(decoded.distance_mm == source.distance_mm);
    assert(decoded.range_status == source.range_status);
    assert(decoded.distance_valid == source.distance_valid);
    assert(decoded.vl53l0x_ready == source.vl53l0x_ready);
}

static void TestFrontZonalStatusRejectsInvalidPayload(void)
{
    FrontZonalStatus decoded = {0};
    uint8_t data[CAN_PROTOCOL_FRONT_ZONAL_STATUS_LENGTH] = {0};

    data[1] = ZONAL_ECU_STATE_NORMAL;
    data[15] = 0x80U;

    assert(CanProtocol_DecodeFrontZonalStatus(
               data,
               sizeof(data),
               &decoded) == CAN_PROTOCOL_ERROR_PAYLOAD);

    data[15] = 0x03U;
    data[14] = 1U;

    assert(CanProtocol_DecodeFrontZonalStatus(
               data,
               sizeof(data),
               &decoded) == CAN_PROTOCOL_ERROR_PAYLOAD);
}

static void TestCentralControlCommandRoundTrip(void)
{
    CentralControlCommand source = {0};
    CentralControlCommand decoded = {0};
    uint8_t data[CAN_PROTOCOL_CENTRAL_COMMAND_LENGTH] = {0};
    uint32_t index;

    source.alive_counter = 7U;
    source.led_request = 1U;
    source.warning_led_request = 1U;

    assert(CanProtocol_EncodeCentralControlCommand(
               &source,
               data,
               sizeof(data)) == CAN_PROTOCOL_OK);

    assert(data[0] == 7U);
    assert(data[1] == 1U);
    assert(data[2] == 1U);

    for (index = 3U;
         index < CAN_PROTOCOL_CENTRAL_COMMAND_LENGTH;
         index++)
    {
        assert(data[index] == 0U);
    }

    assert(CanProtocol_DecodeCentralControlCommand(
               data,
               sizeof(data),
               &decoded) == CAN_PROTOCOL_OK);

    assert(decoded.alive_counter == source.alive_counter);
    assert(decoded.led_request == source.led_request);
    assert(decoded.warning_led_request ==
           source.warning_led_request);

    data[2] = 2U;

    assert(CanProtocol_DecodeCentralControlCommand(
               data,
               sizeof(data),
               &decoded) == CAN_PROTOCOL_ERROR_PAYLOAD);
}

int main(void)
{
    TestFrontZonalStatusRoundTrip();
    TestFrontZonalStatusRejectsInvalidPayload();
    TestCentralControlCommandRoundTrip();

    puts("can_protocol tests: PASS");
    return 0;
}
