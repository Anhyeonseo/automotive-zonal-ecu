# STM32 Zonal ECU Diagnostic System

## 프로젝트 개요

이 프로젝트는 STM32 기반의 교육용 차량 ECU 프로토타입이다.

목표는 STM32 NUCLEO-G474RE 보드, 센서, 로컬 출력 장치, 진단 로직, Fault Injection, Fail-safe 상태 전이, PC 기반 중앙 진단·검증 도구를 이용해 FreeRTOS 기반 Front Zonal ECU 개념을 축소 구현하는 것이다.

이 프로젝트는 실제 양산 ECU가 아니며, AUTOSAR 또는 ISO 26262 준수를 주장하지 않는다.

대신 자동차 임베디드 소프트웨어 개발 방식을 학습하고 포트폴리오로 정리하기 위한 **AUTOSAR-inspired**, **functional-safety-inspired**, **HIL-inspired** 교육용 프로토타입이다.

---

## 프로젝트명

**STM32 FreeRTOS-based AUTOSAR-inspired Front Zonal ECU with Central Diagnostic & Validation System**

---

## 레포지토리명

```text
stm32-zonal-ecu-diagnostic-system
```

---

## 사용 보드

- STM32 NUCLEO-G474RE

---

## 개발 환경

- STM32CubeIDE
- STM32 HAL
- CMSIS-RTOS v2 / FreeRTOS
- Python 기반 PC Diagnostic & Validation Tool

---

## 시스템 개념

STM32 보드는 단순화된 **Front Zonal ECU** 역할을 한다.

PC는 단순화된 **Central Vehicle Computer** 및 **Diagnostic & Validation Tool** 역할을 한다.

초기 통신은 UART 기반으로 구현한다.  
향후 확장 단계에서는 CAN-FD/FDCAN 통신으로 확장할 수 있다.

---

## STM32 ECU 역할

STM32 ECU는 다음 기능을 담당한다.

- 센서 데이터 수집
- 로컬 ECU 상태 관리
- Diagnostic Trouble Code 관리
- Fault 감지
- Fault Injection에 대한 반응
- Fail-safe 상태 전이
- Flash 기반 설정값 저장
- Watchdog 및 Alive Monitoring
- UART 기반 Diagnostic Protocol 제공
- OLED, LED, Buzzer 출력 제어

---

## PC Tool 역할

PC Tool은 다음 기능을 담당한다.

- ECU 상태 실시간 모니터링
- 센서값 실시간 확인
- DTC 조회
- DTC 삭제
- Fault Injection 명령 전송
- Threshold 설정 변경
- CSV 로그 저장
- Test Case 실행
- Validation Report 생성

---

## 핵심 설계 원칙

이 프로젝트는 단순 STM32 센서 실습이 아니라, 자동차 ECU 소프트웨어 개발 구조를 축소해서 경험하는 것을 목표로 한다.

핵심 설계 원칙은 다음과 같다.

- AUTOSAR-inspired 계층형 소프트웨어 구조를 적용한다.
- Application Layer는 STM32 HAL API를 직접 호출하지 않는다.
- Application logic은 RTE-like Interface를 통해 하위 모듈에 접근한다.
- Diagnostic, Configuration, Communication, Watchdog 기능은 Services Layer로 분리한다.
- 요구사항, 테스트 케이스, 검증 증거는 Traceability Matrix로 연결한다.
- 구현 결과뿐 아니라 검증 과정과 Evidence를 함께 남긴다.

---

## 프로젝트 범위와 한계

이 프로젝트는 다음에 해당한다.

- 교육용 차량 ECU 프로토타입
- AUTOSAR-inspired 구조
- Functional-safety-inspired 고장 대응 구조
- PC Tool 기반 HIL-inspired Validation Environment

이 프로젝트는 다음에 해당하지 않는다.

- AUTOSAR-compliant 시스템
- ISO 26262-compliant 시스템
- 실제 양산 Zonal ECU
- 실제 HIL 시스템
- 인증된 Safety-critical 시스템

---

## 주요 구현 예정 기능

- GPIO/Button/LED/Buzzer Bring-up
- UART Logging 및 Diagnostic Command Interface
- Timer 기반 주기 실행 구조
- ADC 기반 Virtual Pedal 입력
- I2C Sensor Bring-up
- OLED 상태 표시
- MPU6050 IMU 데이터 수집
- VL53L0X 거리 센서 데이터 수집
- ECU State Machine
- FreeRTOS Task Architecture
- DEM-Lite 기반 DTC Manager
- DCM-Lite 기반 UART Diagnostic Protocol
- NvM-Lite 기반 Flash Configuration Storage
- WdgM-Lite 기반 Alive Monitoring
- Fault Injection Framework
- Fail-safe 및 Recovery Logic
- PC Central Diagnostic Tool
- PC Validation Test Runner
- CSV Logging
- Validation Report 생성
- Requirement-Test Traceability Matrix

---

## 문서 구조

```text
docs/
├── requirements/
├── interface/
├── architecture/
├── safety/
├── verification/
├── validation_reports/
└── images/
```

| 폴더 | 역할 |
|---|---|
| requirements | 시스템 요구사항 및 소프트웨어 요구사항 관리 |
| interface | 모듈 간 Interface, Diagnostic Protocol, Signal Dictionary 정의 |
| architecture | Software Architecture, Architecture Rules, State Machine, Task Design 관리 |
| safety | Fault Scenario 및 Safe State 정의 |
| verification | Verification Plan, Test Case, Traceability Matrix 관리 |
| validation_reports | 실제 검증 결과 및 Evidence 저장 |
| images | 아키텍처 그림, 배선도, 테스트 캡처 저장 |

---

## 소프트웨어 아키텍처

펌웨어는 다음과 같은 AUTOSAR-inspired 계층 구조로 구성한다.

```text
Application Layer
RTE-like Interface Layer
Services Layer
ECU Abstraction Layer
Drivers Layer
Platform Layer
```

| 계층 | 역할 |
|---|---|
| Application Layer | ECU 상태 판단, Safety 판단, 제어 로직 |
| RTE-like Interface Layer | Application과 하위 모듈 사이의 인터페이스 제공 |
| Services Layer | 진단, 설정 저장, 통신, Watchdog 관리 |
| ECU Abstraction Layer | 보드별 입출력 세부사항 추상화 |
| Drivers Layer | 센서, OLED, 버튼, LED, Buzzer 등 장치 제어 |
| Platform Layer | STM32 HAL 기반 GPIO, ADC, I2C, UART, Timer, Flash 접근 |

---

## 개발 프로세스

이 프로젝트는 요구사항 기반 개발과 검증 중심 개발 흐름을 따른다.

```text
Requirement Definition
Interface Specification
Architecture Design
Module Bring-up
Module Verification
Integrated ECU Baseline
FreeRTOS Integration
Diagnostic and Safety Logic
Fault Injection Testing
PC Validation Tool
Validation Evidence
Traceability Matrix
Portfolio Packaging
```

즉, 단순히 기능을 구현하는 것에서 끝내지 않고, 각 기능이 어떤 요구사항을 만족하는지, 어떤 테스트로 검증되었는지, 검증 증거가 어디에 저장되어 있는지까지 추적할 수 있도록 개발한다.

---

## 개발 단계 요약

| Phase | 내용 |
|---|---|
| Phase 0 | 레포 및 문서 기반 설계 준비 |
| Phase 1 | AUTOSAR-inspired 코드 구조 스켈레톤 |
| Phase 2 | STM32 보드 Bring-up |
| Phase 3 | GPIO/Button/LED/Buzzer 기본 I/O 검증 |
| Phase 4 | UART Logging 및 Diagnostic Communication 기반 |
| Phase 5 | Timer 기반 주기 실행 및 Timing 검증 |
| Phase 6 | ADC 기반 Virtual Pedal 입력 |
| Phase 7 | I2C Bus Bring-up |
| Phase 8 | OLED/MPU6050/VL53L0X Sensor Driver Bring-up |
| Phase 9 | ECU State Machine Baseline |
| Phase 10 | while-loop 기반 Mini ECU 통합 |
| Phase 11 | FreeRTOS Task Architecture |
| Phase 12 | RTOS Queue/Mutex/Data Consistency |
| Phase 13 | AUTOSAR-inspired 계층 리팩토링 |
| Phase 14 | DEM-Lite / DTC Manager |
| Phase 15 | DCM-Lite / UART Diagnostic Protocol |
| Phase 16 | NvM-Lite / Flash Config Management |
| Phase 17 | WdgM-Lite / Alive Monitoring |
| Phase 18 | Fault Injection Framework |
| Phase 19 | Fail-safe & Recovery Logic |
| Phase 20 | PC Central Diagnostic Tool |
| Phase 21 | PC Validation Tool |
| Phase 22 | CSV Logging & Validation Evidence |
| Phase 23 | Traceability Matrix 완성 |
| Phase 24 | Demo Scenario 구성 |
| Phase 25 | README 및 Portfolio Packaging |
| Phase 26 | CAN-FD/FDCAN 확장 |
| Phase 27 | OTA-like Update / Bootloader 확장 |

---

## Future Work

- CAN-FD/FDCAN 통신
- CAN Signal Specification 작성
- Bootloader
- OTA-like Update-oriented Architecture
- PC Dashboard
- 더 현실적인 HIL-inspired Validation Setup
