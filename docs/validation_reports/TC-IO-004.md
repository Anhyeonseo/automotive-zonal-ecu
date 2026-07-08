# TC-IO-004: 버튼 디바운스 및 엣지 검출 테스트

## 1. 테스트 ID

TC-IO-004

## 2. 테스트 이름

버튼 디바운스 및 엣지 검출 테스트

## 3. 관련 단계

Phase 3-4: Button Debounce

## 4. 목적

STM32 NUCLEO-G474RE 보드가 외부 tact switch 입력을 소프트웨어 디바운스 로직으로 안정화하고, 하나의 유효한 버튼 눌림 이벤트를 정확히 검출할 수 있는지 확인한다.

이 테스트는 외부 버튼 입력을 이후 AUTOSAR-inspired Front Zonal ECU 펌웨어 프로토타입에서 안정적인 fault injection trigger로 사용하기 위한 단계이다.

## 5. 테스트 환경

- Board: NUCLEO-G474RE
- MCU: STM32G474RET6
- IDE: STM32CubeIDE 2.2.0
- Configuration Tool: STM32CubeMX
- Firmware Project: front_zonal_ecu_firmware_v2
- Development Method: HAL 기반 polling
- Logic Analyzer Software: PulseView
- Logic Analyzer Device: fx2lafw 호환 USB logic analyzer
- Input Device: 외부 tact switch
- Output Devices:
  - 내장 LD2 LED
  - EXT_LED GPIO에 연결된 외부 LED

## 6. 핀 설정

| 기능 | 보드 핀 | STM32 핀 | User Label | 방향 | 설정 |
|---|---|---|---|---|---|
| Raw Button Input | D2 | PA10 | USER_BTN | Input | GPIO_Input, Pull-up |
| Raw Input Indicator | D13 | PA5 | LD2 | Output | GPIO_Output |
| Debounced Event Output | D3 | PB3 | EXT_LED | Output | GPIO_Output |

## 7. 배선

### 외부 버튼

```text
D2 / PA10 ---- Tact Switch ---- GND
```

버튼 입력은 STM32 내부 Pull-up 저항을 사용한다.

따라서 입력 논리는 active-low이다.

```text
버튼 안 누름: GPIO_PIN_SET
버튼 누름   : GPIO_PIN_RESET
```

### 외부 LED

```text
D3 / PB3 ---- Resistor ---- LED ---- GND
```

### 로직 애널라이저 연결

```text
Logic Analyzer CH1 ---- D2 / PA10 / Raw Button Input
Logic Analyzer CH2 ---- D3 / PB3 / Debounced Output
Logic Analyzer GND ---- NUCLEO GND
```

## 8. 소프트웨어 설계

디바운스 로직은 raw GPIO 입력과 안정화된 버튼 상태를 분리하여 처리한다.

펌웨어는 다음 순서로 동작한다.

1. HAL_GPIO_ReadPin()을 사용해 raw GPIO 입력을 읽는다.
2. raw 입력 상태가 바뀌면 debounce timer를 초기화한다.
3. 입력이 debounce delay보다 오래 동일하게 유지되면 stable button state로 인정한다.
4. released 상태에서 pressed 상태로 바뀌는 falling edge를 유효 버튼 이벤트로 검출한다.
5. 유효한 debounced button press마다 EXT_LED를 한 번만 toggle한다.
6. LD2는 raw 입력 상태를 시각적으로 표시하는 용도로 사용한다.

디바운스 지연 시간은 다음과 같이 설정하였다.

```text
50 ms
```

## 9. 테스트 절차

1. PA10을 GPIO_Input 및 내부 Pull-up으로 설정한다.
2. 외부 tact switch를 Arduino D2와 GND 사이에 연결한다.
3. PA5를 LD2 출력으로 설정한다.
4. PB3를 EXT_LED 출력으로 설정한다.
5. HAL_GetTick() 기반 소프트웨어 디바운스 로직을 추가한다.
6. GPIO_PIN_SET에서 GPIO_PIN_RESET으로 전환되는 falling edge를 검출한다.
7. 유효한 버튼 입력 1회마다 EXT_LED가 한 번만 toggle되도록 구현한다.
8. 프로젝트를 빌드하고 펌웨어를 다운로드한다.
9. 로직 애널라이저를 연결한다.
   - CH1: D2 / PA10
   - CH2: D3 / PB3
   - GND: NUCLEO GND
10. PulseView를 열고 fx2lafw 호환 logic analyzer를 선택한다.
11. raw button input과 debounced output 파형을 캡처한다.
12. 버튼을 한 번 누르고 LED 및 파형 동작을 관찰한다.
13. 버튼을 길게 눌렀을 때 EXT_LED가 반복 toggle되지 않는지 확인한다.

## 10. 기대 결과

- LD2는 버튼을 물리적으로 누르고 있는 동안 ON 된다.
- LD2는 버튼을 떼면 OFF 된다.
- EXT_LED는 유효한 버튼 눌림 1회마다 한 번만 toggle된다.
- 버튼을 길게 눌러도 EXT_LED가 반복적으로 toggle되지 않는다.
- PA10의 raw button input은 HIGH와 LOW 사이에서 전환된다.
- PB3의 debounced output은 유효한 버튼 이벤트 1회에 대해 한 번만 변화한다.
- 접점 bouncing이 발생하더라도 여러 개의 소프트웨어 이벤트가 발생하지 않는다.

## 11. 실제 결과

- 외부 tact switch를 Arduino D2와 GND 사이에 연결하였다.
- Arduino D2는 STM32 PA10에 매핑되어 있다.
- PA10은 GPIO_Input 및 내부 Pull-up으로 설정하였다.
- 버튼 입력은 active-low로 동작하였다.
  - 버튼 안 누름: GPIO_PIN_SET
  - 버튼 누름: GPIO_PIN_RESET
- LD2는 raw 입력 상태를 시각적으로 확인하는 indicator로 사용하였다.
- Arduino D3 / STM32 PB3에 연결된 EXT_LED는 debounced event output으로 사용하였다.
- PulseView에서 fx2lafw 호환 logic analyzer가 정상적으로 인식되었다.
- 로직 애널라이저를 통해 raw button input 파형을 정상적으로 캡처하였다.
- 테스트 조건에서는 버튼 신호가 명확한 bouncing 없이 비교적 깨끗하게 전환되는 것을 확인하였다.
- 다만 물리 스위치 입력의 안정성을 확보하기 위해 소프트웨어 디바운스 로직을 적용하였다.
- EXT_LED는 유효한 버튼 입력 1회마다 한 번만 toggle되었다.
- 버튼을 길게 눌러도 EXT_LED가 반복적으로 toggle되지 않았다.

## 12. 결과

PASS

## 13. 비고

이번 테스트 조건에서는 심한 mechanical bouncing이 명확하게 관찰되지는 않았지만, 물리 스위치 입력은 스위치 품질, 누르는 힘, 배선 상태, 노후화, 전기적 노이즈 등에 따라 달라질 수 있다.

따라서 소프트웨어 디바운스 로직은 단순한 편의 기능이 아니라 입력 신호 안정화 계층으로 취급한다.

ECU 펌웨어 아키텍처 관점에서 디바운스 로직은 불안정한 물리 입력 변화가 애플리케이션 로직으로 직접 전달되는 것을 막는 역할을 한다.

이후 단계에서는 이 로직을 main.c에서 분리하여 button driver 또는 board abstraction layer로 이동시키는 것이 적절하다.

향후 확장 구조는 다음과 같다.

```text
Raw GPIO Input
→ Button Debounce
→ Button Press Event
→ Fault Injection Request
→ SafetyManager
→ Dem_Lite Diagnostic Event
→ Safe State Transition
```
