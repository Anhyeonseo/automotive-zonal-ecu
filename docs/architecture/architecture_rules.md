# Architecture Rules

## 문서 목적

이 문서는 `STM32 Zonal ECU Diagnostic System` 펌웨어의 소프트웨어 아키텍처 규칙을 정의한다.

본 프로젝트는 AUTOSAR-inspired 구조를 참고하지만, 실제 AUTOSAR 표준을 구현하거나 AUTOSAR-compliant임을 주장하지 않는다.

이 문서의 목적은 프로젝트가 단순 STM32 센서 예제가 아니라, 자동차 ECU 소프트웨어처럼 계층 분리, 인터페이스 분리, 진단 서비스 분리, 요구사항-테스트 추적성을 갖도록 개발 기준을 정하는 것이다.

---

## Layering Model

펌웨어는 다음과 같은 AUTOSAR-inspired 계층 구조로 구성한다.

```text
Application Layer
RTE-like Interface Layer
Services Layer
ECU Abstraction Layer
Drivers Layer
Platform Layer
```

---

## Layer Responsibilities

## 1. Application Layer

Application Layer는 ECU 수준의 핵심 동작 로직을 담당한다.

예정 모듈은 다음과 같다.

```text
StateManager
SafetyManager
ZonalControl
```

주요 책임은 다음과 같다.

- ECU State 전이 판단
- Safety 판단 로직
- 고수준 제어 판단
- Fault 발생 시 반응 결정
- Recovery 가능 여부 판단

Application Layer는 STM32 HAL API를 직접 호출하지 않는다.

---

## 2. RTE-like Interface Layer

RTE-like Interface Layer는 Application Layer와 하위 모듈 사이의 접근 경로를 제공한다.

예정 모듈은 다음과 같다.

```text
Rte_Sensor
Rte_Actuator
Rte_Diagnostic
Rte_Config
```

주요 책임은 다음과 같다.

- Sensor read interface 제공
- Actuator command interface 제공
- Diagnostic service call interface 제공
- Configuration read/write interface 제공
- Application logic과 하위 모듈 간 직접 의존성 감소

---

## 3. Services Layer

Services Layer는 ECU 공통 서비스를 담당한다.

예정 모듈은 다음과 같다.

```text
Dem_Lite
Dcm_Lite
NvM_Lite
WdgM_Lite
Com_Lite
```

주요 책임은 다음과 같다.

- Diagnostic Trouble Code 관리
- Diagnostic command 처리
- Non-volatile configuration 관리
- Watchdog 및 Alive Monitoring
- Communication service 추상화

---

## 4. ECU Abstraction Layer

ECU Abstraction Layer는 보드별 입출력 세부사항을 상위 계층에서 숨긴다.

예정 모듈은 다음과 같다.

```text
SensorAb
ActuatorAb
BoardAb
```

주요 책임은 다음과 같다.

- Sensor input 접근 추상화
- Actuator output 접근 추상화
- Board-specific pin mapping 추상화
- 상위 계층에 hardware-independent interface 제공

---

## 5. Drivers Layer

Drivers Layer는 개별 장치 수준의 제어를 담당한다.

예정 모듈은 다음과 같다.

```text
button
led
buzzer
ssd1306
mpu6050
vl53l0x
```

주요 책임은 다음과 같다.

- Device initialization
- Device read/write operation
- Device-level error detection
- Application state logic 배제

---

## 6. Platform Layer

Platform Layer는 STM32 peripheral 접근을 담당한다.

예정 모듈은 다음과 같다.

```text
gpio
adc
i2c
uart
timer
flash
```

주요 책임은 다음과 같다.

- STM32 HAL 기반 peripheral access
- GPIO control
- ADC read
- I2C communication
- UART communication
- Timer access
- Flash read/write

Platform Layer는 STM32 HAL에 직접 의존하는 유일한 계층이다.

---

# Architecture Rules

## AR-001. Application Layer HAL Access 금지

Application Layer는 STM32 HAL API를 직접 호출하지 않는다.

Application Layer에서 금지되는 호출 예시는 다음과 같다.

```c
HAL_GPIO_WritePin(...);
HAL_I2C_Master_Transmit(...);
HAL_UART_Transmit(...);
HAL_ADC_Start(...);
HAL_FLASH_Program(...);
```

Application logic은 RTE-like Interface를 통해 하위 기능에 접근해야 한다.

---

## AR-002. RTE-like Interface 경유

Application Layer는 Sensor, Actuator, Diagnostic, Configuration 데이터에 접근할 때 RTE-like Interface를 사용한다.

예시는 다음과 같다.

```c
Rte_Read_DistanceMm(&distanceMm);
Rte_Read_PedalPercent(&pedalPercent);
Rte_Write_EcuState(ECU_STATE_NORMAL);
Rte_Call_Actuator_SetPattern(ACTUATOR_PATTERN_WARNING);
Rte_Call_Diagnostic_ReportFault(DTC_DISTANCE_TIMEOUT);
```

---

## AR-003. Services Layer 책임 분리

Services Layer는 진단, 설정 저장, 통신, Watchdog 감시와 같은 ECU 공통 기능을 담당한다.

Application module은 DTC 상태, Flash memory layout, UART command parsing을 직접 관리하지 않는다.

---

## AR-004. ECU Abstraction 책임

ECU Abstraction Layer는 보드별 입출력 세부사항을 Application logic으로부터 분리한다.

Application module은 물리적 핀 이름, STM32 port 이름, 보드 배선 정보에 직접 의존하지 않는다.

---

## AR-005. Driver 책임 제한

Driver는 장치 수준의 제어만 담당한다.

Driver는 Application state logic을 포함하지 않는다.

예를 들어 VL53L0X driver는 거리값과 timeout status를 반환할 수 있지만, ECU State가 WARNING, EMERGENCY, FAIL_SAFE인지 판단하지 않는다.

---

## AR-006. Platform Layer의 HAL 의존성 제한

Platform Layer는 STM32 HAL에 직접 의존하는 유일한 계층이다.

상위 계층에서 GPIO, ADC, I2C, UART, Timer, Flash 접근이 필요할 경우 HAL을 직접 호출하지 않고 wrapper interface를 호출한다.

---

## AR-007. Diagnostic Separation

Diagnostic event reporting과 Application state logic은 분리한다.

Application module은 service interface를 통해 fault를 보고할 수 있지만, DTC 상태 관리는 Dem_Lite가 담당한다.

예시는 다음과 같다.

```c
Rte_Call_Diagnostic_ReportFault(DTC_IMU_COMM_FAIL);
```

Application Layer는 PENDING, CONFIRMED, HEALED, CLEARED와 같은 DTC status field를 직접 수정하지 않는다.

---

## AR-008. Configuration Access

Runtime configuration 값은 RTE-like Interface 또는 NvM_Lite Interface를 통해 접근한다.

Application module은 Flash memory에 직접 접근하지 않는다.

Configuration 값 예시는 다음과 같다.

```text
warningDistanceMm
emergencyDistanceMm
shockThresholdG
configVersion
```

---

## AR-009. Communication Separation

UART 송수신 처리와 Diagnostic command 해석은 분리한다.

- `Platform/uart`는 UART byte-level access를 담당한다.
- `Com_Lite` 또는 `Dcm_Lite`는 diagnostic message parsing과 command processing을 담당한다.

---

## AR-010. Fault Detection과 Fault Reaction 분리

가능한 경우 Fault Detection과 Fault Reaction을 분리한다.

예시는 다음과 같다.

```text
SensorAb detects sensor timeout.
Dem_Lite records the diagnostic event.
SafetyManager decides whether to enter DEGRADED or FAIL_SAFE.
ActuatorAb applies the output pattern.
```

이 구조를 통해 fault 감지, DTC 기록, Safety 판단, 출력 제어의 책임을 분리한다.

---

## AR-011. Traceability

주요 소프트웨어 기능은 다음 항목과 연결되어야 한다.

```text
Requirement ID
Design Module
Implementation Module
Test Case ID
Validation Evidence
```

예시는 다음과 같다.

```text
REQ-SAF-001
Design: SafetyManager, StateManager
Implementation: App/SafetyManager, Services/Dem_Lite, Drivers/button
Test: TC-SAF-001
Evidence: docs/validation_reports/TC-SAF-001.md
```

---

## AR-012. Naming Convention

Module 이름은 해당 모듈의 아키텍처 계층과 역할을 드러내야 한다.

예시는 다음과 같다.

```text
App/StateManager
Rte/Rte_Sensor
Services/Dem_Lite
EcuAb/SensorAb
Drivers/mpu6050
Platform/i2c
```

---

## AR-013. Testability

각 모듈은 테스트 가능한 형태로 작성해야 한다.

이를 위해 다음 원칙을 따른다.

- 상태 판단 로직과 하드웨어 접근 코드를 분리한다.
- Sensor value, Fault status, Config value는 가능한 한 구조체로 전달한다.
- Fault Injection을 통해 주요 fault path를 재현할 수 있어야 한다.
- Test Case와 Validation Evidence를 남길 수 있어야 한다.

---

## AR-014. Logging and Evidence

중요 상태 변화, DTC 발생, Fault Injection, Fail-safe 전이는 로그로 남긴다.

필요한 경우 다음 evidence를 저장한다.

```text
UART log
CSV log
Validation Report
Logic Analyzer capture
Screenshot
Demo video
```

---

# Compliance Statement

이 프로젝트는 AUTOSAR-compliant 시스템이 아니다.

이 프로젝트는 AUTOSAR-inspired 아키텍처 개념을 교육용·포트폴리오용으로 축소 적용한다.

본 문서의 Architecture Rules는 계층 분리, 인터페이스 분리, Diagnostic Service 분리, Requirement-to-Test Traceability를 보여주기 위한 개발 기준이다.
