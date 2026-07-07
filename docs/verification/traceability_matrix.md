# Traceability Matrix

## 목적

이 문서는 Requirement, Design Module, Implementation Module, Test Case, Validation Evidence를 연결한다.

Traceability Matrix는 프로젝트 마지막에 한 번 작성하는 문서가 아니라, 개발 과정에서 계속 갱신하는 중심 문서이다.

---

## Traceability Table

| Requirement ID | Requirement Summary | Design Module | Implementation Module | Test Case ID | Evidence |
|---|---|---|---|---|---|
| REQ-SYS-001 | STM32가 Front Zonal ECU prototype 역할 수행 | System Architecture | firmware/ | TC-SYS-001, BRINGUP-001 | TBD |
| REQ-SYS-002 | PC가 Central Diagnostic & Validation Tool 역할 수행 | PC Tool Architecture | pc_tool/ | TC-PC-001, TC-PC-002 | TBD |
| REQ-SYS-003 | UART 기반 초기 통신 제공 | DCM-Lite, Com-Lite | Services/Dcm_Lite, Platform/uart | TC-COM-001, TC-DCM-001 | TBD |
| REQ-SYS-004 | 센서값 기반 ECU 상태 판단 | State Machine | App/StateManager | TC-STA-001~TC-STA-004 | TBD |
| REQ-SYS-005 | DTC 감지 및 관리 | Dem_Lite | Services/Dem_Lite | TC-DTC-001~TC-DTC-003 | TBD |
| REQ-SYS-006 | Critical Fault 시 FAIL_SAFE 전이 | SafetyManager | App/SafetyManager | TC-SAF-001~TC-SAF-003 | TBD |
| REQ-SYS-007 | 설정값 비휘발성 저장 | NvM_Lite | Services/NvM_Lite, Platform/flash | TC-CFG-001~TC-CFG-004 | TBD |
| REQ-SYS-008 | 검증 Evidence 생성 | Validation Tool | pc_tool/validation | TC-PC-004, TC-PC-005 | TBD |
| REQ-SW-SEN-001 | 센서 데이터 주기 수집 | SensorTask | EcuAb/SensorAb, Drivers/* | TC-SEN-001 | TBD |
| REQ-SW-SEN-003 | 거리 센서값 읽기 | SensorAb | Drivers/vl53l0x | TC-DIST-001 | TBD |
| REQ-SW-SEN-004 | ADC Virtual Pedal 읽기 | SensorAb | Platform/adc | TC-ADC-001 | TBD |
| REQ-SW-STA-001 | ECU State Machine 관리 | StateManager | App/StateManager | TC-STA-001, TC-STA-002 | TBD |
| REQ-SW-DIAG-001 | Fault 발생 시 DTC 관리 | Dem_Lite | Services/Dem_Lite | TC-DTC-001, TC-DTC-002 | TBD |
| REQ-SW-SAF-001 | Emergency Button 입력 시 FAIL_SAFE 전이 | SafetyManager | App/SafetyManager, Drivers/button | TC-SAF-001 | TBD |
| REQ-SW-WDG-001 | Task Alive Monitoring | WdgM_Lite | Services/WdgM_Lite | TC-WDG-001, TC-WDG-002 | TBD |
| REQ-SW-CFG-003 | Config Flash 저장 | NvM_Lite | Services/NvM_Lite, Platform/flash | TC-CFG-003 | TBD |

---

## Evidence 작성 규칙

Evidence는 다음 형식을 권장한다.

```text
docs/validation_reports/<TEST_CASE_ID>.md
pc_tool/logs/<TEST_CASE_ID>_<timestamp>.csv
```

예시:

```text
docs/validation_reports/TC-SAF-001.md
pc_tool/logs/TC-SAF-001_2026-07-07_21-30-00.csv
```

---

## 업데이트 규칙

다음 작업이 발생하면 이 문서를 갱신한다.

- Requirement 추가 또는 수정
- Architecture Module 추가 또는 수정
- Test Case 추가 또는 수정
- Validation Report 생성
- CSV Log 저장
- Demo Evidence 추가
