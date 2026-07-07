# TC-IO-002: External LED GPIO Output Verification

## 테스트 목적

NUCLEO-G474RE 보드의 외부 GPIO 핀을 이용해 브레드보드에 연결한 외부 LED를 제어할 수 있는지 확인한다.

이번 테스트에서는 Arduino header의 `D3` 핀을 사용했고, CubeMX에서는 해당 핀을 `PB3` GPIO Output으로 설정하였다.

---

## 관련 요구사항

| Requirement ID | 내용 |
|---|---|
| REQ-SW-IO-002 | ECU는 외부 GPIO Output을 통해 상태 표시용 LED를 제어할 수 있어야 한다. |
| REQ-SYS-004 | ECU는 입력 및 상태 조건에 따라 로컬 출력 장치를 제어할 수 있어야 한다. |

---

## 테스트 환경

| 항목 | 내용 |
|---|---|
| 보드 | STM32 NUCLEO-G474RE |
| IDE | STM32CubeIDE |
| 설정 도구 | STM32CubeMX |
| 사용 핀 | D3 / PB3 |
| User Label | EXT_LED |
| 출력 장치 | 외부 LED |
| 보호 저항 | 220Ω 또는 330Ω |
| 연결 방식 | GPIO → 저항 → LED → GND |

---

## CubeMX 설정

| 항목 | 설정값 |
|---|---|
| Pin | PB3 |
| Arduino Header | D3 |
| Mode | GPIO_Output |
| Output Type | Push Pull |
| Pull-up/Pull-down | No pull-up and no pull-down |
| Speed | Low |
| User Label | EXT_LED |

---

## 테스트 코드

`main.c`의 `while (1)` 내부에 다음 코드를 추가하였다.

```c
HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
HAL_GPIO_TogglePin(EXT_LED_GPIO_Port, EXT_LED_Pin);
HAL_Delay(500);
```

---

## 테스트 절차

1. CubeMX에서 `PB3`를 `GPIO_Output`으로 설정한다.
2. User Label을 `EXT_LED`로 지정한다.
3. 코드를 Generate한다.
4. `main.h`에 `EXT_LED_Pin`, `EXT_LED_GPIO_Port`가 생성되었는지 확인한다.
5. 외부 LED를 `D3 → 저항 → LED → GND` 구조로 연결한다.
6. `main.c`의 `while (1)`에서 `EXT_LED`를 500 ms 주기로 Toggle한다.
7. 프로젝트를 Build한다.
8. Debug 또는 Run을 실행한다.
9. 외부 LED가 500 ms 주기로 점멸하는지 확인한다.

---

## 기대 결과

| 항목 | 기대 결과 |
|---|---|
| Build | 0 errors로 완료되어야 한다. |
| GPIO 설정 | `EXT_LED_Pin`, `EXT_LED_GPIO_Port`가 생성되어야 한다. |
| 외부 LED 출력 | 외부 LED가 500 ms 주기로 점멸해야 한다. |
| 내장 LED 출력 | 내장 LD2 LED도 기존 코드에 따라 점멸해야 한다. |

---

## 실제 결과

| 항목 | 실제 결과 |
|---|---|
| Build | PASS |
| GPIO 설정 | PASS: D3 / PB3를 `EXT_LED`로 설정 |
| 외부 LED 출력 | PASS: 외부 LED 점멸 확인 |
| 내장 LED 출력 | PASS: LD2 점멸 확인 |

---

## 최종 결과

```text
PASS
```

---

## 비고

초기에는 D13/PA5 신호를 외부 LED로 확인하려 했으나, 보드 핀 매핑 확인이 혼동되어 독립 외부 출력 핀으로 D3/PB3를 사용하였다.

이번 테스트를 통해 STM32 GPIO Output이 보드 외부 핀으로 정상 출력되고, 외부 LED 회로를 제어할 수 있음을 확인했다.
