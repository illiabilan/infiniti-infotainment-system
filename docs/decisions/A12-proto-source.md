# A12: Джерело AA `.proto`

Статус: прийнято. Дата: 2026-10-20.

## Рішення

P1: `protobuf/aap_protobuf/` з репо aasdk. Фіксуємо коміт aasdk у git submodule, наприклад `third_party/aasdk`. Збірка через CMake aasdk або його `protobuf/CMakeLists.txt`.

## Причини

- aasdk вже обрано як бібліотеку протоколу (D2). Імена типів у `#include` і в коді збігаються без перейменувань.
- Набір повний: 291 файл, по одному повідомленню на файл.
- Ліцензія: GPLv3 успадковується від aasdk (D6). Файли без власних заголовків ліцензії.

## Перевірка покриття (за файлами P1)

| Потрібно для | Тип у P1 | Файл |
|---|---|---|
| Версія, TLS, AuthComplete | VersionRequest, AuthResponse | `service/control/message/AuthResponse.proto` |
| ServiceDiscovery | ServiceDiscoveryRequest, ServiceDiscoveryResponse | `service/control/message/ServiceDiscoveryResponse.proto` |
| Відео setup і старт | Setup, Config, Start | `service/media/shared/message/Setup.proto`, `Config.proto`, `Start.proto` |
| Фокус відео | VideoFocusNotification | `service/media/video/message/VideoFocusNotification.proto` |
| ACK кадрів | Ack | `service/media/source/message/Ack.proto` |
| Навігація | NavigationStatus, NavigationState, NavigationCurrentPosition, NavigationNextTurnEvent, NavigationNextTurnDistanceEvent | `service/navigationstatus/message/*.proto` |

Примітка до primer: повідомлення називається `Setup`, а не `MediaSetup`. Primer треба поправити (зроблено).

## Ризики

- Версія протоколу. Трасa з телефоном показує protocol 6.1. Чи підтримує aasdk версію 6.1 без змін, перевіряється в Z2 T1.
- Походження файлів: це реверс-інжиніринг, не офіційні файли Google. Ризик зафіксовано в R11 дослідження.
- Оновлення aasdk може змінити імена. Коміт фіксується в submodule.

## Дія

- Тікети Z2 T1 і Z3 T1 посилаються на цей файл.
- EPIC: залежність A12 закрита.
