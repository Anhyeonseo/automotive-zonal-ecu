# TC-IO-001: Internal LED GPIO Output Verification

## 테스트 목적

NUCLEO-G474RE 보드의 내장 LED를 GPIO Output으로 설정하고, 펌웨어에서 주기적으로 토글하여 기본 GPIO 출력 제어가 정상적으로 동작하는지 확인한다.

---

## 관련 요구사항

| Requirement ID | 내용 |
|---|---|
| REQ-SW-IO-001 | 펌웨어는 GPIO Output을 통해 보드 상태 표시용 LED를 제어할 수 있어야 한다. |
| REQ-SW-BASE-001 | 펌웨어 프로젝트는 STM32CubeIDE에서 빌드 가능해야 한다. |
| REQ-SYS-001 | STM32 NUCLEO-G474RE 보드는 단순화된 Front Zonal ECU prototype 역할을 수행해야 한다. |

---

## 테스트 환경

| 항목 | 내용 |
|---|---|
| 보드 | STM32 NUCLEO-G474RE |
| IDE | STM32CubeIDE |
| 설정 도구 | STM32CubeMX |
| Debugger | On-board ST-LINK |
| GPIO Pin | PA5 |
| User Label | LD2 |
| 출력 장치 | 보드 내장 LED |

---

## 테스트 절차

1. STM32CubeMX에서 PA5 핀을 `GPIO_Output`으로 설정한다.
2. PA5의 User Label을 `LD2`로 설정한다.
3. Code Generate를 수행한다.
4. `main.c`의 `while (1)` 루프 안에 LED Toggle 코드를 추가한다.
5. STM32CubeIDE에서 프로젝트를 Build한다.
6. ST-LINK를 통해 펌웨어를 다운로드하고 실행한다.
7. 내장 LED가 500 ms 주기로 점멸하는지 확인한다.

---

## 테스트 코드

```c
while (1)
{
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  HAL_Delay(500);
  /* USER CODE END 3 */
}
```

---

## 기대 결과

| 항목 | 기대 결과 |
|---|---|
| GPIO 설정 | PA5가 GPIO Output으로 설정되어야 한다. |
| Build | 0 errors로 완료되어야 한다. |
| LED 동작 | 내장 LED가 약 500 ms 간격으로 켜짐/꺼짐을 반복해야 한다. |

---

## 실제 결과

| 항목 | 실제 결과 |
|---|---|
| GPIO 설정 | PASS: PA5 GPIO Output 설정 |
| Build | PASS |
| LED 동작 | PASS: 내장 LED Blink 확인 |

---

## 검증 의미

이번 테스트를 통해 다음 항목이 정상 동작함을 확인했다.

- STM32CubeMX 기반 GPIO 설정
- STM32 HAL GPIO 제어 함수 호출
- `HAL_GPIO_TogglePin()` 기반 GPIO 출력 반전
- `HAL_Delay()` 기반 SysTick Delay 동작
- 보드 내장 LED를 이용한 기본 출력 확인

---

## 최종 결과

```text
PASS
```

---

## 비고

이 테스트는 향후 ECU 상태 표시 LED 패턴 구현의 기본 검증으로 사용된다.

예상 확장:

| ECU State | LED Pattern |
|---|---|
| NORMAL | Slow Blink |
| WARNING | Fast Blink |
| EMERGENCY | Very Fast Blink |
| FAIL_SAFE | Fixed ON or dedicated pattern |
