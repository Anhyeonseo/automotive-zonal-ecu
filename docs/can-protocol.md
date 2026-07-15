# CAN FD Protocol

## 1. Baseline network configuration

| Property | Value |
|---|---|
| Identifier format | 11-bit standard ID |
| Frame format | CAN FD with BRS |
| Arbitration rate | 500 kbit/s |
| Data rate | 2 Mbit/s |
| Multi-byte encoding | Little-endian |
| Current transport | STM32 FDCAN internal loopback |

## 2. Message summary

| CAN ID | Message | Sender | Receiver | Length | Cycle |
|---:|---|---|---|---:|---:|
| `0x180` | `FrontZonalStatus` | Front Zonal ECU | Central Computer | 16 bytes | 100 ms |
| `0x200` | `CentralControlCommand` | Central Computer | Front Zonal ECU | 8 bytes | 200 ms in current simulator |

The 200 ms command cycle is a property of the temporary STM32
`CentralSimulator`. The future Linux Central Computer may change the cycle only after the
receiver timeout contract is reviewed.

## 3. FrontZonalStatus — ID 0x180

| Byte | Field | Encoding | Valid range |
|---:|---|---|---|
| 0 | `alive_counter` | Low 4 bits | `0..15`, rolling |
| 1 | `ecu_state` | Enum | `0..3` |
| 2..5 | `uptime_ms` | `uint32`, little-endian | `0..0xFFFFFFFF` |
| 6..7 | `adc_raw` | `uint16`, little-endian | `0..4095` |
| 8..9 | `position_permille` | `uint16`, little-endian | `0..1000` |
| 10 | `adc_valid` | Boolean | `0` or `1` |
| 11 | `zone_switch` | Boolean | `0` or `1` |
| 12..13 | `distance_mm` | `uint16`, little-endian | Sensor-dependent |
| 14 | `range_status` | VL53L0X status | `0` means valid ranging status |
| 15 bit 0 | `distance_valid` | Boolean flag | `0` or `1` |
| 15 bit 1 | `vl53l0x_ready` | Boolean flag | `0` or `1` |
| 15 bits 2..7 | Reserved | Must be zero | `0` |

ECU state values:

| Value | State | Meaning |
|---:|---|---|
| 0 | `INIT` | Distance sensor is not ready |
| 1 | `NORMAL` | Command and distance data are valid |
| 2 | `DEGRADED` | Command is fresh but distance data is invalid/faulted |
| 3 | `SAFE` | Central command is missing or stale |

Payload validation rejects the frame when ADC, permille, boolean fields or ECU state
are outside the defined range. `distance_valid=1` additionally requires
`range_status=0` and `vl53l0x_ready=1`.

## 4. CentralControlCommand — ID 0x200

| Byte | Field | Encoding | Valid range |
|---:|---|---|---|
| 0 | `alive_counter` | Low 4 bits | `0..15`, rolling |
| 1 | `led_request` | Boolean | `0` or `1` |
| 2 | `warning_led_request` | Boolean | `0` or `1` |
| 3..7 | Reserved | Transmitter writes zero | Reserved |

The current decoder validates the two request values. Reserved command bytes are
written as zero by the project transmitter but are not yet rejected on reception.

## 5. Alive counter and timeout behavior

- Each successfully transmitted message increments its own 4-bit counter modulo 16.
- The receiver compares a new counter with the expected next value.
- A discontinuity increments a sequence-error counter.
- A sequence error is diagnostic information; it does not by itself refresh or replace
  the application safe-state policy.
- Front Status loopback RX timeout: 300 ms.
- Central Command application timeout: 500 ms.
- Central Simulator distance freshness timeout: 300 ms.

## 6. Current warning policy

The temporary Central Simulator converts distance status into a warning request.

```text
valid distance <= 200 mm  -> warning ON
valid distance >= 250 mm  -> warning OFF
200 mm < distance < 250 mm -> keep previous state
invalid or stale distance -> warning OFF
```

The 50 mm gap is hysteresis. It prevents output chatter when the measured distance
oscillates around one threshold.

## 7. CAN service diagnostics

| Reason | Enum value | Meaning |
|---|---:|---|
| None | 0 | No recorded error |
| Front Status encode | 1 | Status payload could not be encoded |
| Front Status HAL TX | 2 | HAL rejected status TX enqueue |
| Central Command encode | 3 | Command payload could not be encoded |
| Central Command HAL TX | 4 | HAL rejected command TX enqueue |
| Front Status RX timeout | 5 | No status loopback RX inside 300 ms |
| Central Command payload | 6 | Invalid command length or value |
| Unknown RX frame | 7 | ID/DLC does not match either contract |
| Front Status payload | 8 | Invalid status length or value |

Separate counters are retained for TX, RX, sequence, payload, encode, HAL and timeout
events so a single generic error flag does not hide the failure source.

## 8. Not implemented yet

- CRC or E2E protection
- Explicit command validity/expiry field
- DBC source of truth
- CAN bus-off recovery policy
- Physical two-node CAN FD verification
- ISO-TP and diagnostic messages

