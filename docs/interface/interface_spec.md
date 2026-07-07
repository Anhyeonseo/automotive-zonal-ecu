# Interface Specification

## 목적

이 문서는 STM32 Zonal ECU Diagnostic System의 주요 모듈 간 Interface를 정의한다.

이 프로젝트에서는 실제 AUTOSAR RTE를 구현하지 않지만, Application Layer가 하위 Driver나 STM32 HAL에 직접 의존하지 않도록 **RTE-like Interface**를 사용한다.

---

## Interface 설계 원칙

- Application Layer는 STM32 HAL API를 직접 호출하지 않는다.
- Application Layer는 센서값, 출력 명령, 진단 서비스, 설정값에 접근할 때 RTE-like Interface를 사용한다.
- Interface는 데이터 타입, 입력, 출력, 반환값, 오류 조건을 명확히 정의한다.
- 각 Interface는 관련 Requirement 및 Test Case와 연결한다.

---

## 주요 데이터 타입

### EcuState

```c
typedef enum
{
    ECU_STATE_INIT = 0,
    ECU_STATE_SELF_TEST,
    ECU_STATE_NORMAL,
    ECU_STATE_WARNING,
    ECU_STATE_EMERGENCY,
    ECU_STATE_DEGRADED,
    ECU_STATE_FAIL_SAFE,
    ECU_STATE_RECOVERY
} EcuState_t;
```

### SensorData

```c
typedef struct
{
    uint32_t timestampMs;
    uint16_t distanceMm;
    float accelX_g;
    float accelY_g;
    float accelZ_g;
    uint8_t pedalPercent;
    bool distanceValid;
    bool imuValid;
    bool pedalValid;
} SensorData_t;
```

### DtcCode

```c
typedef enum
{
    DTC_NONE = 0,
    DTC_DISTANCE_TIMEOUT,
    DTC_IMU_COMM_FAIL,
    DTC_CONTROL_LOOP_OVERRUN,
    DTC_EMERGENCY_STOP,
    DTC_CONFIG_CRC_MISMATCH,
    DTC_SENSOR_VALUE_OUT_OF_RANGE
} DtcCode_t;
```

### DtcStatus

```c
typedef enum
{
    DTC_STATUS_PASSED = 0,
    DTC_STATUS_FAILED,
    DTC_STATUS_PENDING,
    DTC_STATUS_CONFIRMED,
    DTC_STATUS_HEALED,
    DTC_STATUS_CLEARED
} DtcStatus_t;
```

### ConfigData

```c
typedef struct
{
    uint32_t magic;
    uint16_t configVersion;
    uint16_t warningDistanceMm;
    uint16_t emergencyDistanceMm;
    float shockThresholdG;
    uint32_t crc32;
} ConfigData_t;
```

---

## RTE-like Sensor Interface

### Rte_Read_SensorData

```c
Std_ReturnType Rte_Read_SensorData(SensorData_t* sensorData);
```

역할:

- 최신 센서 데이터 전체를 읽는다.
- StateManager와 SafetyManager가 센서값을 판단할 때 사용한다.

입력:

- `sensorData`: 센서 데이터가 저장될 포인터

출력:

- `SensorData_t`

반환:

- `E_OK`: 정상 읽기
- `E_NOT_OK`: 데이터 유효하지 않음

관련 Requirement:

- REQ-SW-SEN-001
- REQ-SW-SEN-002

관련 Test Case:

- TC-SEN-001
- TC-SEN-002

---

### Rte_Read_DistanceMm

```c
Std_ReturnType Rte_Read_DistanceMm(uint16_t* distanceMm);
```

역할:

- VL53L0X 거리 센서 기반 전방 거리값을 mm 단위로 읽는다.

관련 Requirement:

- REQ-SW-SEN-003

관련 Test Case:

- TC-DIST-001

---

### Rte_Read_PedalPercent

```c
Std_ReturnType Rte_Read_PedalPercent(uint8_t* pedalPercent);
```

역할:

- ADC 기반 Virtual Pedal 입력값을 0~100% 범위로 읽는다.

관련 Requirement:

- REQ-SW-SEN-004

관련 Test Case:

- TC-ADC-001

---

## RTE-like Actuator Interface

### Rte_Call_Actuator_SetStatePattern

```c
Std_ReturnType Rte_Call_Actuator_SetStatePattern(EcuState_t state);
```

역할:

- ECU 상태에 따라 LED, OLED, Buzzer 출력 패턴을 적용한다.

예시:

| ECU State | LED | Buzzer | OLED |
|---|---|---|---|
| NORMAL | Green or slow blink | Off | 상태/센서값 표시 |
| WARNING | Yellow pattern | Short beep | Warning 표시 |
| EMERGENCY | Red fast pattern | Repeated beep | Emergency 표시 |
| FAIL_SAFE | Red fixed or fail pattern | Fail-safe alert | DTC 표시 |

관련 Requirement:

- REQ-SW-ACT-001

관련 Test Case:

- TC-ACT-001

---

## RTE-like Diagnostic Interface

### Rte_Call_Diagnostic_ReportFault

```c
Std_ReturnType Rte_Call_Diagnostic_ReportFault(DtcCode_t dtcCode);
```

역할:

- Application 또는 Abstraction Layer에서 감지한 Fault를 Dem_Lite에 보고한다.
- DTC 상태 전이는 Dem_Lite가 관리한다.

관련 Requirement:

- REQ-SW-DIAG-001

관련 Test Case:

- TC-DTC-001

---

### Rte_Call_Diagnostic_ClearDtc

```c
Std_ReturnType Rte_Call_Diagnostic_ClearDtc(DtcCode_t dtcCode);
```

역할:

- 지정된 DTC를 삭제 요청한다.
- 실제 DTC 상태 변경은 Dem_Lite 정책에 따른다.

관련 Requirement:

- REQ-SW-DIAG-002

관련 Test Case:

- TC-DTC-003

---

## RTE-like Config Interface

### Rte_Read_Config

```c
Std_ReturnType Rte_Read_Config(ConfigData_t* configData);
```

역할:

- 현재 Runtime Configuration을 읽는다.

관련 Requirement:

- REQ-SW-CFG-001

관련 Test Case:

- TC-CFG-001

---

### Rte_Write_Config

```c
Std_ReturnType Rte_Write_Config(const ConfigData_t* configData);
```

역할:

- PC Tool에서 전달된 설정값을 Runtime Configuration에 반영한다.

관련 Requirement:

- REQ-SW-CFG-002

관련 Test Case:

- TC-CFG-002

---

### Rte_Call_Config_Save

```c
Std_ReturnType Rte_Call_Config_Save(void);
```

역할:

- Runtime Configuration을 Flash에 저장하도록 NvM_Lite에 요청한다.

관련 Requirement:

- REQ-SW-CFG-003

관련 Test Case:

- TC-CFG-003

---

## Interface 검증 기준

Interface 검증은 다음 기준으로 수행한다.

- 잘못된 포인터 입력 시 오류를 반환해야 한다.
- 센서 데이터가 유효하지 않을 경우 `E_NOT_OK`를 반환해야 한다.
- Application Layer에서 HAL API를 직접 호출하지 않아야 한다.
- Interface 호출 결과는 UART log 또는 PC Tool로 확인 가능해야 한다.

---

## 향후 업데이트 항목

- 실제 구현 후 함수명 확정
- `Std_ReturnType` 정의 위치 확정
- SensorData 필드 확정
- Diagnostic Protocol과 DTC Table 연동
- Traceability Matrix 반영
