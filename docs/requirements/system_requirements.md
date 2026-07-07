# System Requirements

## 문서 목적

이 문서는 `STM32 Zonal ECU Diagnostic System`의 상위 시스템 요구사항을 정의한다.

본 프로젝트는 STM32 NUCLEO-G474RE 보드를 이용해 Front Zonal ECU 개념을 교육용으로 축소 구현하는 것을 목표로 한다. 또한 PC Tool을 통해 Central Vehicle Computer 및 Diagnostic & Validation Tool의 역할을 일부 모사한다.

이 문서의 요구사항은 이후 Software Requirements, Interface Specification, Architecture Design, Test Case, Validation Evidence와 연결된다.

---

## 요구사항 작성 규칙

각 요구사항은 다음 형식을 따른다.

```text
Requirement ID:
Requirement:
Rationale:
Verification Method:
Linked Test Case:
```

요구사항은 가능한 한 검증 가능하게 작성한다.

예를 들어 `빠르게 반응해야 한다`와 같은 표현은 피하고, `50 ms 이내 FAIL_SAFE 상태로 전이해야 한다`처럼 정량 기준을 포함한다.

---

## 시스템 요구사항 목록

### REQ-SYS-001. Front Zonal ECU 역할

**Requirement:**  
STM32 NUCLEO-G474RE 보드는 교육용 Front Zonal ECU prototype 역할을 수행해야 한다.

**Rationale:**  
이 프로젝트의 목적은 실제 양산 ECU를 구현하는 것이 아니라, 차량 Front Zonal ECU의 센서 수집, 상태 판단, 진단, 안전 상태 전이 개념을 STM32 기반으로 축소 구현하는 것이다.

**Verification Method:**  
System Integration Test 및 Demo Scenario를 통해 확인한다.

**Linked Test Case:**  
TC-SYS-001

---

### REQ-SYS-002. PC Central Diagnostic Tool 역할

**Requirement:**  
PC Tool은 단순화된 Central Vehicle Computer 및 Diagnostic & Validation Tool 역할을 수행해야 한다.

**Rationale:**  
SDV 관점에서는 중앙 컴퓨터가 ECU 상태를 모니터링하고, 진단 정보를 조회하며, 설정값 변경과 검증을 수행할 수 있어야 한다. 본 프로젝트에서는 이를 PC Tool로 축소 구현한다.

**Verification Method:**  
PC Tool Communication Test 및 Validation Scenario를 통해 확인한다.

**Linked Test Case:**  
TC-SYS-002

---

### REQ-SYS-003. UART 기반 초기 통신

**Requirement:**  
STM32 ECU와 PC Tool은 초기 구현 단계에서 UART를 통해 통신해야 한다.

**Rationale:**  
UART는 초기 Bring-up과 진단 프로토콜 구현에 적합한 단순하고 관찰 가능한 통신 방식이다. CAN-FD/FDCAN은 향후 확장 단계로 둔다.

**Verification Method:**  
UART Command-Response Test를 통해 확인한다.

**Linked Test Case:**  
TC-COM-001

---

### REQ-SYS-004. 센서 기반 ECU 상태 관리

**Requirement:**  
ECU는 센서값, Emergency Stop Button 입력, Fault 상태를 기반으로 ECU State를 결정해야 한다.

**Rationale:**  
차량 ECU는 입력 조건에 따라 NORMAL, WARNING, EMERGENCY, DEGRADED, FAIL_SAFE 등의 상태로 전이해야 한다.

**Verification Method:**  
State Transition Test를 통해 확인한다.

**Linked Test Case:**  
TC-STA-001

---

### REQ-SYS-005. Diagnostic Trouble Code 관리

**Requirement:**  
ECU는 사전에 정의된 Fault Condition을 감지하고, 해당 Fault에 대한 Diagnostic Trouble Code를 관리해야 한다.

**Rationale:**  
DTC 관리는 차량 ECU 진단의 핵심 개념이다. 본 프로젝트에서는 DEM-Lite를 통해 DTC 상태와 발생 정보를 관리한다.

**Verification Method:**  
Fault Injection 및 DTC Read Test를 통해 확인한다.

**Linked Test Case:**  
TC-DTC-001

---

### REQ-SYS-006. Fail-safe 상태 전이

**Requirement:**  
ECU는 Critical Fault 또는 Emergency Stop Button 입력이 감지되면 FAIL_SAFE 상태로 전이해야 한다.

**Rationale:**  
이 프로젝트는 functional-safety-inspired 구조를 포함하므로, 고장 상황에서 안전 상태로 전이하는 동작을 명확히 구현하고 검증해야 한다.

**Verification Method:**  
Fault Injection Test 및 Response Time Measurement를 통해 확인한다.

**Linked Test Case:**  
TC-SAF-001

---

### REQ-SYS-007. 비휘발성 설정값 저장

**Requirement:**  
ECU는 Warning Distance, Emergency Distance, Shock Threshold와 같은 주요 Threshold 설정값을 Non-volatile Memory에 저장할 수 있어야 한다.

**Rationale:**  
차량 ECU는 재부팅 후에도 설정값을 유지해야 한다. 본 프로젝트에서는 NvM-Lite와 Flash Storage를 통해 설정값 저장 개념을 축소 구현한다.

**Verification Method:**  
Configuration Save, Reboot, CRC Check Test를 통해 확인한다.

**Linked Test Case:**  
TC-CFG-001

---

### REQ-SYS-008. Validation Evidence 생성

**Requirement:**  
프로젝트는 Test Case 실행 결과, CSV Log, Validation Report, Traceability Matrix 등의 검증 증거를 제공해야 한다.

**Rationale:**  
자동차 소프트웨어 개발에서는 단순 구현보다 요구사항이 어떤 테스트로 검증되었는지와 그 증거가 중요하다.

**Verification Method:**  
Validation Report Review 및 Traceability Matrix Review를 통해 확인한다.

**Linked Test Case:**  
TC-VER-001

---

### REQ-SYS-009. Fault Injection 지원

**Requirement:**  
ECU는 PC Tool 또는 Local Button 입력을 통해 지정된 Fault를 인위적으로 주입할 수 있어야 한다.

**Rationale:**  
Fault Injection은 고장 감지, DTC 발생, 상태 전이, Fail-safe 반응을 검증하기 위한 핵심 기능이다.

**Verification Method:**  
Fault Injection Scenario Test를 통해 확인한다.

**Linked Test Case:**  
TC-FLT-001

---

### REQ-SYS-010. 로컬 출력 장치 기반 상태 표시

**Requirement:**  
ECU는 LED, OLED, Buzzer를 통해 현재 ECU State, 주요 Sensor Value, DTC 또는 Fail-safe 상태를 표시해야 한다.

**Rationale:**  
로컬 출력 장치는 ECU의 상태를 사용자가 직접 확인할 수 있게 하며, 시연 영상에서 상태 전이와 Fault 반응을 명확히 보여준다.

**Verification Method:**  
Actuator Output Test 및 Demo Scenario를 통해 확인한다.

**Linked Test Case:**  
TC-ACT-001

---

### REQ-SYS-011. FreeRTOS 기반 주기 실행 구조

**Requirement:**  
ECU Firmware는 FreeRTOS 기반 Task 구조를 사용하여 Sensor, State Management, Actuator, Diagnostic, Watchdog 기능을 주기적으로 실행해야 한다.

**Rationale:**  
차량 ECU 소프트웨어는 주기 실행 구조와 Task Scheduling 개념이 중요하다. 본 프로젝트는 CMSIS-RTOS v2 기반 FreeRTOS Task 구조로 이를 구현한다.

**Verification Method:**  
Task Period Verification 및 Alive Monitoring Test를 통해 확인한다.

**Linked Test Case:**  
TC-RTOS-001

---

### REQ-SYS-012. 요구사항-테스트 추적성 관리

**Requirement:**  
모든 주요 요구사항은 Design Module, Implementation Module, Test Case, Validation Evidence와 연결되어야 한다.

**Rationale:**  
Traceability Matrix는 요구사항 기반 개발과 검증 중심 개발 흐름을 보여주는 핵심 산출물이다.

**Verification Method:**  
Traceability Matrix Review를 통해 확인한다.

**Linked Test Case:**  
TC-TRC-001

---

## 요구사항 요약 표

| Requirement ID | 요약 | Verification Method | Linked Test Case |
|---|---|---|---|
| REQ-SYS-001 | STM32가 Front Zonal ECU prototype 역할 수행 | System Integration Test | TC-SYS-001 |
| REQ-SYS-002 | PC Tool이 Central Diagnostic Tool 역할 수행 | PC Tool Test | TC-SYS-002 |
| REQ-SYS-003 | UART 기반 초기 통신 | UART Command-Response Test | TC-COM-001 |
| REQ-SYS-004 | 센서 기반 ECU State 관리 | State Transition Test | TC-STA-001 |
| REQ-SYS-005 | DTC 관리 | Fault Injection + DTC Read Test | TC-DTC-001 |
| REQ-SYS-006 | Critical Fault 시 FAIL_SAFE 전이 | Response Time Test | TC-SAF-001 |
| REQ-SYS-007 | 설정값 Non-volatile Storage 저장 | Config Save/Reboot Test | TC-CFG-001 |
| REQ-SYS-008 | Validation Evidence 생성 | Report Review | TC-VER-001 |
| REQ-SYS-009 | Fault Injection 지원 | Fault Scenario Test | TC-FLT-001 |
| REQ-SYS-010 | LED/OLED/Buzzer 상태 표시 | Actuator Output Test | TC-ACT-001 |
| REQ-SYS-011 | FreeRTOS Task 기반 주기 실행 | Task Period Test | TC-RTOS-001 |
| REQ-SYS-012 | Traceability Matrix 관리 | Traceability Review | TC-TRC-001 |
