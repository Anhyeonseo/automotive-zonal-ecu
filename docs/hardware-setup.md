# Hardware Setup

## 1. Current bench

| Component | Role | Current status |
|---|---|---|
| NUCLEO-G474RE | Front Zonal ECU | In use |
| Potentiometer | Analog zone input | Verified |
| VL53L0X | Distance sensor | Verified |
| On-board LD2 | Command-controlled output | Verified |
| External red LED + series resistor | Obstacle warning output | Verified |
| ST-LINK USB | NUCLEO power, flash and debug | In use |
| Logic analyzer / multimeter | Bring-up and electrical checks | Available |
| Raspberry Pi 5 4 GB | Future Central Computer | Integration pending |
| MCP2518FD module | Future Pi CAN FD controller | Integration pending |
| TJA1051T/3 module | Future STM32 CAN transceiver | Integration pending |

## 2. MCU and clock

- MCU board: `NUCLEO-G474RE`
- External clock input: 24 MHz HSE
- System clock: 170 MHz
- FDCAN peripheral clock: 170 MHz PCLK1
- Development power: ST-LINK USB

## 3. Current pin map

| Signal | STM32 pin | Configuration | Usage |
|---|---|---|---|
| Analog input | `PA0 / ADC1_IN1` | 12-bit ADC, single-ended | Potentiometer wiper |
| Zone switch | `PC13` | GPIO input | Debounced switch input |
| Status LED | `PA5 / LD2` | Push-pull output | Central command LED request |
| Warning LED | `PA8` | Push-pull output | Distance warning request |
| VL53L0X SCL | `PB8 / I2C1_SCL` | I2C open-drain | Distance sensor clock |
| VL53L0X SDA | `PB9 / I2C1_SDA` | I2C open-drain | Distance sensor data |
| FDCAN RX | `PA11 / FDCAN1_RX` | Alternate function 9 | External transceiver connection pending |
| FDCAN TX | `PA12 / FDCAN1_TX` | Alternate function 9 | External transceiver connection pending |
| Reserved output | `PB3` | Push-pull output | Configured but not used by current application |

## 4. Current wiring

### Potentiometer

```text
NUCLEO 3.3 V  -> potentiometer end terminal
NUCLEO GND    -> potentiometer opposite terminal
PA0 / A0      -> potentiometer center wiper
```

ADC input에는 5 V를 연결하지 않습니다. 현재 conversion 기준 전압은 3.3 V이며,
raw `0..4095`를 `0..3300 mV`와 `0..100%`로 변환합니다.

### Warning LED

```text
PA8 -> current-limiting resistor -> LED -> GND
```

현재 bench에서는 220 ohm series resistor를 사용할 수 있습니다. LED 극성과 실제
점퍼 위치는 최종 배선 사진과 함께 기록합니다.

### VL53L0X

```text
VL53L0X +/VCC -> module-supported supply
VL53L0X -/GND -> NUCLEO GND
VL53L0X C/SCL -> PB8
VL53L0X D/SDA -> PB9
```

현재 firmware는 I2C address `0x29`를 사용합니다. `GPIO1`과 `XSHUT`은 현재
firmware에서 사용하지 않습니다. 전원 전압은 실제 module 표기와 regulator 유무를
우선 확인합니다.

## 5. Current FDCAN configuration

- Frame format: CAN FD with bit-rate switching
- Mode: internal loopback
- Arbitration rate: 500 kbit/s
- Data rate: 2 Mbit/s
- Standard ID filters: 2
- RX path: FIFO0 new-message interrupt
- Physical CANH/CANL bus: not connected in this baseline

Internal loopback은 FDCAN controller, message RAM, filter, interrupt, codec와
application flow를 검증하지만 transceiver, termination, wiring과 bus noise는
검증하지 않습니다.

## 6. External CAN FD integration checklist

하드웨어 도착 후 다음 항목을 별도 단계에서 검증합니다.

- TJA1051T/3 module의 실제 `VCC/GND/RX/TX/SLNT/CANH/CANL` label
- MCU logic voltage compatibility
- `SLNT`의 normal-mode logic level
- 전원을 끈 상태의 CANH-CANL resistance
- bus 양 끝의 120 ohm termination; 전체 측정값 약 60 ohm
- STM32 TX/RX와 transceiver TX/RX 방향
- 공통 GND
- 짧은 twisted CANH/CANL pair
- FDCAN mode를 internal loopback에서 normal mode로 변경
- Raspberry Pi MCP2518FD oscillator, interrupt, SPI chip select 설정

