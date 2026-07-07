# Signal Dictionary

## 목적

이 문서는 STM32 ECU 내부와 PC Tool 사이에서 사용되는 주요 Signal을 정의한다.

Signal Dictionary는 센서값, 상태값, 진단 정보, 설정값, 로그 항목의 이름과 단위, 범위, 갱신 주기를 명확히 하기 위해 사용한다.

---

## Sensor Signals

| Signal Name | 설명 | 단위 | 범위 | 갱신 주기 | Source | Consumer |
|---|---|---:|---:|---:|---|---|
| distance_mm | 전방 거리 센서값 | mm | TBD | 20 ms | VL53L0X | StateManager, SafetyManager, PC Tool |
| pedal_percent | Virtual Pedal 입력값 | % | 0~100 | 20 ms | ADC | StateManager, PC Tool |
| accel_x_g | X축 가속도 | g | TBD | 20 ms | MPU6050 | SafetyManager, PC Tool |
| accel_y_g | Y축 가속도 | g | TBD | 20 ms | MPU6050 | SafetyManager, PC Tool |
| accel_z_g | Z축 가속도 | g | TBD | 20 ms | MPU6050 | SafetyManager, PC Tool |
| imu_valid | IMU 데이터 유효성 | bool | true/false | 20 ms | SensorAb | Dem_Lite, SafetyManager |
| distance_valid | 거리 센서 데이터 유효성 | bool | true/false | 20 ms | SensorAb | Dem_Lite, SafetyManager |

---

## State Signals

| Signal Name | 설명 | 값 | 갱신 주기 | Source | Consumer |
|---|---|---|---:|---|---|
| ecu_state | 현재 ECU State | INIT/SELF_TEST/NORMAL/WARNING/EMERGENCY/DEGRADED/FAIL_SAFE/RECOVERY | 20 ms | StateManager | ActuatorTask, DiagnosticTask, PC Tool |
| previous_ecu_state | 이전 ECU State | EcuState | 20 ms | StateManager | Logger, PC Tool |
| state_entered_time_ms | 현재 State 진입 시각 | ms | State 변경 시 | StateManager | Logger, Validation Tool |

---

## Diagnostic Signals

| Signal Name | 설명 | 값 | Source | Consumer |
|---|---|---|---|---|
| dtc_code | DTC 코드 | P1001~P1006 | Dem_Lite | Dcm_Lite, PC Tool |
| dtc_status | DTC 상태 | PASSED/FAILED/PENDING/CONFIRMED/HEALED/CLEARED | Dem_Lite | Dcm_Lite, PC Tool |
| dtc_occurrence_count | DTC 발생 횟수 | 0~65535 | Dem_Lite | Dcm_Lite, PC Tool |
| dtc_first_detected_tick | 최초 감지 tick | ms | Dem_Lite | PC Tool |
| dtc_last_detected_tick | 마지막 감지 tick | ms | Dem_Lite | PC Tool |

---

## Configuration Signals

| Signal Name | 설명 | 단위 | 기본값 | 범위 | 저장 여부 |
|---|---|---:|---:|---:|---|
| warning_distance_mm | WARNING 상태 진입 거리 기준 | mm | 600 | TBD | Flash 저장 |
| emergency_distance_mm | EMERGENCY 상태 진입 거리 기준 | mm | 250 | TBD | Flash 저장 |
| shock_threshold_g | 충격 감지 기준 | g | 2.5 | TBD | Flash 저장 |
| config_version | 설정 구조 버전 | - | 1 | 1~65535 | Flash 저장 |
| config_crc32 | 설정 CRC | - | 계산값 | uint32 | Flash 저장 |

---

## Fault Injection Signals

| Signal Name | 설명 | 값 |
|---|---|---|
| injected_fault | 현재 주입된 Fault | none/distance_timeout/imu_comm_fail/sensor_out_of_range/task_overrun/config_crc_mismatch/emergency_stop |
| fault_injection_active | Fault Injection 활성 여부 | true/false |
| fault_injection_time_ms | Fault Injection 시작 시각 | ms |

---

## Actuator Output Signals

| Signal Name | 설명 | 값 | Consumer |
|---|---|---|---|
| led_pattern | LED 출력 패턴 | OFF/NORMAL/WARNING/EMERGENCY/FAIL_SAFE | LED Driver |
| buzzer_pattern | Buzzer 출력 패턴 | OFF/SHORT_BEEP/REPEAT_BEEP/FAIL_SAFE_ALERT | Buzzer Driver |
| oled_page | OLED 표시 화면 | STATE/SENSOR/DTC/FAULT | SSD1306 Driver |

---

## Logging Signals

| CSV Column | 설명 | 단위 |
|---|---|---:|
| timestamp_ms | 로그 시간 | ms |
| ecu_state | 현재 ECU State | - |
| distance_mm | 전방 거리 | mm |
| pedal_percent | Virtual Pedal 값 | % |
| accel_x_g | X축 가속도 | g |
| accel_y_g | Y축 가속도 | g |
| accel_z_g | Z축 가속도 | g |
| dtc_active | 활성 DTC 여부 | bool |
| fault_injection | 주입된 Fault | string |
| response_time_ms | Fault 주입 후 반응 시간 | ms |

---

## 업데이트 원칙

Signal이 추가되면 다음 문서도 함께 업데이트한다.

- interface_spec.md
- diagnostic_protocol.md
- test_case_list.md
- traceability_matrix.md
