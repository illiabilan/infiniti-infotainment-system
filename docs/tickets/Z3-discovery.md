# Z3: ServiceDiscovery і ChannelOpen

Джерело: primer розділ 4 кроки 5 – 8 і розділ 5. Траса B: `SDR: 13 channels, 582B`, 13 ChannelOpen за ~13 мс.

Залежності: A12 (P1: NavigationStatus, ServiceDiscoveryResponse, MediaSetup у `protobuf/aap_protobuf/`), A14 (для T4).

## AA-Z3-T1: Контракт

АС:
- [ ] Список оголошених сервісів з ID: video 3, input 8, cluster input 20, cluster video 19 (опційно), audio 4, 5, 6, sensor 1, mic 9, bluetooth 10 (бездротовий), navigation 12, media info 13, phone status 14.
- [ ] Параметри відео: 1920x1080 для головного дисплея, DPI і відступи з конфігу.
- [ ] Для H.264 роздільність ≤ 1080p (коментар LIVI `config.rs:79`). Вище 1080p потрібен H.265.
- [ ] Один кодек на дисплей (коментар LIVI `discovery.rs:52`). H.264 обов'язковий (D10).

## AA-Z3-T2: Реалізація

Область: `core/aa/discovery/`. Побудова ServiceDiscoveryResponse з конфігу.

АС:
- [ ] Профіль H.264 присутній завжди.
- [ ] Профіль H.265 додається тільки конфігом.
- [ ] Вибір кодека за `phone_make_and_model` з ServiceDiscoveryRequest (D10). Для невідомої моделі діє H.264.
- [ ] ChannelOpenRequest на всі оголошені канали отримують OK.
- [ ] Файли до 700 рядків.

## AA-Z3-T3: Replay

АС:
- [ ] Побудований SDR розбирається назад, набір ID збігається з T1.
- [ ] Вхідний SDR-hex береться з трасу A (`0a3808031a34...`) як контрольний приклад: поля id 3, codec 7, dpi 180 відповідають.
- [ ] Один тест на вибір кодека за моделлю телефона.

## AA-Z3-T4: Валідація на залізі

АС:
- [ ] Pixel 9 Pro показує екран проєкції (маркери Z3 у трасі).
- [ ] Другий телефон з H.264 (A14) показує екран проєкції.
- [ ] 10 з 10 ChannelOpen OK на кожному телефоні.
- [ ] Трасу збережено в `docs/traces/`.
