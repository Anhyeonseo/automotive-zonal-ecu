# STM32 Front Zonal ECU Requirements

## 1. Scope

이 문서는 현재 NUCLEO-G474RE에서 실행되는 Front Zonal ECU firmware baseline의
검증 가능한 요구사항을 정의합니다. Raspberry Pi Central Computer, 외부 CAN FD bus,
FreeRTOS, Yocto와 UDS는 현재 요구사항 범위에 포함하지 않습니다.

## 2. Input and sensor requirements

| ID | Requirement | Current verification |
|---|---|---|
| `IN-001` | ECU는 ADC1_IN1을 10 ms 이상 간격으로 sample해야 한다. | Host unit test + board observation |
| `IN-002` | 12-bit ADC 값은 `0..4095` 범위와 `0..1000` permille로 표현해야 한다. | Host unit test + potentiometer boundary test |
| `IN-003` | ADC conversion 실패 시 `adc_valid=0`과 error counter를 기록해야 한다. | Host unit test |
| `IN-004` | Zone switch는 raw 값이 20 ms 유지된 후 debounced state를 변경해야 한다. | Host unit test + board button test |
| `SEN-001` | VL53L0X는 I2C readiness와 API initialization 결과를 별도로 기록해야 한다. | Board observation |
| `SEN-002` | 거리 측정은 superloop를 장시간 block하지 않는 state machine으로 처리해야 한다. | Code review + live distance update |
| `SEN-003` | 거리 측정이 100 ms 안에 완료되지 않으면 timeout counter와 invalid 상태를 기록해야 한다. | Implementation inspection; explicit fault test pending |

## 3. Communication requirements

| ID | Requirement | Current verification |
|---|---|---|
| `COM-001` | Front Zonal Status는 standard ID `0x180`, CAN FD 16-byte payload로 전송해야 한다. | Codec unit test + internal loopback |
| `COM-002` | Front Zonal Status 전송 주기는 100 ms이어야 한다. | Board debugger observation |
| `COM-003` | Central Control Command는 standard ID `0x200`, CAN FD 8-byte payload여야 한다. | Codec unit test + internal loopback |
| `COM-004` | 임시 Central Simulator는 200 ms 주기로 command를 생성해야 한다. | Host unit test + internal loopback |
| `COM-005` | 각 cyclic frame은 4-bit alive counter를 포함해야 한다. | Codec unit test |
| `COM-006` | 수신 측은 payload length, value range와 alive counter 연속성을 진단해야 한다. | Unit test + debugger counters |
| `COM-007` | FDCAN RX는 FIFO0 new-message interrupt로 처리해야 한다. | Call stack and RX counter observation |
| `COM-008` | Front Status loopback RX가 300 ms 넘게 없으면 timeout을 기록해야 한다. | Debugger fault observation and recovery test |

## 4. Application and safety requirements

| ID | Requirement | Current verification |
|---|---|---|
| `APP-001` | ECU state는 `INIT`, `NORMAL`, `DEGRADED`, `SAFE` 중 하나여야 한다. | Codec validation + application unit test |
| `SAFE-001` | 유효한 Central Command를 500 ms 넘게 받지 못하면 모든 command-controlled LED 출력을 꺼야 한다. | Application unit test |
| `SAFE-002` | command timeout 동안 ECU는 `SAFE` 상태여야 한다. | Application unit test |
| `SAFE-003` | 거리센서가 초기화되지 않았으면 ECU는 `INIT` 상태여야 한다. | Application unit test + board startup observation |
| `SAFE-004` | 거리센서가 준비됐지만 측정값이 invalid이면 ECU는 `DEGRADED` 상태여야 한다. | Application unit test |
| `SAFE-005` | fault injection 활성화 시 status는 distance invalid, range status `255`, sensor not-ready를 보고해야 한다. | Application unit test |
| `SAFE-006` | Central Simulator는 status가 300 ms보다 오래됐거나 invalid이면 warning request를 꺼야 한다. | Central Simulator unit test |
| `APP-002` | obstacle warning은 200 mm 이하에서 켜지고 250 mm 이상에서 꺼지는 hysteresis를 가져야 한다. | Central Simulator unit test + board LED observation |

## 5. Diagnostic requirements

| ID | Requirement | Current verification |
|---|---|---|
| `DIAG-001` | CAN encode, HAL TX, payload, sequence, timeout, unknown-ID 오류를 독립 counter로 기록해야 한다. | Debugger Expressions observation |
| `DIAG-002` | 가장 최근 CAN service 오류 원인을 enum 값으로 보존해야 한다. | Debugger Expressions observation |
| `DIAG-003` | 시간 비교는 interrupt 갱신 순서와 32-bit tick rollover에서 false timeout을 만들지 않아야 한다. | Time utility unit test + board regression test |

## 6. Out of scope for this baseline

- 실제 Raspberry Pi–STM32 external CAN FD 통신
- bus-off detection and recovery
- CRC/E2E protection
- watchdog supervision
- FreeRTOS scheduling and task monitoring
- persistent DTC and freeze frame
- ISO-TP/UDS
- Device Tree, SocketCAN service and Yocto image
