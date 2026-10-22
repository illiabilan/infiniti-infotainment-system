# Z7: Навігація (K2, CL4)

Джерело: primer розділ 12. Траса B: `NavigationChannel] status=Active` 22:19:51.718, `NavigationFocusRequest raw: 0802` 51.722, `status=Inactive` 22:27:22.084, `NavigationFocusRequest raw: 0801` 22.186.

Підтверджено трасами: дані навігації доходять до Кора, поля відомі (STATE, CURRENT_POSITION). Зупинка дає STATUS Inactive, потім NavFocusRequest з типом 1. STOP (0x8002) не був.

## AA-Z7-T1: Контракт

АС:
- [ ] Повідомлення ch 12: START 0x8001, STOP 0x8002, STATUS 0x8003, NEXT_TURN 0x8004, NEXT_TURN_DISTANCE 0x8005, STATE 0x8006, CURRENT_POSITION 0x8007.
- [ ] Відповідь на NavFocusRequest (ch 0, msgId 0x000d): echo payload як NavFocusNotification (LIVI `control.rs:126 – 131`). Тип 2 при старті, тип 1 при зупинці.
- [ ] Таблиця полів для клієнтів: тип маневру (enum NavigationManeuverType), дорога, адреса призначення, відстань до маневру (step), відстань до призначення (dest), час до прибуття, ETA як текст.
- [ ] Формат для клієнтів Кора (WebSocket): ключі полів, частота ~1 с.

## AA-Z7-T2: Реалізація

Область: `core/aa/navigation/`.

АС:
- [ ] Відповідь на NavFocusRequest надсилається.
- [ ] STATUS, STATE, CURRENT_POSITION парсяться. NEXT_TURN і NEXT_TURN_DISTANCE теж парсяться (LIVI їх обробляє, інші телефони можуть слати їх).
- [ ] Кор публікує дані клієнтам. Кластер (K2) і кліматичний навігаційний блок (CL4) отримують маневр і відстань.
- [ ] Після STATUS Inactive клієнти очищають дані.
- [ ] Файли до 700 рядків.

## AA-Z7-T3: Replay

АС:
- [ ] Сценарій "старт": STATUS Active → NavFocus type 2 → STATE → CURRENT_POSITION.
- [ ] Сценарій "зупинка": STATUS Inactive → NavFocus type 1.
- [ ] Фікстури байтові (потрібен дамп з `TRACE=1` у LIVI або власний). Логи з полями самі по собі фікстурою не є.

## AA-Z7-T4: Валідація на залізі

АС:
- [ ] Старт маршруту в Google Maps: дані з'являються в клієнті кластера і в CL4-блоку.
- [ ] Зупинка: клієнти очищають дані (маркер STATUS Inactive).
- [ ] Затримка від повідомлення до відображення виміряна і записана.
- [ ] Трасу збережено в `docs/traces/`.
