# Diagnostic Protocol

## 목적

이 문서는 PC Tool과 STM32 ECU 사이의 UART 기반 Diagnostic Protocol을 정의한다.

초기 구현은 사람이 읽기 쉬운 CLI 형식으로 시작한다. 이후 필요하면 UDS-inspired command structure로 확장한다.

이 프로젝트는 UDS-compatible 구현이 아니다. 정확한 표현은 **UDS-inspired lightweight diagnostic protocol over UART**이다.

---

## 통신 개요

| 항목 | 내용 |
|---|---|
| 물리 통신 | UART |
| 초기 Baudrate | 115200 bps |
| 데이터 형식 | ASCII command/response |
| Line ending | `\n` |
| Host | PC Diagnostic & Validation Tool |
| Target | STM32 Front Zonal ECU Prototype |

---

## 기본 메시지 형식

### Command

```text
<command> [argument1] [argument2] ...\n
```

예시:

```text
read_state
read_sensor
read_dtc
clear_dtc all
inject_fault imu_timeout
set_threshold warning_distance 600
save_config
```

### Response

```text
OK,<response_payload>\n
ERR,<error_code>,<message>\n
```

예시:

```text
OK,STATE,NORMAL
OK,SENSOR,DIST=650,PEDAL=35,ACCEL_X=0.01,ACCEL_Y=0.02,ACCEL_Z=1.00
OK,DTC,P1002,CONFIRMED,COUNT=3
ERR,INVALID_COMMAND,unknown command
```

---

## 명령어 목록

### read_state

```text
read_state
```

역할:

- 현재 ECU State를 조회한다.

응답 예시:

```text
OK,STATE,NORMAL
```

관련 Requirement:

- REQ-SW-DIAG-003

관련 Test Case:

- TC-DCM-001

---

### read_sensor

```text
read_sensor
```

역할:

- 현재 센서값을 조회한다.

응답 예시:

```text
OK,SENSOR,DIST=650,PEDAL=35,ACCEL_X=0.01,ACCEL_Y=0.02,ACCEL_Z=1.00
```

관련 Requirement:

- REQ-SW-DIAG-004

관련 Test Case:

- TC-DCM-002

---

### read_dtc

```text
read_dtc
```

역할:

- 현재 저장된 DTC 목록과 상태를 조회한다.

응답 예시:

```text
OK,DTC,P1001_DISTANCE_TIMEOUT,PENDING,COUNT=1
OK,DTC,P1002_IMU_COMM_FAIL,CONFIRMED,COUNT=3
OK,DTC,NONE
```

관련 Requirement:

- REQ-SW-DIAG-005

관련 Test Case:

- TC-DCM-003

---

### clear_dtc

```text
clear_dtc all
clear_dtc P1002
```

역할:

- 전체 또는 특정 DTC 삭제를 요청한다.

응답 예시:

```text
OK,DTC_CLEARED,ALL
OK,DTC_CLEARED,P1002
```

관련 Requirement:

- REQ-SW-DIAG-006

관련 Test Case:

- TC-DCM-004

---

### inject_fault

```text
inject_fault <fault_name>
```

지원 Fault 이름:

```text
distance_timeout
imu_comm_fail
sensor_out_of_range
task_overrun
config_crc_mismatch
emergency_stop
```

역할:

- 테스트를 위해 Fault를 인위적으로 주입한다.

응답 예시:

```text
OK,FAULT_INJECTED,imu_comm_fail
```

관련 Requirement:

- REQ-SW-SAF-003

관련 Test Case:

- TC-FLT-001

---

### set_threshold

```text
set_threshold warning_distance 600
set_threshold emergency_distance 250
set_threshold shock_g 2.5
```

역할:

- Runtime Threshold 값을 변경한다.

응답 예시:

```text
OK,THRESHOLD,warning_distance=600
```

관련 Requirement:

- REQ-SW-CFG-002

관련 Test Case:

- TC-CFG-002

---

### save_config

```text
save_config
```

역할:

- 현재 Runtime Configuration을 Flash에 저장한다.

응답 예시:

```text
OK,CONFIG_SAVED
```

관련 Requirement:

- REQ-SW-CFG-003

관련 Test Case:

- TC-CFG-003

---

### reset_ecu

```text
reset_ecu
```

역할:

- ECU reset을 요청한다.

응답 예시:

```text
OK,RESET_REQUESTED
```

관련 Requirement:

- REQ-SW-DIAG-007

관련 Test Case:

- TC-DCM-007

---

### start_recovery

```text
start_recovery
```

역할:

- Fault 조건이 해소된 뒤 Recovery 절차를 요청한다.

응답 예시:

```text
OK,RECOVERY_STARTED
```

관련 Requirement:

- REQ-SW-SAF-004

관련 Test Case:

- TC-SAF-004

---

## Error Code

| Error Code | 의미 |
|---|---|
| INVALID_COMMAND | 정의되지 않은 명령 |
| INVALID_ARGUMENT | 인자 형식 오류 |
| OUT_OF_RANGE | 설정값 범위 초과 |
| NOT_ALLOWED_IN_STATE | 현재 ECU State에서 허용되지 않는 명령 |
| INTERNAL_ERROR | 내부 처리 오류 |

---

## 상태별 명령 허용 정책

| Command | NORMAL | WARNING | EMERGENCY | FAIL_SAFE |
|---|---:|---:|---:|---:|
| read_state | 허용 | 허용 | 허용 | 허용 |
| read_sensor | 허용 | 허용 | 허용 | 허용 |
| read_dtc | 허용 | 허용 | 허용 | 허용 |
| clear_dtc | 허용 | 허용 | 제한 | 제한 |
| inject_fault | 허용 | 허용 | 제한 | 제한 |
| set_threshold | 허용 | 허용 | 제한 | 제한 |
| save_config | 허용 | 허용 | 제한 | 제한 |
| start_recovery | 제한 | 제한 | 제한 | 허용 조건부 |

---

## 향후 UDS-inspired 확장 후보

| UDS-inspired Service | 의미 |
|---|---|
| 0x10 | Diagnostic Session Control |
| 0x19 | Read DTC Information |
| 0x14 | Clear Diagnostic Information |
| 0x22 | Read Data By Identifier |
| 0x2E | Write Data By Identifier |
| 0x31 | Routine Control |
| 0x11 | ECU Reset |

주의: 이 프로젝트는 UDS-compatible 구현이 아니다.
