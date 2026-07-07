# Verification Plan

## 목적

이 문서는 STM32 Zonal ECU Diagnostic System의 검증 전략을 정의한다.

목표는 단순히 기능이 동작하는지 확인하는 것이 아니라, 요구사항과 테스트 케이스, 검증 증거를 연결해 자동차 SW 개발 방식에 가까운 Evidence를 남기는 것이다.

---

## 검증 원칙

각 주요 기능은 다음 항목을 가져야 한다.

```text
Requirement ID
Test Case ID
Expected Result
Actual Result
PASS/FAIL
Evidence File
```

---

## 검증 단계

### 1. Bring-up Verification

목적:

- STM32 보드, Debugger, 기본 Peripheral이 정상 동작하는지 확인한다.

예시:

| Test Case ID | 내용 |
|---|---|
| BRINGUP-001 | ST-LINK 연결 및 Debug 진입 확인 |
| TC-IO-001 | LED 출력 확인 |
| TC-IO-002 | Button 입력 확인 |

---

### 2. Module Verification

목적:

- 개별 모듈을 통합 전에 검증한다.

예시:

| Test Case ID | 내용 |
|---|---|
| TC-SEN-001 | ADC Virtual Pedal 읽기 |
| TC-I2C-001 | I2C Scan |
| TC-DISP-001 | OLED 출력 |
| TC-IMU-001 | MPU6050 WHO_AM_I 확인 |
| TC-DIST-001 | VL53L0X 거리값 읽기 |

---

### 3. Integration Verification

목적:

- 센서, StateManager, Diagnostic, Actuator 출력이 함께 동작하는지 검증한다.

예시:

| Test Case ID | 내용 |
|---|---|
| TC-STA-001 | INIT → SELF_TEST 전이 |
| TC-STA-002 | SELF_TEST → NORMAL 전이 |
| TC-STA-003 | NORMAL → WARNING 전이 |
| TC-STA-004 | WARNING → EMERGENCY 전이 |
| TC-ACT-001 | ECU State별 LED/Buzzer/OLED 출력 확인 |

---

### 4. Diagnostic Verification

목적:

- DTC 감지, DTC 상태 전이, DTC 조회/삭제를 검증한다.

예시:

| Test Case ID | 내용 |
|---|---|
| TC-DTC-001 | Fault 발생 시 DTC PENDING |
| TC-DTC-002 | 반복 Fault 발생 시 DTC CONFIRMED |
| TC-DTC-003 | clear_dtc 명령 확인 |
| TC-DTC-004 | Fault 해소 시 HEALED 후보 처리 |

---

### 5. Safety-inspired Verification

목적:

- Fault Injection, Fail-safe 전이, Recovery Logic을 검증한다.

예시:

| Test Case ID | 내용 |
|---|---|
| TC-SAF-001 | Emergency Button 입력 후 50 ms 이내 FAIL_SAFE 전이 |
| TC-SAF-002 | Sensor timeout 시 DEGRADED 또는 FAIL_SAFE 전이 |
| TC-SAF-003 | Task alive timeout 시 FAIL_SAFE 전이 |
| TC-SAF-004 | Fault 해소 후 Recovery 진입 |

---

### 6. PC Validation Tool Verification

목적:

- PC Tool이 테스트 케이스를 실행하고 로그와 리포트를 생성하는지 확인한다.

예시:

| Test Case ID | 내용 |
|---|---|
| TC-PC-001 | Serial 연결 확인 |
| TC-PC-002 | PC Tool에서 DTC 조회 |
| TC-PC-003 | PC Tool에서 Fault Injection 수행 |
| TC-PC-004 | CSV 로그 생성 |
| TC-PC-005 | Validation Report 생성 |

---

## Evidence 종류

검증 증거는 다음 형태로 저장할 수 있다.

- UART log
- CSV log
- Validation Report markdown file
- Logic Analyzer capture
- Screenshot
- Demo video

---

## PASS/FAIL 기준

Test Case는 다음 조건을 만족할 때 PASS로 판단한다.

- Expected Result가 관찰된다.
- 정량 기준이 정의된 경우 측정값이 기준을 만족한다.
- Evidence file이 저장된다.
- Traceability Matrix가 업데이트된다.

하나라도 부족하면 PARTIAL 또는 FAIL로 남긴다.

---

## Validation Report 파일명 규칙

```text
docs/validation_reports/TC-SAF-001.md
pc_tool/logs/TC-SAF-001_YYYY-MM-DD_HH-MM-SS.csv
```

---

## Traceability 관리 원칙

검증이 완료될 때마다 다음 문서를 업데이트한다.

- test_case_list.md
- traceability_matrix.md
- validation_reports/
