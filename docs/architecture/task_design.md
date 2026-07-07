# FreeRTOS Task Design

## 목적

이 문서는 FreeRTOS 기반 ECU Task 구조를 정의한다.

이 프로젝트는 CMSIS-RTOS v2 기준으로 Task를 구성한다.

---

## Task 설계 원칙

- 각 Task는 명확한 주기와 책임을 가진다.
- Task 간 데이터 전달은 Queue, Mutex, 또는 보호된 Shared Data를 사용한다.
- 주기 실행이 깨지거나 Alive Counter가 갱신되지 않으면 WdgM_Lite에서 Fault로 판단한다.
- UART 출력 충돌을 방지하기 위해 공용 UART 접근은 Mutex로 보호한다.

---

## Task 목록

| Task | 주기 | 우선순위 | 역할 |
|---|---:|---:|---|
| SensorTask | 10 ms 또는 20 ms | High | 센서 데이터 수집 |
| StateManagerTask | 20 ms | High | ECU State 판단 |
| ActuatorTask | 50 ms | Medium | LED/OLED/Buzzer 출력 제어 |
| DiagnosticTask | 50 ms | Medium | UART Diagnostic Command 처리 |
| WatchdogTask | 100 ms | High | Alive Monitoring 및 Overrun 감시 |
| LoggerTask | 100 ms | Low | 상태/센서/DTC 로그 출력 |

---

## SensorTask

역할:

- ADC Virtual Pedal 읽기
- MPU6050 데이터 읽기
- VL53L0X 거리값 읽기
- SensorData 구조체 갱신
- 센서 timeout 감지 후보 생성

입력:

- ADC
- I2C sensors

출력:

- SensorData
- Sensor validity flag

관련 Requirement:

- REQ-SW-SEN-001
- REQ-SW-SEN-002

관련 Test Case:

- TC-SEN-001
- TC-SEN-002

---

## StateManagerTask

역할:

- SensorData, FaultStatus, ConfigData 기반 ECU State 판단
- State transition log 생성
- 상태별 출력 요청 생성

입력:

- SensorData
- FaultStatus
- ConfigData

출력:

- EcuState

관련 Requirement:

- REQ-SW-STA-001
- REQ-SW-STA-002

관련 Test Case:

- TC-STA-001
- TC-STA-002
- TC-STA-003

---

## ActuatorTask

역할:

- ECU State에 따라 LED pattern 적용
- Buzzer pattern 적용
- OLED page update

입력:

- EcuState
- SensorData
- DTC summary

출력:

- LED/Buzzer/OLED output

관련 Requirement:

- REQ-SW-ACT-001

관련 Test Case:

- TC-ACT-001

---

## DiagnosticTask

역할:

- UART command 수신
- Dcm_Lite command 처리
- DTC 조회/삭제
- Fault Injection 명령 처리
- Threshold 설정 변경 요청 처리

입력:

- UART RX
- Diagnostic command

출력:

- UART TX response

관련 Requirement:

- REQ-SW-DIAG-003
- REQ-SW-DIAG-004

관련 Test Case:

- TC-DCM-001
- TC-DCM-002

---

## WatchdogTask

역할:

- 각 Task alive counter 확인
- Task overrun 감지
- timeout 발생 시 DTC 보고
- Critical timeout 시 FAIL_SAFE 요청

입력:

- Alive counters
- Task timing data

출력:

- DTC_CONTROL_LOOP_OVERRUN
- FAIL_SAFE request

관련 Requirement:

- REQ-SW-WDG-001
- REQ-SW-WDG-002

관련 Test Case:

- TC-WDG-001
- TC-WDG-002

---

## LoggerTask

역할:

- 주기적 UART log 출력
- 상태, 센서값, DTC summary 출력
- PC Tool CSV logging과 연동 가능한 text output 제공

관련 Test Case:

- TC-LOG-001

---

## Shared Resource 정책

| Resource | 접근 Task | 보호 방식 |
|---|---|---|
| SensorData | SensorTask, StateManagerTask, DiagnosticTask | Mutex 또는 Queue |
| EcuState | StateManagerTask, ActuatorTask, DiagnosticTask | Mutex 또는 Queue |
| DTC Table | Dem_Lite, DiagnosticTask, SafetyManager | Mutex |
| UART TX | DiagnosticTask, LoggerTask | Mutex |
| ConfigData | DiagnosticTask, StateManagerTask, NvM_Lite | Mutex |

---

## Timing 검증

| Test Case ID | 내용 |
|---|---|
| TC-TIM-001 | 10 ms periodic execution 확인 |
| TC-TIM-002 | 20 ms state update 주기 확인 |
| TC-TIM-003 | Logic Analyzer로 GPIO toggle 주기 측정 |
| TC-WDG-003 | Task overrun fault injection 확인 |

---

## 향후 업데이트 항목

- 실제 Task priority 확정
- Stack size 확정
- Queue depth 확정
- osDelayUntil 적용 방식 확정
- Runtime statistics 측정 여부 결정
