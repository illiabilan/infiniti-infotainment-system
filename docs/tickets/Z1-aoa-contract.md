# Z1: контракт USB-сесії (AOA)

Тікет: `Z1-aoa.md`, AA-Z1-T1. Маркери: `docs/traces/README.md`, рядок Z1. Реалізація: `core/aa/usb/` (AA-Z1-T2). Replay: `core/aa/tests/` (AA-Z1-T3).

## Стани

| Стан | Значення |
|---|---|
| Idle | Кор не веде жодного телефона і чекає кандидата. Початковий стан. |
| Switching | Кандидату надіслано AOA-запити 51, 52 ×6, 53. Кор чекає accessory (18d1:2d00 – 2d05) не довше 2000 мс. |
| AccessoryUp | Accessory відкрито, інтерфейс 0 захоплено. Далі сесію веде Z2. |
| Failed | Перемикання не вдалося. Вихід: новий кандидат, accessory або відключення пристрою. |

## Переходи і маркери

Формат рядка: `aa-usb: usb:<серійний номер>: <маркер>`. Без серійного номера: `usb:phone`. Стартовий рядок без пристрою: `aa-usb: watching for phones`. Рядки `aoap:` і `reset:` йдуть як warning, решта як info.

| # | З | Подія | До | Маркер | Траса LIVI |
|---|---|---|---|---|---|
| 1 | старт | Кор почав стежити за USB | Idle | `watching for phones` | A:48, 19:45:03.374 |
| 2 | будь-який | Accessory, підключений на старті Кора | без змін | `accessory left over from an earlier run, resetting it`, при помилці `reset: <помилка>` | A:49, 19:45:03.374 |
| 3 | Idle, Failed | Підключено кандидата | Switching | `phone candidate <vid>:<pid>, switching to accessory mode` | A:64, 20:02:02.872. B:57, 22:19:37.093 |
| 4 | Switching | Телефон прийняв AOA-запити | Switching | `accessory mode requested` | A:65, 20:02:02.885. B:58, 22:19:37.106 |
| 5 | Switching, Failed | Прибув accessory і відкрився | AccessoryUp | `accessory up in <N> ms` (LIVI: `accessory up, session socket ...`) | A:67, 20:02:03.890. B:59, 22:19:38.107 |
| 6 | Switching | Помилка AOA-запиту | Failed | `aoap: <помилка>` | у трасах не було |
| 7 | Switching | 2000 мс без accessory | Failed | `aoap: no accessory within 2000 ms` | у трасах не було |
| 8 | Switching, Failed | Accessory не відкрився | Failed | `aoap: open accessory: <помилка>` | у трасах не було |
| 9 | AccessoryUp, Failed | Відключено пристрій сесії | Idle | `closed (phone unplugged)` | A:218, 20:02:41.674 (у LIVI цей рядок пише aa-session) |

Події без переходу і без маркера:
- Пристрій сесії зник у Switching. Це переенумерація телефона після запиту 53.
- Кандидат у Switching або AccessoryUp. Z1 веде один телефон.
- Accessory поза стартом у Idle або AccessoryUp. Як у LIVI, accessory, який Кор не перемикав, не відкривається.
- Результат AOA-запиту, що прийшов після переходу в Failed.

## Таймаути

- Перемикання: від `phone candidate` до `accessory up` не довше 2000 мс, далі перехід 7. Траси: 1018 мс (A 20:02:02.872 → 03.890), 1006 мс (A 21:53:30.813 → 31.819), 1014 мс (B 22:19:37.093 → 38.107). Траси зняті LIVI на macOS, не на ROCK 5B+.
- Один AOA-запит: 1000 мс (aasdk `AccessoryModeQuery::cTransferTimeoutMs`). LIVI чекає 2 с на запит.
- Кор не чекає 1000 мс перед запитами. Цю затримку має aasdk `USBHub` (`src/USB/USBHub.cpp:108`), тому Кор не використовує `USBHub`.

## Залишок від попереднього запуску

- Перевіряються лише пристрої, підключені на старті Кора (як `at_startup` у LIVI).
- Скидання: `libusb_reset_device`. Відповідь `LIBUSB_ERROR_NOT_FOUND` означає, що пристрій переенумерувався, і вважається успіхом.
- Після скидання телефон повертається як кандидат, далі перехід 3. Ручне втручання не потрібне.

## Відомий дефект aasdk: AOAPDevice

- Дефект: `AOAPDevice::create` (`src/USB/AOAPDevice.cpp:59-73`, коміт `9bf6adf`) зберігає `interfaceDescriptor_` як вказівник усередину config descriptor, який звільняється, коли `create` повертає керування. Деструктор `AOAPDevice` читає цей вказівник (`src/USB/AOAPDevice.cpp:46`). Це use-after-free.
- Вплив на Кор: `AasdkUsbPort` створює `AOAPDevice` у `openAccessory` і знищує його при відключенні пристрою та при завершенні Кора. Кожен цикл підключення і відключення в T4 виконує цей код.
- Рішення власника до Z2: або патч сабмодуля через форк aasdk, або захоплення інтерфейсу в Корі без деструктора `AOAPDevice` з aasdk. Z1 не змінює сабмодуль.
- Запуски T4: робити зі збіркою з AddressSanitizer або враховувати, що збій чи сміття на відключенні може бути цим дефектом, а не помилкою Z1.

## Поза Z1

- `AaSessionState::Phase::Accessory` і поле `phase` протоколу v1 не змінюються. Перехід сесії в `accessory` робить Z2.
- Handshake, TLS, читання bulk-endpoints (Z2).
- Скидання після сесії і повтори на Busy (Z8).
- Кілька телефонів одночасно.
