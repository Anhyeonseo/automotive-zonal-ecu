# Software Requirements

## 목적

이 문서는 STM32 Zonal ECU Diagnostic System의 소프트웨어 요구사항을 정의한다.

요구사항은 실제 AUTOSAR 또는 ISO 26262 인증 요구사항이 아니라, 교육용 차량 ECU 프로토타입에서 구현하고 검증할 소프트웨어 기능을 명확히 정리하기 위한 기준이다.

각 요구사항은 구현 모듈, Test Case, Validation Evidence와 연결될 수 있도록 ID를 가진다.

---

## 요구사항 작성 기준

각 요구사항은 다음 형식을 따른다.

```text
Requirement ID:
Requirement:
Rationale:
Verification Method:
Linked Test Case:
```

요구사항은 모호한 표현을 피하고, 가능한 경우 정량 기준을 포함한다.

예를 들어 다음 표현은 피한다.

```text
빠르게 반응해야 한다.
정상적으로 동작해야 한다.
적절히 경고해야 한다.
```

대신 다음과 같이 작성한다.

```text
Emergency Stop 입력 감지 후 50 ms 이내에 FAIL_SAFE 상태로 전이해야 한다.
센서 통신 timeout이 3회 연속 발생하면 DTC를 CONFIRMED 상태로 전이해야 한다.
```

---

# 1. Sensor Software Requirements

## REQ-SW-SEN-001. Distance Sensor Data Acquisition

Requirement:  
ECU 소프트웨어는 VL53L0X 거리 센서로부터 전방 거리값을 mm 단위로 읽을 수 있어야 한다.

Rationale:  
전방 거리값은 WARNING, EMERGENCY, FAIL_SAFE 상태 판단의 주요 입력으로 사용된다.

Verification Method:  
센서 Bring-up 테스트 및 UART 로그 확인.

Linked Test Case:  
TC-SEN-DIST-001

---

## REQ-SW-SEN-002. IMU Data Acquisition

Requirement:  
ECU 소프트웨어는 MPU6050으로부터 가속도 및 자이로 raw data를 읽고, 가속도 값을 g 단위로 변환할 수 있어야 한다.

Rationale:  
IMU는 차체 움직임, 충격, 급가속, 급감속 상황을 모사하는 입력으로 사용된다.

Verification Method:  
WHO_AM_I 확인, raw data 출력, g 단위 변환값 확인.

Linked Test Case:  
TC-SEN-IMU-001

---

## REQ-SW-SEN-003. Virtual Pedal ADC Input

Requirement:  
ECU 소프트웨어는 ADC raw value를 0~100% 범위의 virtual pedal position으로 변환해야 한다.

Rationale:  
가변저항은 차량의 pedal position sensor 또는 analog sensor 입력을 축소 모사한다.

Verification Method:  
ADC raw value와 변환된 pedal_percent 값을 UART 로그로 확인.

Linked Test Case:  
TC-SEN-ADC-001

---

## REQ-SW-SEN-004. Sensor Timeout Detection

Requirement:  
ECU 소프트웨어는 I2C 기반 센서 통신 timeout을 감지하고, 해당 fault를 Diagnostic Service에 보고해야 한다.

Rationale:  
차량 ECU에서는 센서 통신 실패를 감지하고 진단 이벤트로 관리해야 한다.

Verification Method:  
센서 연결 해제 또는 Fault Injection을 통한 timeout 발생 확인.

Linked Test Case:  
TC-SEN-TIMEOUT-001

---

# 2. State Management Requirements

## REQ-SW-STA-001. ECU State Machine

Requirement:  
ECU 소프트웨어는 다음 상태를 포함하는 ECU State Machine을 구현해야 한다.

```text
INIT
SELF_TEST
NORMAL
WARNING
EMERGENCY
DEGRADED
FAIL_SAFE
RECOVERY
```

Rationale:  
ECU 동작을 명확한 상태 기반으로 관리해야 진단, 안전 반응, 복구 흐름을 체계적으로 구현할 수 있다.

Verification Method:  
상태 전이 테스트 및 UART/OLED 상태 출력 확인.

Linked Test Case:  
TC-STA-001

---

## REQ-SW-STA-002. Distance-based Warning Transition

Requirement:  
거리 센서값이 warningDistanceMm보다 작아지면 ECU 상태는 NORMAL에서 WARNING으로 전이해야 한다.

Rationale:  
전방 물체와의 거리가 가까워졌을 때 경고 상태로 전이하는 차량 ECU 동작을 축소 모사한다.

Verification Method:  
거리 센서값 조작 후 ECU 상태 로그 확인.

Linked Test Case:  
TC-STA-WARNING-001

---

## REQ-SW-STA-003. Distance-based Emergency Transition

Requirement:  
거리 센서값이 emergencyDistanceMm보다 작아지면 ECU 상태는 WARNING 또는 NORMAL에서 EMERGENCY로 전이해야 한다.

Rationale:  
위험 거리에 진입했을 때 더 높은 위험 상태로 전이해야 한다.

Verification Method:  
거리 센서값 조작 후 ECU 상태 로그 및 출력 패턴 확인.

Linked Test Case:  
TC-STA-EMERGENCY-001

---

## REQ-SW-STA-004. Critical Fault State Transition

Requirement:  
Critical fault가 감지되면 ECU는 FAIL_SAFE 상태로 전이해야 한다.

Rationale:  
Functional-safety-inspired 설계에서 critical fault 발생 시 안전 상태로 전이하는 흐름이 필요하다.

Verification Method:  
Fault Injection 및 상태 전이 시간 측정.

Linked Test Case:  
TC-SAF-001

---

# 3. Safety Software Requirements

## REQ-SW-SAF-001. Emergency Stop Reaction Time

Requirement:  
Emergency Stop 버튼 입력이 감지되면 ECU는 50 ms 이내에 FAIL_SAFE 상태로 전이해야 한다.

Rationale:  
비상 입력에 대한 빠른 안전 상태 전이는 ECU safety concept의 핵심 시연 요소이다.

Verification Method:  
Logic Analyzer 또는 timestamp log를 통한 response time 측정.

Linked Test Case:  
TC-SAF-ESTOP-001

---

## REQ-SW-SAF-002. Fail-safe Output Pattern

Requirement:  
FAIL_SAFE 상태에서 ECU는 Fail-safe LED pattern, Buzzer warning, OLED 상태 표시를 활성화해야 한다.

Rationale:  
안전 상태 진입 여부를 로컬 출력 장치로 명확하게 확인할 수 있어야 한다.

Verification Method:  
FAIL_SAFE 진입 후 LED/OLED/Buzzer 출력 확인.

Linked Test Case:  
TC-SAF-OUTPUT-001

---

## REQ-SW-SAF-003. Recovery Condition Check

Requirement:  
ECU는 fault가 해소되지 않은 상태에서 RECOVERY 또는 NORMAL 상태로 복귀하면 안 된다.

Rationale:  
고장이 남아 있는 상태에서 정상 상태로 복귀하면 안전하지 않은 동작이 될 수 있다.

Verification Method:  
Fault 유지 상태에서 recovery command 입력 후 상태 유지 확인.

Linked Test Case:  
TC-SAF-RECOVERY-001

---

# 4. Diagnostic Software Requirements

## REQ-SW-DIAG-001. DTC Event Reporting

Requirement:  
ECU 소프트웨어는 predefined fault condition이 발생하면 Dem_Lite에 diagnostic event를 보고해야 한다.

Rationale:  
Fault detection과 DTC state management를 분리하기 위해 diagnostic event reporting 구조가 필요하다.

Verification Method:  
Fault Injection 후 DTC 상태 확인.

Linked Test Case:  
TC-DTC-001

---

## REQ-SW-DIAG-002. DTC State Management

Requirement:  
Dem_Lite는 DTC 상태를 다음 상태로 관리해야 한다.

```text
PASSED
FAILED
PENDING
CONFIRMED
HEALED
CLEARED
```

Rationale:  
단순 fault flag가 아니라 차량 진단 개념에 가까운 DTC 상태 관리 구조를 구현하기 위함이다.

Verification Method:  
Fault 발생, 반복 발생, fault 해소, DTC clear 명령 시 상태 변화 확인.

Linked Test Case:  
TC-DTC-STATE-001

---

## REQ-SW-DIAG-003. DTC Snapshot Data

Requirement:  
DTC가 발생할 때 ECU는 관련 sensor snapshot data를 함께 저장해야 한다.

Rationale:  
고장 발생 당시의 ECU 상태와 센서값을 진단 증거로 남기기 위함이다.

Verification Method:  
DTC read 명령을 통해 snapshot data 확인.

Linked Test Case:  
TC-DTC-SNAPSHOT-001

---

## REQ-SW-DIAG-004. UART Diagnostic Command Handling

Requirement:  
DCM-Lite는 UART를 통해 PC Tool에서 전달되는 diagnostic command를 처리해야 한다.

Rationale:  
PC Tool이 Central Diagnostic Tool 역할을 수행하려면 command-response 기반 진단 인터페이스가 필요하다.

Verification Method:  
read_state, read_sensor, read_dtc, clear_dtc 명령 테스트.

Linked Test Case:  
TC-DCM-001

---

# 5. Configuration Software Requirements

## REQ-SW-CFG-001. Runtime Threshold Update

Requirement:  
ECU 소프트웨어는 PC Tool 명령을 통해 warningDistanceMm, emergencyDistanceMm, shockThresholdG 값을 runtime에 변경할 수 있어야 한다.

Rationale:  
SDV-style 설정 변경 및 중앙 설정 관리 개념을 축소 구현하기 위함이다.

Verification Method:  
set_threshold 명령 후 ECU 상태 판단 기준 변화 확인.

Linked Test Case:  
TC-CFG-UPDATE-001

---

## REQ-SW-CFG-002. Flash Configuration Storage

Requirement:  
ECU 소프트웨어는 save_config 명령을 받으면 설정값을 Flash에 저장해야 한다.

Rationale:  
ECU 설정값은 재부팅 이후에도 유지될 수 있어야 한다.

Verification Method:  
설정 변경, save_config, reset 이후 설정 유지 확인.

Linked Test Case:  
TC-CFG-SAVE-001

---

## REQ-SW-CFG-003. Configuration CRC Check

Requirement:  
ECU 소프트웨어는 부팅 시 Flash configuration의 CRC를 검증해야 하며, CRC mismatch 발생 시 default configuration으로 복구하고 DTC를 발생시켜야 한다.

Rationale:  
비정상 설정값을 그대로 사용하는 것을 방지하기 위함이다.

Verification Method:  
CRC mismatch Fault Injection 후 default recovery 및 DTC 확인.

Linked Test Case:  
TC-CFG-CRC-001

---

# 6. Watchdog and Timing Requirements

## REQ-SW-WDG-001. Task Alive Monitoring

Requirement:  
WdgM_Lite는 각 FreeRTOS Task의 alive counter를 주기적으로 확인해야 한다.

Rationale:  
Task hang, overrun, 비정상 중단을 감지하기 위함이다.

Verification Method:  
task_overrun 또는 task_alive_timeout Fault Injection.

Linked Test Case:  
TC-WDG-ALIVE-001

---

## REQ-SW-WDG-002. Task Overrun Detection

Requirement:  
정의된 task period를 초과하는 task overrun이 감지되면 ECU는 DTC를 발생시켜야 한다.

Rationale:  
자동차 ECU는 정해진 주기 내에 태스크가 실행되는 것이 중요하다.

Verification Method:  
인위적 delay 삽입 또는 Fault Injection 후 DTC 확인.

Linked Test Case:  
TC-WDG-OVERRUN-001

---

# 7. PC Tool Interface Requirements

## REQ-SW-PC-001. ECU State Monitoring

Requirement:  
PC Tool은 UART를 통해 ECU 상태를 읽고 표시할 수 있어야 한다.

Rationale:  
PC Tool은 Central Diagnostic Tool 역할을 수행한다.

Verification Method:  
read_state 명령 및 응답 확인.

Linked Test Case:  
TC-PC-STATE-001

---

## REQ-SW-PC-002. Fault Injection Command

Requirement:  
PC Tool은 UART command를 통해 predefined Fault Injection을 실행할 수 있어야 한다.

Rationale:  
Fault Injection 기반 검증을 자동화하기 위함이다.

Verification Method:  
inject_fault 명령 후 ECU 상태, DTC, 로그 확인.

Linked Test Case:  
TC-PC-FI-001

---

## REQ-SW-PC-003. CSV Logging

Requirement:  
PC Tool은 테스트 실행 중 timestamp, ECU state, sensor value, DTC, fault injection, response time을 CSV로 저장해야 한다.

Rationale:  
Validation Evidence로 사용할 수 있는 로그를 남기기 위함이다.

Verification Method:  
테스트 실행 후 CSV 파일 생성 및 항목 확인.

Linked Test Case:  
TC-PC-LOG-001

---

## REQ-SW-PC-004. Validation Report Generation

Requirement:  
PC Tool은 Test Case 실행 결과를 Markdown 기반 Validation Report로 생성할 수 있어야 한다.

Rationale:  
요구사항 검증 결과를 포트폴리오 산출물로 남기기 위함이다.

Verification Method:  
run_test 후 validation report 파일 생성 확인.

Linked Test Case:  
TC-PC-REPORT-001
