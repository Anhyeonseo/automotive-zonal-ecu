# Test Case List

## 목적

이 문서는 STM32 Zonal ECU Diagnostic System의 Test Case 목록을 관리한다.

각 Test Case는 Requirement와 연결되며, 검증 완료 후 Validation Evidence를 남긴다.

---

## Test Case 형식

```text
Test Case ID:
Linked Requirement:
Purpose:
Precondition:
Test Step:
Expected Result:
Actual Result:
Result:
Evidence:
```

---

## Bring-up Test Cases

| Test Case ID | Linked Requirement | 목적 | 상태 |
|---|---|---|---|
| BRINGUP-001 | REQ-SYS-001 | ST-LINK 연결 및 Debug 진입 확인 | TBD |
| TC-IO-001 | REQ-SW-IO-001 | LED 출력 확인 | TBD |
| TC-IO-002 | REQ-SW-IO-002 | Button 입력 확인 | TBD |
| TC-IO-003 | REQ-SW-IO-003 | Button interrupt 확인 | TBD |

---

## Sensor Test Cases

| Test Case ID | Linked Requirement | 목적 | 상태 |
|---|---|---|---|
| TC-ADC-001 | REQ-SW-SEN-004 | ADC Virtual Pedal 값 읽기 | TBD |
| TC-I2C-001 | REQ-SW-SEN-002 | I2C Scan 확인 | TBD |
| TC-DISP-001 | REQ-SW-ACT-001 | OLED 출력 확인 | TBD |
| TC-IMU-001 | REQ-SW-SEN-005 | MPU6050 WHO_AM_I 확인 | TBD |
| TC-IMU-002 | REQ-SW-SEN-005 | MPU6050 Accel/Gyro 값 읽기 | TBD |
| TC-DIST-001 | REQ-SW-SEN-003 | VL53L0X 거리값 읽기 | TBD |

---

## State Machine Test Cases

| Test Case ID | Linked Requirement | 목적 | 상태 |
|---|---|---|---|
| TC-STA-001 | REQ-SW-STA-001 | INIT → SELF_TEST 전이 | TBD |
| TC-STA-002 | REQ-SW-STA-001 | SELF_TEST → NORMAL 전이 | TBD |
| TC-STA-003 | REQ-SW-STA-002 | NORMAL → WARNING 전이 | TBD |
| TC-STA-004 | REQ-SW-STA-002 | WARNING → EMERGENCY 전이 | TBD |
| TC-STA-005 | REQ-SW-STA-003 | DEGRADED 상태 진입 | TBD |

---

## Diagnostic Test Cases

| Test Case ID | Linked Requirement | 목적 | 상태 |
|---|---|---|---|
| TC-DTC-001 | REQ-SW-DIAG-001 | Fault 발생 시 DTC PENDING | TBD |
| TC-DTC-002 | REQ-SW-DIAG-001 | 반복 Fault 발생 시 DTC CONFIRMED | TBD |
| TC-DTC-003 | REQ-SW-DIAG-002 | clear_dtc 명령 확인 | TBD |
| TC-DCM-001 | REQ-SW-DIAG-003 | read_state 명령 확인 | TBD |
| TC-DCM-002 | REQ-SW-DIAG-004 | read_sensor 명령 확인 | TBD |
| TC-DCM-003 | REQ-SW-DIAG-005 | read_dtc 명령 확인 | TBD |
| TC-DCM-004 | REQ-SW-DIAG-006 | clear_dtc 명령 확인 | TBD |

---

## Configuration Test Cases

| Test Case ID | Linked Requirement | 목적 | 상태 |
|---|---|---|---|
| TC-CFG-001 | REQ-SW-CFG-001 | 기본 Config 읽기 | TBD |
| TC-CFG-002 | REQ-SW-CFG-002 | Threshold 변경 | TBD |
| TC-CFG-003 | REQ-SW-CFG-003 | save_config 후 재부팅 유지 | TBD |
| TC-CFG-004 | REQ-SW-CFG-004 | CRC mismatch 감지 | TBD |

---

## Watchdog / Timing Test Cases

| Test Case ID | Linked Requirement | 목적 | 상태 |
|---|---|---|---|
| TC-TIM-001 | REQ-SW-TIM-001 | 10 ms 주기 실행 확인 | TBD |
| TC-TIM-002 | REQ-SW-TIM-002 | 20 ms StateManager 주기 확인 | TBD |
| TC-WDG-001 | REQ-SW-WDG-001 | Task Alive 정상 갱신 확인 | TBD |
| TC-WDG-002 | REQ-SW-WDG-002 | Alive timeout 감지 | TBD |
| TC-WDG-003 | REQ-SW-WDG-003 | Task overrun Fault Injection | TBD |

---

## Safety / Fault Injection Test Cases

| Test Case ID | Linked Requirement | 목적 | 상태 |
|---|---|---|---|
| TC-FLT-001 | REQ-SW-SAF-003 | distance_timeout Fault Injection | TBD |
| TC-FLT-002 | REQ-SW-SAF-003 | imu_comm_fail Fault Injection | TBD |
| TC-FLT-003 | REQ-SW-SAF-003 | sensor_out_of_range Fault Injection | TBD |
| TC-SAF-001 | REQ-SW-SAF-001 | Emergency Button 입력 후 FAIL_SAFE 전이 | TBD |
| TC-SAF-002 | REQ-SW-SAF-002 | Sensor timeout 후 안전 반응 확인 | TBD |
| TC-SAF-003 | REQ-SW-SAF-002 | Task alive timeout 후 FAIL_SAFE 전이 | TBD |
| TC-SAF-004 | REQ-SW-SAF-004 | Recovery 조건 확인 | TBD |
| TC-SAF-005 | REQ-SW-SAF-005 | FAIL_SAFE 출력 pattern 확인 | TBD |

---

## PC Tool Test Cases

| Test Case ID | Linked Requirement | 목적 | 상태 |
|---|---|---|---|
| TC-PC-001 | REQ-SYS-002 | Serial 연결 확인 | TBD |
| TC-PC-002 | REQ-SYS-002 | PC Tool 상태 조회 | TBD |
| TC-PC-003 | REQ-SYS-002 | PC Tool DTC 조회 | TBD |
| TC-PC-004 | REQ-SYS-008 | CSV log 저장 | TBD |
| TC-PC-005 | REQ-SYS-008 | Validation Report 생성 | TBD |

---

## 상태 값 정의

| 상태 | 의미 |
|---|---|
| TBD | 아직 수행 전 |
| PASS | 기대 결과 만족 |
| FAIL | 기대 결과 불만족 |
| PARTIAL | 일부만 확인됨 |
| BLOCKED | 외부 조건 때문에 수행 불가 |
