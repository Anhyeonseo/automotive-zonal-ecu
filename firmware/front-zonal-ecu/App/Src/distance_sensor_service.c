#include "distance_sensor_service.h"

#include "i2c.h"
#include "vl53l0x_api.h"

#include <string.h>

#define DISTANCE_SENSOR_I2C_ADDRESS          (0x29U << 1)
#define DISTANCE_SENSOR_I2C_TRIALS           3U
#define DISTANCE_SENSOR_I2C_TIMEOUT_MS       100U
#define DISTANCE_SENSOR_BOOT_DELAY_MS        10U
#define DISTANCE_SENSOR_PERIOD_MS            100U
#define DISTANCE_SENSOR_MEASUREMENT_TIMEOUT_MS 100U
#define DISTANCE_SENSOR_TIMING_BUDGET_US     33000U

static DistanceSensorStatus s_status;
static VL53L0X_Dev_t s_device;
static VL53L0X_RangingMeasurementData_t s_measurement;

static uint8_t s_vhv_settings;
static uint8_t s_phase_calibration;
static uint8_t s_is_aperture_spads;
static uint32_t s_reference_spad_count;

static uint32_t s_last_measurement_tick;
static uint32_t s_measurement_start_tick;

static void UpdateMeasurementResult(void)
{
    s_status.distance_mm =
        s_measurement.RangeMilliMeter;
    s_status.range_status =
        s_measurement.RangeStatus;
    s_status.measurement_valid =
        (s_status.range_status == 0U) ? 1U : 0U;
}

void DistanceSensorService_Init(void)
{
    memset(&s_status, 0, sizeof(s_status));
    memset(&s_device, 0, sizeof(s_device));
    memset(&s_measurement, 0, sizeof(s_measurement));

    s_status.range_status = 255U;

    HAL_Delay(DISTANCE_SENSOR_BOOT_DELAY_MS);

    if (HAL_I2C_IsDeviceReady(
            &hi2c1,
            DISTANCE_SENSOR_I2C_ADDRESS,
            DISTANCE_SENSOR_I2C_TRIALS,
            DISTANCE_SENSOR_I2C_TIMEOUT_MS) == HAL_OK)
    {
        s_status.i2c_ready = 1U;
        s_status.i2c_error = HAL_I2C_ERROR_NONE;
    }
    else
    {
        s_status.i2c_ready = 0U;
        s_status.i2c_error = HAL_I2C_GetError(&hi2c1);
    }

    if (s_status.i2c_ready != 0U)
    {
        s_device.I2cDevAddr =
            (uint8_t)DISTANCE_SENSOR_I2C_ADDRESS;
        s_device.comms_type = I2C;
        s_device.comms_speed_khz = 100U;

        s_status.init_step = 1U;
        s_status.api_status =
            VL53L0X_DataInit(&s_device);

        if (s_status.api_status == VL53L0X_ERROR_NONE)
        {
            s_status.init_step = 2U;
            s_status.api_status =
                VL53L0X_StaticInit(&s_device);
        }

        if (s_status.api_status == VL53L0X_ERROR_NONE)
        {
            s_status.init_step = 3U;
            s_status.api_status =
                VL53L0X_PerformRefCalibration(
                    &s_device,
                    &s_vhv_settings,
                    &s_phase_calibration);
        }

        if (s_status.api_status == VL53L0X_ERROR_NONE)
        {
            s_status.init_step = 4U;
            s_status.api_status =
                VL53L0X_PerformRefSpadManagement(
                    &s_device,
                    &s_reference_spad_count,
                    &s_is_aperture_spads);
        }

        if (s_status.api_status == VL53L0X_ERROR_NONE)
        {
            s_status.init_step = 5U;
            s_status.api_status =
                VL53L0X_SetDeviceMode(
                    &s_device,
                    VL53L0X_DEVICEMODE_SINGLE_RANGING);
        }

        if (s_status.api_status == VL53L0X_ERROR_NONE)
        {
            s_status.init_step = 6U;
            s_status.api_status =
                VL53L0X_SetMeasurementTimingBudgetMicroSeconds(
                    &s_device,
                    DISTANCE_SENSOR_TIMING_BUDGET_US);
        }

        if (s_status.api_status == VL53L0X_ERROR_NONE)
        {
            s_status.init_step = 7U;
            s_status.api_status =
                VL53L0X_PerformSingleRangingMeasurement(
                    &s_device,
                    &s_measurement);
        }

        if (s_status.api_status == VL53L0X_ERROR_NONE)
        {
            UpdateMeasurementResult();
            s_status.api_ready = 1U;
            s_status.init_step = 8U;
        }
    }

    s_last_measurement_tick = HAL_GetTick();
}

void DistanceSensorService_Process(uint32_t current_time_ms)
{
    if (s_status.api_ready == 0U)
    {
        return;
    }

    if (s_status.measurement_state == 0U)
    {
        if ((current_time_ms - s_last_measurement_tick) >=
            DISTANCE_SENSOR_PERIOD_MS)
        {
            s_last_measurement_tick = current_time_ms;
            s_measurement_start_tick = current_time_ms;
            s_status.data_ready = 0U;

            s_status.api_status =
                VL53L0X_StartMeasurement(&s_device);

            if (s_status.api_status == VL53L0X_ERROR_NONE)
            {
                s_status.measurement_state = 1U;
            }
            else
            {
                s_status.measurement_valid = 0U;
                s_status.measurement_error_count++;
            }
        }

        return;
    }

    s_status.api_status =
        VL53L0X_GetMeasurementDataReady(
            &s_device,
            &s_status.data_ready);

    if (s_status.api_status != VL53L0X_ERROR_NONE)
    {
        s_status.measurement_valid = 0U;
        s_status.measurement_error_count++;
        s_status.measurement_state = 0U;
    }
    else if (s_status.data_ready != 0U)
    {
        s_status.api_status =
            VL53L0X_GetRangingMeasurementData(
                &s_device,
                &s_measurement);

        if (s_status.api_status == VL53L0X_ERROR_NONE)
        {
            s_status.api_status =
                VL53L0X_ClearInterruptMask(&s_device, 0U);
        }

        if (s_status.api_status == VL53L0X_ERROR_NONE)
        {
            UpdateMeasurementResult();
            s_status.measurement_count++;
        }
        else
        {
            s_status.measurement_valid = 0U;
            s_status.measurement_error_count++;
        }

        s_status.measurement_state = 0U;
    }
    else if ((current_time_ms - s_measurement_start_tick) >=
             DISTANCE_SENSOR_MEASUREMENT_TIMEOUT_MS)
    {
        (void)VL53L0X_StopMeasurement(&s_device);
        s_status.api_status = VL53L0X_ERROR_TIME_OUT;
        s_status.measurement_valid = 0U;
        s_status.measurement_timeout_count++;
        s_status.measurement_error_count++;
        s_status.measurement_state = 0U;
    }
}

void DistanceSensorService_GetSnapshot(
    DistanceSensorStatus *snapshot)
{
    if (snapshot != 0)
    {
        *snapshot = s_status;
    }
}
