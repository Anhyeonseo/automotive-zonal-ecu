# Fault Scenarios

## 목적

이 문서는 STM32 Zonal ECU Diagnostic System에서 검증할 Fault Scenario를 정의한다.

이 프로젝트는 ISO 26262 compliant 시스템이 아니다. 다만 기능안전의 결함 감지, Fault Injection, Safe State 전이 개념을 교육용으로 축소 구현한다.

---

## Fault 분류

| 분류 | 설명 |
|---|---|
| Sensor Fault | 센서 통신 실패, timeout, 값 범위 이상 |
| Timing Fault | Task overrun, Alive timeout |
| Configuration Fault | Flash 저장값 손상, CRC mismatch |
| User-triggered Fault | Emergency Button 입력 |
| Communication Fault | UART command 오류, 응답 timeout |

---

## Fault Scenario 목록

### FLT-001. Distance Sensor Timeout

설명:

- VL53L0X 거리 센서가 일정 시간 내 유효 데이터를 반환하지 않는 상황

감지 조건:

```text
VL53L0X read timeout count >= threshold
```

예상 반응:

- `P1001_DISTANCE_TIMEOUT` DTC 발생
- 상태는 DEGRADED 또는 FAIL_SAFE 후보
- PC Tool에서 DTC 조회 가능

관련 Test Case:

- TC-FLT-001
- TC-DTC-001

---

### FLT-002. IMU Communication Fail

설명:

- MPU6050 I2C 통신 실패 또는 WHO_AM_I 응답 실패

감지 조건:

```text
MPU6050 WHO_AM_I mismatch
or I2C read failure count >= threshold
```

예상 반응:

- `P1002_IMU_COMM_FAIL` DTC 발생
- 충격 감지 기능 제한
- 상태는 DEGRADED 후보

관련 Test Case:

- TC-FLT-002
- TC-DTC-002

---

### FLT-003. Control Loop Overrun

설명:

- 정해진 주기 내 제어 루프 또는 Task 실행이 완료되지 않는 상황

감지 조건:

```text
Task execution time > assigned period
or alive counter not updated within timeout
```

예상 반응:

- `P1003_CONTROL_LOOP_OVERRUN` DTC 발생
- Critical 조건이면 FAIL_SAFE 전이

관련 Test Case:

- TC-WDG-003
- TC-SAF-003

---

### FLT-004. Emergency Stop

설명:

- 사용자가 Emergency Button을 누르는 상황

감지 조건:

```text
Emergency Button input active
```

예상 반응:

- `P1004_EMERGENCY_STOP` DTC 발생
- 50 ms 이내 FAIL_SAFE 전이
- Fail-safe LED/Buzzer pattern 출력

관련 Test Case:

- TC-SAF-001

---

### FLT-005. Configuration CRC Mismatch

설명:

- Flash에 저장된 Configuration의 CRC가 맞지 않는 상황

감지 조건:

```text
Calculated CRC != stored CRC
```

예상 반응:

- `P1005_CONFIG_CRC_MISMATCH` DTC 발생
- 기본 설정값 복구
- 복구 실패 시 FAIL_SAFE 후보

관련 Test Case:

- TC-CFG-004

---

### FLT-006. Sensor Value Out of Range

설명:

- ADC 또는 센서값이 정의된 정상 범위를 벗어나는 상황

감지 조건:

```text
pedal_percent < 0 or pedal_percent > 100
or distance_mm invalid
or acceleration value exceeds plausible range
```

예상 반응:

- `P1006_SENSOR_VALUE_OUT_OF_RANGE` DTC 발생
- 상태는 WARNING, DEGRADED, 또는 FAIL_SAFE 후보

관련 Test Case:

- TC-FLT-006

---

## Fault Injection 방법

Fault Injection은 PC Diagnostic Command 또는 버튼 입력으로 수행한다.

지원 명령:

```text
inject_fault distance_timeout
inject_fault imu_comm_fail
inject_fault sensor_out_of_range
inject_fault task_overrun
inject_fault config_crc_mismatch
inject_fault emergency_stop
```

---

## Fault 검증 기준

Fault Scenario 검증 시 다음 항목을 확인한다.

- Fault 감지 여부
- DTC 발생 여부
- DTC 상태 전이 여부
- ECU State 전이 여부
- Fail-safe 진입 시간
- LED/OLED/Buzzer 출력
- PC Tool 표시
- CSV 로그 저장
- Validation Report 생성

---

## Fault Severity 초안

| Fault | Severity | 예상 상태 |
|---|---|---|
| Distance Timeout | Medium | DEGRADED 또는 FAIL_SAFE |
| IMU Communication Fail | Medium | DEGRADED |
| Control Loop Overrun | High | FAIL_SAFE |
| Emergency Stop | High | FAIL_SAFE |
| Config CRC Mismatch | Medium/High | RECOVERY 또는 FAIL_SAFE |
| Sensor Value Out of Range | Medium | WARNING 또는 DEGRADED |
