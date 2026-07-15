#ifndef DISTANCE_SENSOR_SERVICE_H
#define DISTANCE_SENSOR_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    uint8_t i2c_ready;
    uint32_t i2c_error;
    uint8_t api_ready;
    uint8_t init_step;
    int32_t api_status;

    uint16_t distance_mm;
    uint8_t range_status;
    uint8_t measurement_valid;

    uint8_t measurement_state;
    uint8_t data_ready;
    uint32_t measurement_count;
    uint32_t measurement_error_count;
    uint32_t measurement_timeout_count;
} DistanceSensorStatus;

void DistanceSensorService_Init(void);
void DistanceSensorService_Process(uint32_t current_time_ms);
void DistanceSensorService_GetSnapshot(
    DistanceSensorStatus *snapshot);

#ifdef __cplusplus
}
#endif

#endif /* DISTANCE_SENSOR_SERVICE_H */
