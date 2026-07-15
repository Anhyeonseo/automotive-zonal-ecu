/*
 * COPYRIGHT (C) STMicroelectronics 2015. All rights reserved.
 *
 * This software is the confidential and proprietary information of
 * STMicroelectronics ("Confidential Information").  You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered into
 * with STMicroelectronics
 *
 * Programming Golden Rule: Keep it Simple!
 *
 */

/*!
 * \file   VL53L0X_platform.c
 * \brief  Code function defintions for Ewok Platform Layer
 *
 */


#include "vl53l0x_i2c_platform.h"
#include "i2c.h"

#define VL53L0X_PLATFORM_STATUS_OK       0
#define VL53L0X_PLATFORM_STATUS_FAIL     1
#define VL53L0X_I2C_TIMEOUT_MS           100U

static int32_t VL53L0X_ConvertHalStatus(
    HAL_StatusTypeDef hal_status)
{
    if (hal_status == HAL_OK)
    {
        return VL53L0X_PLATFORM_STATUS_OK;
    }

    return VL53L0X_PLATFORM_STATUS_FAIL;
}

int32_t VL53L0X_comms_initialise(
    uint8_t comms_type,
    uint16_t comms_speed_khz)
{
    (void)comms_speed_khz;

    if (comms_type != I2C)
    {
        return VL53L0X_PLATFORM_STATUS_FAIL;
    }

    if (HAL_I2C_GetState(&hi2c1) !=
        HAL_I2C_STATE_READY)
    {
        return VL53L0X_PLATFORM_STATUS_FAIL;
    }

    return VL53L0X_PLATFORM_STATUS_OK;
}

int32_t VL53L0X_comms_close(void)
{
    return VL53L0X_PLATFORM_STATUS_OK;
}

int32_t VL53L0X_write_multi(
    uint8_t address,
    uint8_t index,
    uint8_t *pdata,
    int32_t count)
{
    if ((pdata == NULL) ||
        (count <= 0) ||
        (count > COMMS_BUFFER_SIZE))
    {
        return VL53L0X_PLATFORM_STATUS_FAIL;
    }

    return VL53L0X_ConvertHalStatus(
        HAL_I2C_Mem_Write(
            &hi2c1,
            (uint16_t)address,
            (uint16_t)index,
            I2C_MEMADD_SIZE_8BIT,
            pdata,
            (uint16_t)count,
            VL53L0X_I2C_TIMEOUT_MS));
}

int32_t VL53L0X_read_multi(
    uint8_t address,
    uint8_t index,
    uint8_t *pdata,
    int32_t count)
{
    if ((pdata == NULL) ||
        (count <= 0) ||
        (count > COMMS_BUFFER_SIZE))
    {
        return VL53L0X_PLATFORM_STATUS_FAIL;
    }

    return VL53L0X_ConvertHalStatus(
        HAL_I2C_Mem_Read(
            &hi2c1,
            (uint16_t)address,
            (uint16_t)index,
            I2C_MEMADD_SIZE_8BIT,
            pdata,
            (uint16_t)count,
            VL53L0X_I2C_TIMEOUT_MS));
}

int32_t VL53L0X_write_byte(
    uint8_t address,
    uint8_t index,
    uint8_t data)
{
    return VL53L0X_write_multi(
        address,
        index,
        &data,
        1);
}

int32_t VL53L0X_write_word(
    uint8_t address,
    uint8_t index,
    uint16_t data)
{
    uint8_t buffer[BYTES_PER_WORD];

    /* VL53L0X는 높은 바이트부터 전송 */
    buffer[0] = (uint8_t)(data >> 8);
    buffer[1] = (uint8_t)(data & 0xFFU);

    return VL53L0X_write_multi(
        address,
        index,
        buffer,
        BYTES_PER_WORD);
}

int32_t VL53L0X_write_dword(
    uint8_t address,
    uint8_t index,
    uint32_t data)
{
    uint8_t buffer[BYTES_PER_DWORD];

    buffer[0] = (uint8_t)(data >> 24);
    buffer[1] = (uint8_t)(data >> 16);
    buffer[2] = (uint8_t)(data >> 8);
    buffer[3] = (uint8_t)data;

    return VL53L0X_write_multi(
        address,
        index,
        buffer,
        BYTES_PER_DWORD);
}

int32_t VL53L0X_read_byte(
    uint8_t address,
    uint8_t index,
    uint8_t *pdata)
{
    return VL53L0X_read_multi(
        address,
        index,
        pdata,
        1);
}

int32_t VL53L0X_read_word(
    uint8_t address,
    uint8_t index,
    uint16_t *pdata)
{
    uint8_t buffer[BYTES_PER_WORD];
    int32_t status;

    if (pdata == NULL)
    {
        return VL53L0X_PLATFORM_STATUS_FAIL;
    }

    status = VL53L0X_read_multi(
        address,
        index,
        buffer,
        BYTES_PER_WORD);

    if (status == VL53L0X_PLATFORM_STATUS_OK)
    {
        *pdata =
            ((uint16_t)buffer[0] << 8) |
            (uint16_t)buffer[1];
    }

    return status;
}

int32_t VL53L0X_read_dword(
    uint8_t address,
    uint8_t index,
    uint32_t *pdata)
{
    uint8_t buffer[BYTES_PER_DWORD];
    int32_t status;

    if (pdata == NULL)
    {
        return VL53L0X_PLATFORM_STATUS_FAIL;
    }

    status = VL53L0X_read_multi(
        address,
        index,
        buffer,
        BYTES_PER_DWORD);

    if (status == VL53L0X_PLATFORM_STATUS_OK)
    {
        *pdata =
            ((uint32_t)buffer[0] << 24) |
            ((uint32_t)buffer[1] << 16) |
            ((uint32_t)buffer[2] << 8) |
            (uint32_t)buffer[3];
    }

    return status;
}

int32_t VL53L0X_platform_wait_us(
    int32_t wait_us)
{
    uint32_t wait_ms;

    if (wait_us <= 0)
    {
        return VL53L0X_PLATFORM_STATUS_OK;
    }

    /*
     * 현재는 마이크로초를 밀리초 단위로
     * 올림하여 안전하게 기다림
     */
    wait_ms =
        ((uint32_t)wait_us + 999U) / 1000U;

    HAL_Delay(wait_ms);

    return VL53L0X_PLATFORM_STATUS_OK;
}

int32_t VL53L0X_wait_ms(
    int32_t wait_ms)
{
    if (wait_ms > 0)
    {
        HAL_Delay((uint32_t)wait_ms);
    }

    return VL53L0X_PLATFORM_STATUS_OK;
}

int32_t VL53L0X_set_gpio(
    uint8_t level)
{
    /*
     * 현재 XSHUT 핀을 연결하지 않았으므로
     * 지원하지 않는 기능
     */
    (void)level;

    return VL53L0X_PLATFORM_STATUS_FAIL;
}

int32_t VL53L0X_get_gpio(
    uint8_t *plevel)
{
    if (plevel != NULL)
    {
        *plevel = 0U;
    }

    return VL53L0X_PLATFORM_STATUS_FAIL;
}

int32_t VL53L0X_release_gpio(void)
{
    return VL53L0X_PLATFORM_STATUS_FAIL;
}

int32_t VL53L0X_cycle_power(void)
{
    /*
     * 센서 전원 제어 회로가 없으므로
     * 소프트웨어 전원 재시작은 지원하지 않음
     */
    return VL53L0X_PLATFORM_STATUS_FAIL;
}

int32_t VL53L0X_get_timer_frequency(
    int32_t *ptimer_freq_hz)
{
    if (ptimer_freq_hz == NULL)
    {
        return VL53L0X_PLATFORM_STATUS_FAIL;
    }

    /* HAL_GetTick은 1kHz, 즉 1ms 단위 */
    *ptimer_freq_hz = 1000;

    return VL53L0X_PLATFORM_STATUS_OK;
}

int32_t VL53L0X_get_timer_value(
    int32_t *ptimer_count)
{
    if (ptimer_count == NULL)
    {
        return VL53L0X_PLATFORM_STATUS_FAIL;
    }

    *ptimer_count =
        (int32_t)HAL_GetTick();

    return VL53L0X_PLATFORM_STATUS_OK;
}