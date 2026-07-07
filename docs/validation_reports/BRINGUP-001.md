# BRINGUP-001: STM32 Board Bring-up Verification

## 테스트 목적

NUCLEO-G474RE 보드가 PC에서 정상 인식되고, STM32CubeIDE에서 기본 펌웨어를 빌드, 다운로드, 디버그할 수 있는지 확인한다.

---

## 관련 요구사항

| Requirement ID | 내용 |
|---|---|
| REQ-SYS-001 | STM32 NUCLEO-G474RE 보드는 단순화된 Front Zonal ECU prototype 역할을 수행해야 한다. |
| REQ-SW-BASE-001 | 펌웨어 프로젝트는 STM32CubeIDE에서 빌드 가능해야 한다. |
| REQ-SW-BASE-002 | 펌웨어 프로젝트는 ST-LINK를 통해 다운로드 및 디버그 가능해야 한다. |

---

## 테스트 환경

| 항목 | 내용 |
|---|---|
| 보드 | STM32 NUCLEO-G474RE |
| MCU | STM32G47x/G48x/G414 계열 |
| CPU | Cortex-M4 |
| IDE | STM32CubeIDE |
| Debugger | On-board ST-LINK |
| ST-LINK FW | V3J17M11 |
| Host OS | Windows |
| 연결 방식 | USB Micro-B |
| Debug Interface | SWD |
| Target Voltage | 3.29V |

---

## 테스트 절차

1. NUCLEO-G474RE 보드를 USB로 PC에 연결한다.
2. Windows 장치 관리자에서 ST-LINK 인식 여부를 확인한다.
3. STM32CubeIDE에서 `NUCLEO-G474RE` 보드 기반 프로젝트를 생성한다.
4. 프로젝트 이름을 `front_zonal_ecu_firmware`로 설정한다.
5. 기본 코드를 생성한다.
6. 프로젝트를 Build한다.
7. Debug를 실행한다.
8. ST-LINK를 통해 펌웨어가 Flash에 다운로드되는지 확인한다.
9. Download verify가 성공하는지 확인한다.
10. `main.c`의 `for(;;)` 구간에서 디버그 정지가 가능한지 확인한다.

---

## 기대 결과

| 항목 | 기대 결과 |
|---|---|
| ST-LINK 인식 | 장치 관리자 또는 CubeIDE에서 ST-LINK가 인식되어야 한다. |
| Board Detection | 보드가 NUCLEO-G474RE로 인식되어야 한다. |
| Build | 0 errors로 완료되어야 한다. |
| Flash Download | 펌웨어 다운로드가 성공해야 한다. |
| Verify | Download verify가 성공해야 한다. |
| Debug | `main.c`에서 디버그 정지가 가능해야 한다. |

---

## 실제 결과

| 항목 | 실제 결과 |
|---|---|
| ST-LINK 인식 | PASS |
| Board Detection | PASS: NUCLEO-G474RE |
| Target Voltage | PASS: 3.29V |
| Build | PASS: 0 errors, 1 warning |
| Flash Download | PASS |
| Verify | PASS: Download verified successfully |
| Debug | PASS: `main.c`의 `for(;;)` 구간에서 정지 확인 |

---

## Build 결과 요약

```text
Build Finished. 0 errors, 1 warnings.
```

---

## Debug 로그 요약

```text
Board       : NUCLEO-G474RE
Voltage     : 3.29V
Device CPU  : Cortex-M4
Download verified successfully
```

---

## Warning 기록

빌드 중 다음 warning이 발생했다.

```text
FPU is not initialized, but the project is compiling for an FPU.
```

현재 Phase 2에서는 FPU를 사용하는 연산을 구현하지 않았으므로 Bring-up 검증에는 영향이 없는 것으로 판단한다.  
추후 수치 연산 또는 제어 로직을 구현할 때 FPU 초기화 설정을 별도로 확인한다.

---

## 최종 결과

```text
PASS
```

---

## 비고

초기 Bring-up 단계에서는 센서, OLED, FreeRTOS, UART, ADC, I2C, CAN-FD 설정은 진행하지 않았다.

이번 테스트를 통해 NUCLEO-G474RE 보드의 기본 개발환경, 빌드, 다운로드, 디버그 진입이 정상임을 확인했다.
