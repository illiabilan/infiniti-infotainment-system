# Infiniti G35 Climate Ribbon

Compact 1920x480 horizontal climate strip: dual-zone temperatures, HVAC status, outside temp, and clock.

![Climate ribbon](../docs/images/climate-ribbon.png)

Run: `just run-clima` (from repo root).

## Climate screen

Four blocks separated by 1px dividers, left to right:

1. **Driver temp** — setpoint in °C with `Driver` label.
2. **Climate block** — `CLIMATE` header, a middle row with rear-defrost, airflow direction, and fan icons, and a bottom chip row with `A/C`, `AUTO`, `RECIRC`, `ECO`. Each chip shows its icon plus label and lights accent when active.
3. **Outside + clock** — outside temp zone, divider, then time (56px) over date (24px).
4. **Passenger temp** — mirrors the driver block with a `Passenger` label.

## Logic

- **Dual mode**: when `dualMode` is on, both temp zones show a green `Dual` label. When off, they show `Driver` / `Passenger`.
- **Airflow direction**: the icon follows `airflowMode` — face, face+feet, feet, or defrost+feet.
- **Rear defrost**: grey when off, green when on.
- **Fan**: 7 bars for speeds 1–7, `fan-max` icon at top speed.
- **HVAC off**: at fan speed 0 the driver, passenger, and climate blocks dim to 40% opacity and the fan group is replaced by a greyed `Off` label. Outside temp and clock stay live.
- **ECO mode**: carried on flags bit 6 of CAN ID `0x54C`, from `VehicleState.ecoMode` through `ClimateRibbonController.ecoActive` to the ECO chip, and toggleable in the emulator (`EmulatorController.ecoActive`, HVAC grid).
- **Transport**: SocketCAN on `vcan0` when present, otherwise multicast mock CAN fallback, so the emulator drives the screen with no setup.
