# State Machine

## 목적

이 문서는 STM32 Front Zonal ECU Prototype의 ECU State Machine을 정의한다.

State Machine은 센서값, Fault 상태, Emergency Button 입력, PC Diagnostic Command에 따라 ECU 동작 상태를 결정한다.

---

## ECU State 목록

| State | 의미 |
|---|---|
| INIT | 부팅 직후 초기화 상태 |
| SELF_TEST | 센서, 설정값, 기본 통신 상태를 점검하는 상태 |
| NORMAL | 정상 동작 상태 |
| WARNING | 주의 조건 감지 상태 |
| EMERGENCY | 위험 조건 감지 상태 |
| DEGRADED | 일부 기능 제한 상태 |
| FAIL_SAFE | 안전 상태 |
| RECOVERY | Fault 해소 후 정상 복귀 준비 상태 |

---

## State 설명

### INIT

진입 조건:

- MCU reset
- 전원 인가

동작:

- 기본 변수 초기화
- Clock, GPIO, UART, Timer 초기화
- Boot log 출력

탈출 조건:

- 기본 초기화 완료 시 SELF_TEST로 전이

---

### SELF_TEST

진입 조건:

- INIT 완료
- reset_ecu 이후 재부팅

동작:

- Config CRC 확인
- 기본 Sensor Interface 확인
- DTC Manager 초기화
- 통신 상태 초기화

탈출 조건:

- Self-test 통과 시 NORMAL
- Critical fault 발생 시 FAIL_SAFE
- 일부 센서 오류 발생 시 DEGRADED

---

### NORMAL

진입 조건:

- SELF_TEST 통과
- RECOVERY 완료

동작:

- 센서값 정상 수집
- 상태 로그 출력
- PC Diagnostic Command 허용
- 일반 LED/OLED 표시

탈출 조건:

- `distance_mm < warning_distance_mm`이면 WARNING
- Critical Fault 발생 시 FAIL_SAFE
- Sensor timeout 발생 시 DEGRADED 또는 FAIL_SAFE

---

### WARNING

진입 조건:

- 전방 거리값이 Warning Threshold 이하

동작:

- Warning LED/Buzzer pattern 출력
- OLED에 WARNING 표시
- PC Tool에 상태 보고

탈출 조건:

- 거리값이 정상 범위로 회복되면 NORMAL
- 거리값이 Emergency Threshold 이하이면 EMERGENCY
- Critical Fault 발생 시 FAIL_SAFE

---

### EMERGENCY

진입 조건:

- 전방 거리값이 Emergency Threshold 이하
- Emergency 조건에 해당하는 Fault 감지

동작:

- Emergency LED/Buzzer pattern 출력
- OLED에 EMERGENCY 표시
- DTC 후보 이벤트 보고

탈출 조건:

- Critical Fault이면 FAIL_SAFE
- 조건이 해소되면 WARNING 또는 RECOVERY

---

### DEGRADED

진입 조건:

- 일부 센서 오류 발생
- 기능 제한으로 계속 동작 가능한 Fault 발생

동작:

- 제한 동작 모드 진입
- 관련 DTC 보고
- PC Tool에 DEGRADED 상태 보고

탈출 조건:

- Fault 해소 후 RECOVERY
- Critical Fault로 악화되면 FAIL_SAFE

---

### FAIL_SAFE

진입 조건:

- Emergency Button 입력
- Critical Fault 감지
- Task Alive Timeout
- Config CRC mismatch 후 복구 실패
- Critical Sensor Fault

동작:

- 안전 출력 패턴 적용
- 관련 DTC Confirmed 처리 후보
- 일반 제어 출력 제한
- PC Diagnostic Command 일부 허용

탈출 조건:

- Fault 해소 및 start_recovery 명령 조건 만족 시 RECOVERY
- 또는 reset_ecu 필요

---

### RECOVERY

진입 조건:

- Fault 조건 해소
- start_recovery 명령 수신
- Recovery 조건 만족

동작:

- 센서 정상 여부 재확인
- DTC 상태 HEALED 후보 처리
- 상태 복귀 준비

탈출 조건:

- Recovery 성공 시 NORMAL
- Recovery 실패 시 DEGRADED 또는 FAIL_SAFE

---

## 주요 상태 전이

| From | To | 조건 |
|---|---|---|
| INIT | SELF_TEST | 기본 초기화 완료 |
| SELF_TEST | NORMAL | Self-test 통과 |
| SELF_TEST | DEGRADED | 일부 센서 오류 |
| SELF_TEST | FAIL_SAFE | Critical fault |
| NORMAL | WARNING | distance_mm < warning_distance_mm |
| WARNING | EMERGENCY | distance_mm < emergency_distance_mm |
| ANY | FAIL_SAFE | Emergency Button 또는 Critical Fault |
| DEGRADED | RECOVERY | Fault 해소 및 recovery 요청 |
| FAIL_SAFE | RECOVERY | Fault 해소 및 recovery 조건 만족 |
| RECOVERY | NORMAL | Recovery 성공 |

---

## 상태별 출력 정책

| State | LED | Buzzer | OLED |
|---|---|---|---|
| INIT | Boot pattern | Off | Boot |
| SELF_TEST | Self-test pattern | Off | Self-test |
| NORMAL | Normal pattern | Off | Sensor values |
| WARNING | Warning pattern | Short beep | Warning |
| EMERGENCY | Emergency pattern | Repeated beep | Emergency |
| DEGRADED | Degraded pattern | Short alert | Degraded + DTC |
| FAIL_SAFE | Fail-safe pattern | Fail-safe alert | Fail-safe + DTC |
| RECOVERY | Recovery pattern | Off or short beep | Recovery |

---

## 검증 항목

| Test Case ID | 내용 |
|---|---|
| TC-STA-001 | INIT → SELF_TEST 전이 확인 |
| TC-STA-002 | SELF_TEST → NORMAL 전이 확인 |
| TC-STA-003 | NORMAL → WARNING 전이 확인 |
| TC-STA-004 | WARNING → EMERGENCY 전이 확인 |
| TC-SAF-001 | Emergency Button 입력 시 FAIL_SAFE 전이 확인 |
| TC-SAF-004 | Recovery 조건 확인 |
