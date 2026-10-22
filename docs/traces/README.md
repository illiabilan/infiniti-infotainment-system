# Трейси AA: карта маркерів

Призначення: для кожного Z описано, які рядки логу LIVI є доказом віхи. Тікети посилаються на цей файл. Трейси з агентів не приймаються без маркерів з цієї таблиці.

## Файли

| Файл | Зміст | Сесії |
|---|---|---|
| `livi-trace-A-2026-10-20-2154.log` | LIVI, DEBUG=1 | Сесія 1: 20:02:02 – 20:02:41 (відключення). Сесія 2: 21:53:30 (без відключення в логу) |
| `livi-trace-B-2026-10-20-2219.log` | LIVI, DEBUG=1 | Сесія 22:19:38, навігація 22:19:51 – 22:27:22, лог триває до 23:27 |

Обмеження: логи містять декодовані поля, а не сирі байти payload. Для replay-тестів (Z2 – Z7) потрібні байтові дампи. Їх дає `TRACE=1` у LIVI або власний дамп модуля.

## Маркери за Z

| Z | Маркер (точний текст або шаблон) | Де в трасі | Очікуваний показник |
|---|---|---|---|
| Z1 | `phone candidate 18d1:4ee7, switching – accessory mode` → `accessory up` | A: 20:02:02.872 → 03.890. B: 22:19:37.093 → 38.107 | ~1,0 с |
| Z2 | `tls up, protocol 6.1`, `AUTH_COMPLETE proto bytes: 0800` | B: 22:19:38.275, 38.276 | TLS ~0,17 с після accessory up. 0800 = status 0 |
| Z3 | `ServiceDiscoveryRequest label="Android" phone="Google Pixel 9 Pro"`, `SDR: 13 channels, 582B`, `CHANNEL_OPEN_REQUEST ch=... responding OK` | B: 22:19:38.282 → 38.3xx | 13 ChannelOpen за ~13 мс |
| Z4 | `video codec selected: h265`, `VideoChannel] stream started, session=5`, `decoded format=NV12 1280x720`, `stream stopped` | B: 38.407, 38.413, 39.048 | Перезапуск потоку через ~0,6 с, у всіх трьох сесіях |
| Z5 | `ch=8 KeyBindingRequest (len=57), replying status=OK` | B | InputReport у трасах не логується. Маркер додає T4 |
| Z6 | `AVSetupRequest ch=4 codec=1`, `audio media start (48000Hz 2ch)`, `AudioFocus type=4(RELEASE) -> state=3(LOSS)`, `AudioFocus type=1(GAIN)`, `audio media stop` | B: 22:19:38.828, 39.869, 48.315 | Ack на кожен пакет у трасі не видно |
| Z7 | `NavigationChannel] status=Active`, `NavigationFocusRequest raw: 0802`, `state maneuver=`, `position dest=`, `NavigationChannel] status=Inactive`, `NavigationFocusRequest raw: 0801` | B: 22:19:51.718, 51.722, 22:27:22.084, 22.186 | 346 пар STATE та CURRENT_POSITION за ~7,5 хв. STOP (0x8002) не було |
| Z8 | `usb read error`, `closed (phone unplugged)`, `session over, video=570 audio=309`, `accessory left over from an earlier run, resetting it`, `Ping sent (1500ms interval)`, `BT pairing request ... -> already paired` | A: 20:02:41.567 → 41.675, 20:02:04.046 (reset). B: кожні ~2 с на ch 10 | Повторне підключення в межах одного запуску LIVI: сесія 2 в трасі A без рестарту |
| CL-2 | `core: listening 127.0.0.1:<port> pid=<pid>`, `core: hello id=<client_id> role=<role> aa_session=<id>/<phase> keyframe=<sent або none>`, `core: client disconnected id=<client_id> role=<role> aa_session=<id>/<phase> kept` | Лог Кора: `docs/traces/cl2-core-lifecycle-<дата>.log` | Один рядок `listening` на запуск. Те саме значення `aa_session=` до і після перепідключення клієнта. PID у `listening` не змінюється |

## Карта ID повідомлень (за LIVI `navigation.proto` і `consts.rs`)

| msgId | Канал | Назва | Значення |
|---|---|---|---|
| 0x0007 | 0 | ChannelOpenRequest | Телефон відкриває канал |
| 0x000d | 0 | NavFocusRequest | Телефон просить фокус навігації. Тип 2 = PROJECTED, тип 1 = NATIVE |
| 0x0012 | 0 | AudioFocusRequest | Фокус аудіо |
| 0x8001 | 12 | START | Старт навігації (у трасах не було) |
| 0x8002 | 12 | STOP | Зупинка (у трасах не було) |
| 0x8003 | 12 | STATUS | ACTIVE або INACTIVE |
| 0x8004 | 12 | NEXT_TURN | Не було в трасах, LIVI обробляє |
| 0x8005 | 12 | NEXT_TURN_DISTANCE | Не було в трасах, LIVI обробляє |
| 0x8006 | 12 | STATE | Маневр, дорога, адреса призначення |
| 0x8007 | 12 | CURRENT_POSITION | Відстані, ETA, час до прибуття |
| 0x8001 | 10 | PairingRequest | Не плутати з START навігації. ID однакові на різних каналах |
| 0x8001 | 13 | Status медіа | Не плутати з START навігації |
| 0x8003 | 13 | Metadata медіа | Не плутати з STATUS навігації |

Важливо: однакові msgId на різних каналах означають різне. Завжди вказувати пару канал + msgId.
