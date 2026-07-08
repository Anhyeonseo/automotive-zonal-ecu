# TC-IO-003: 외부 버튼 GPIO 입력 테스트

## 1. 테스트 ID

TC-IO-003

## 2. 테스트 이름

외부 버튼 GPIO 입력 테스트

## 3. 관련 단계

Phase 3-3: Button Polling Input

## 4. 목적

STM32 NUCLEO-G474RE 보드가 외부 디지털 버튼 입력을 GPIO 핀을 통해 정상적으로 읽을 수 있는지 HAL GPIO polling 방식으로 검증한다.

이 테스트는 이후 AUTOSAR-inspired Front Zonal ECU 펌웨어 프로토타입에서 버튼 입력을 fault injection trigger로 확장하기 위한 기초 입력 검증 단계이다.

## 5. 테스트 환경

- Board: NUCLEO-G474RE
- MCU: STM32G474RET6
- IDE: STM32CubeIDE 2.2.0
- Configuration Tool: STM32CubeMX
- Firmware Project: front_zonal_ecu_firmware_v2
- Development Method: HAL 기반 bare-metal polling
- Input Device: 외부 tact switch
- Output Devices:
  - 내장 LD2 LED
  - EXT_LED GPIO에 연결된 외부 LED

## 6. 핀 설정

| 기능 | 보드 핀 | STM32 핀 | User Label | 방향 | 설정 |
|---|---|---|---|---|---|
| 내장 LED | D13 | PA5 | LD2 | Output | GPIO_Output |
| 외부 LED | D3 | PB3 | EXT_LED | Output | GPIO_Output |
| 외부 버튼 | D2 | PA10 | USER_BTN | Input | GPIO_Input, Pull-up |

## 7. 배선

### 외부 버튼

외부 tact switch는 내부 Pull-up 설정을 사용하는 방식으로 연결하였다.

```text
D2 / PA10 ---- Tact Switch ---- GND
```

이 구성에서는 버튼 입력이 active-low로 동작한다.

```text
버튼 안 누름: GPIO_PIN_SET
버튼 누름   : GPIO_PIN_RESET
```

### 외부 LED

외부 LED는 다음과 같이 연결하였다.

```text
D3 / PB3 ---- Resistor ---- LED ---- GND
```

## 8. 테스트 절차

1. STM32CubeMX에서 PA10을 GPIO_Input으로 설정한다.
2. PA10의 내부 Pull-up을 활성화한다.
3. User Label을 USER_BTN으로 지정한다.
4. main.h에 USER_BTN_Pin, USER_BTN_GPIO_Port가 생성되었는지 확인한다.
5. 외부 tact switch를 Arduino D2와 GND 사이에 연결한다.
6. main.c의 USER CODE 영역에 HAL_GPIO_ReadPin() 기반 polling 코드를 추가한다.
7. LD2와 EXT_LED를 사용하여 버튼 상태를 시각적으로 확인한다.
8. 프로젝트를 빌드한다.
9. 펌웨어를 NUCLEO-G474RE 보드에 다운로드한다.
10. 외부 버튼을 누르고 뗀다.
11. LD2와 EXT_LED의 동작을 관찰한다.

## 9. 기대 결과

- 버튼을 누르지 않았을 때:
  - USER_BTN은 GPIO_PIN_SET으로 읽힌다.
  - LD2와 EXT_LED는 OFF 상태를 유지한다.

- 버튼을 눌렀을 때:
  - USER_BTN은 GPIO_PIN_RESET으로 읽힌다.
  - LD2와 EXT_LED가 ON 된다.

- 버튼을 뗐을 때:
  - USER_BTN은 다시 GPIO_PIN_SET으로 돌아온다.
  - LD2와 EXT_LED가 OFF 된다.

## 10. 실제 결과

- 외부 tact switch를 Arduino D2와 GND 사이에 연결하였다.
- Arduino D2는 STM32 PA10에 매핑되어 있다.
- PA10은 GPIO_Input 및 내부 Pull-up으로 설정하였다.
- 버튼을 누르지 않았을 때 입력 상태는 GPIO_PIN_SET으로 읽혔다.
- 버튼을 눌렀을 때 입력 상태는 GPIO_PIN_RESET으로 읽혔다.
- LD2와 EXT_LED는 버튼 상태에 따라 정상적으로 반응하였다.
- HAL_GPIO_ReadPin()을 이용해 버튼 입력을 정상적으로 읽을 수 있음을 확인하였다.

## 11. 결과

PASS

## 12. 비고

버튼 입력은 내부 Pull-up을 사용하므로 active-low 방식으로 동작한다.

이 테스트는 기본 GPIO 입력 polling 동작을 검증하며, 이후 functional-safety-inspired ECU 펌웨어 구조에서 fault injection 로직으로 확장하기 위한 기반이 된다.

이후 단계에서는 버튼 입력을 다음과 같은 fault injection request 경로로 추상화할 수 있다.

```text
Button Input
→ Debounced Button Event
→ Fault Injection Request
→ SafetyManager
→ Dem_Lite Event
→ Safe State Transition
```
