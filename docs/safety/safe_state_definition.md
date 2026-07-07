# Safe State Definition

## 목적

이 문서는 STM32 Zonal ECU Diagnostic System의 Safe State 개념을 정의한다.

이 프로젝트의 Safe State는 실제 ISO 26262 기반 안전 상태가 아니라, 교육용 **functional-safety-inspired safe state**이다.

---

## Safe State 정의

이 프로젝트에서 Safe State는 다음을 의미한다.

```text
Critical Fault가 발생했을 때 ECU가 일반 제어 동작을 제한하고,
사용자와 PC Tool에 위험 상태를 명확히 표시하며,
진단과 복구 판단이 가능하도록 유지되는 상태
```

State Machine에서는 `FAIL_SAFE`가 Safe State에 해당한다.

---

## FAIL_SAFE 진입 조건

다음 조건 중 하나가 발생하면 FAIL_SAFE 진입 후보가 된다.

- Emergency Button 입력
- Critical Sensor Fault
- Task Alive Timeout
- Control Loop Overrun
- Config CRC mismatch 복구 실패
- SafetyManager가 Critical Fault로 판단한 경우

---

## FAIL_SAFE 상태 동작

FAIL_SAFE 상태에서는 다음 동작을 수행한다.

| 항목 | 동작 |
|---|---|
| 제어 출력 | 일반 출력 제한 또는 비활성화 |
| LED | Fail-safe pattern 출력 |
| Buzzer | Fail-safe alert 출력 |
| OLED | FAIL_SAFE 상태와 주요 DTC 표시 |
| DTC | 관련 Fault를 PENDING 또는 CONFIRMED로 관리 |
| UART Diagnostic | read_state, read_dtc 등 안전한 명령은 허용 |
| Recovery | 조건 만족 시에만 start_recovery 허용 |

---

## FAIL_SAFE에서 허용되는 Diagnostic Command

| Command | 허용 여부 | 설명 |
|---|---:|---|
| read_state | 허용 | 현재 상태 확인 |
| read_sensor | 허용 | 센서값 확인 |
| read_dtc | 허용 | DTC 확인 |
| clear_dtc | 제한 | Fault 해소 후 조건부 허용 |
| inject_fault | 제한 | 안전 상태에서는 기본적으로 제한 |
| set_threshold | 제한 | 안전 상태에서는 기본적으로 제한 |
| save_config | 제한 | 안전 상태에서는 기본적으로 제한 |
| start_recovery | 조건부 허용 | Fault 해소 확인 후 허용 |
| reset_ecu | 허용 후보 | 구현 정책에 따라 결정 |

---

## Recovery 조건

FAIL_SAFE에서 RECOVERY로 전이하려면 다음 조건을 만족해야 한다.

- Critical Fault가 더 이상 감지되지 않아야 한다.
- Emergency Button 입력이 해제되어야 한다.
- 센서 데이터 유효성이 회복되어야 한다.
- Task Alive Monitoring이 정상이어야 한다.
- PC Tool 또는 내부 정책에 의해 Recovery가 요청되어야 한다.

---

## Recovery 실패 조건

다음 조건이면 Recovery를 실패 처리한다.

- 동일 Fault 재발
- DTC가 여전히 CONFIRMED 상태이며 Fault 조건이 유지됨
- 센서 유효성 회복 실패
- Task alive timeout 지속
- 설정값 CRC 검증 실패 지속

Recovery 실패 시 FAIL_SAFE 또는 DEGRADED로 전이한다.

---

## 정량 검증 기준 초안

| 항목 | 기준 |
|---|---:|
| Emergency Button 입력 후 FAIL_SAFE 진입 | 50 ms 이내 |
| Critical Fault DTC 보고 | 100 ms 이내 |
| Fail-safe LED/Buzzer 출력 반영 | 100 ms 이내 |
| PC Tool에서 FAIL_SAFE 상태 조회 가능 | 500 ms 이내 |

---

## 관련 Test Case

| Test Case ID | 내용 |
|---|---|
| TC-SAF-001 | Emergency Button 입력 시 FAIL_SAFE 전이 |
| TC-SAF-002 | Sensor timeout 시 DEGRADED 또는 FAIL_SAFE 전이 |
| TC-SAF-003 | Task alive timeout 시 FAIL_SAFE 전이 |
| TC-SAF-004 | Fault 해소 후 Recovery 진입 |
| TC-SAF-005 | FAIL_SAFE 출력 pattern 확인 |

---

## 한계

이 Safe State 정의는 실제 차량 제어기 안전 분석 결과가 아니다.

ASIL 등급, HARA, FMEA, FTA, ISO 26262 산출물은 포함하지 않는다.

이 문서는 교육용 ECU prototype에서 Fault 감지와 안전 상태 전이 개념을 명확히 하기 위한 기준이다.
