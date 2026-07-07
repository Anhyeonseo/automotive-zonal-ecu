# Software Architecture

## 목적

이 문서는 STM32 Zonal ECU Diagnostic System의 전체 소프트웨어 구조를 정의한다.

이 프로젝트의 구조는 실제 AUTOSAR 구현이 아니라, AUTOSAR Classic의 계층 분리 사고를 참고한 **AUTOSAR-inspired architecture**이다.

---

## 전체 구조

```text
Application Layer
    ↓
RTE-like Interface Layer
    ↓
Services Layer / ECU Abstraction Layer
    ↓
Drivers Layer
    ↓
Platform Layer
    ↓
STM32 HAL / Hardware
```

---

## 폴더 구조

```text
firmware/
├── App/
│   ├── StateManager/
│   ├── SafetyManager/
│   └── ZonalControl/
├── Rte/
│   ├── Rte_Sensor/
│   ├── Rte_Actuator/
│   ├── Rte_Diagnostic/
│   └── Rte_Config/
├── Services/
│   ├── Dem_Lite/
│   ├── Dcm_Lite/
│   ├── NvM_Lite/
│   ├── WdgM_Lite/
│   └── Com_Lite/
├── EcuAb/
│   ├── SensorAb/
│   ├── ActuatorAb/
│   └── BoardAb/
├── Drivers/
│   ├── button/
│   ├── led/
│   ├── buzzer/
│   ├── ssd1306/
│   ├── mpu6050/
│   └── vl53l0x/
└── Platform/
    ├── gpio/
    ├── adc/
    ├── i2c/
    ├── uart/
    ├── timer/
    └── flash/
```

---

## Application Layer

Application Layer는 ECU 수준의 판단 로직을 담당한다.

### StateManager

역할:

- ECU State Machine 관리
- INIT, SELF_TEST, NORMAL, WARNING, EMERGENCY, DEGRADED, FAIL_SAFE, RECOVERY 상태 전이
- 센서값과 Fault 상태를 기반으로 상태 판단

관련 Requirement:

- REQ-SW-STA-001
- REQ-SW-STA-002

---

### SafetyManager

역할:

- Critical Fault 판단
- Emergency Stop 입력 처리
- Fail-safe 진입 조건 판단
- Recovery 가능 조건 판단

관련 Requirement:

- REQ-SW-SAF-001
- REQ-SW-SAF-002

---

### ZonalControl

역할:

- Front Zonal ECU 관점의 제어 결정
- Sensor input과 ECU State에 따른 출력 요청 생성
- 추후 CAN-FD 확장 시 Zonal Signal 처리 후보

관련 Requirement:

- REQ-SW-CTRL-001

---

## RTE-like Interface Layer

RTE-like Interface Layer는 Application Layer와 하위 모듈 사이의 직접 의존성을 줄인다.

주요 모듈:

- Rte_Sensor
- Rte_Actuator
- Rte_Diagnostic
- Rte_Config

예시:

```c
Rte_Read_DistanceMm(&distanceMm);
Rte_Call_Diagnostic_ReportFault(DTC_DISTANCE_TIMEOUT);
Rte_Call_Actuator_SetStatePattern(ECU_STATE_WARNING);
```

---

## Services Layer

Services Layer는 ECU 공통 기능을 담당한다.

### Dem_Lite

역할:

- Diagnostic Event 관리
- DTC 상태 관리
- occurrence count, timestamp, snapshot data 저장

---

### Dcm_Lite

역할:

- UART Diagnostic Command 처리
- read_state, read_sensor, read_dtc, clear_dtc, inject_fault 명령 처리

---

### NvM_Lite

역할:

- Flash 기반 설정값 저장
- CRC 검증
- 기본 설정 복구

---

### WdgM_Lite

역할:

- Task Alive Monitoring
- Task Overrun 감지
- Critical timeout 시 Fault 보고

---

### Com_Lite

역할:

- UART 송수신 버퍼 관리
- Diagnostic Protocol message framing 지원

---

## ECU Abstraction Layer

ECU Abstraction Layer는 보드별 입출력 세부사항을 숨긴다.

### SensorAb

- ADC Virtual Pedal 데이터 읽기
- MPU6050 데이터 읽기
- VL53L0X 데이터 읽기
- Sensor validity 판단

### ActuatorAb

- LED pattern 적용
- Buzzer pattern 적용
- OLED page update 요청

### BoardAb

- 보드 핀 매핑
- Emergency Button 입력 추상화
- Board-specific configuration 관리

---

## Drivers Layer

Drivers Layer는 장치 단위 제어만 담당한다.

예시:

- button driver: 버튼 상태 읽기
- led driver: LED on/off/toggle
- buzzer driver: Buzzer 출력
- ssd1306 driver: OLED 출력
- mpu6050 driver: IMU raw data 읽기
- vl53l0x driver: 거리값 읽기

Driver는 ECU State 판단을 하지 않는다.

---

## Platform Layer

Platform Layer는 STM32 HAL에 직접 의존하는 유일한 계층이다.

주요 기능:

- GPIO 제어
- ADC 읽기
- I2C 송수신
- UART 송수신
- Timer tick 제공
- Flash read/write

---

## 주요 데이터 흐름

### 센서 기반 상태 판단

```text
Sensor Driver
→ SensorAb
→ Rte_Sensor
→ StateManager / SafetyManager
→ Rte_Actuator
→ ActuatorAb
→ LED/Buzzer/OLED Driver
```

### Fault 보고 흐름

```text
SensorAb 또는 SafetyManager
→ Rte_Diagnostic
→ Dem_Lite
→ Dcm_Lite
→ PC Tool
```

### PC Diagnostic Command 흐름

```text
PC Tool
→ UART
→ Platform/uart
→ Com_Lite
→ Dcm_Lite
→ Dem_Lite / NvM_Lite / Fault Injection
```

---

## 설계 원칙 요약

- Application은 HAL을 직접 호출하지 않는다.
- Driver는 Application State를 판단하지 않는다.
- Diagnostic state는 Dem_Lite에서 관리한다.
- Flash 접근은 NvM_Lite와 Platform/flash로 제한한다.
- 모든 주요 기능은 Requirement와 Test Case에 연결한다.
