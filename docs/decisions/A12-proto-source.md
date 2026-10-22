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

## A3: спайк збірки aasdk (2026-10-22)

Рішення власника D-B1 – D-B3. Хост: macOS arm64, Apple Clang 21, CMake 4.4.4, Ninja. aasdk зібрано поза репо.

- Джерело: opencardev/aasdk, `main`, коміт `9bf6adf933665dee26532201719fac14a047ccf1` (2026-06-11). Сабмодуль `third_party/aasdk` фіксує цей коміт.
- Залежності (Homebrew): libusb 1.0.30, boost@1.85 1.85.0_3 (keg-only, формулу вимкнено 2026-04-05, bottle встановлено), protobuf 36.2, abseil 20260817.0, openssl@3 3.5.9.
- Прапорці, що спрацювали: `-G Ninja -DAASDK_TEST=OFF -DSKIP_BUILD_PROTOBUF=ON -DSKIP_BUILD_ABSL=ON -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_PREFIX_PATH="/opt/homebrew/opt/boost@1.85;/opt/homebrew"`.
- Без `CMAKE_POLICY_VERSION_MINIMUM=3.5` конфігурація падає: `cmake_modules/gitversion.cmake:2` має `cmake_minimum_required(VERSION 3.0.0)`, CMake 4 таке відхиляє.
- Генератор Unix Makefiles на цьому хості падає (`gmake: /opt/homebrew/opt/make/libexec/gnubin: Permission denied`, проблема хоста). Ninja працює.
- Результат: `ninja aasdk` зібрав 569 цілей за 55 с, код виходу 0, артефакти `lib/libaasdk.a` і `lib/libaap_protobuf.a`. На Darwin aasdk статичний, на Linux його `CMakeLists.txt` збирає SHARED. Попередження лише з заголовка libusb (`-Wzero-length-array`, `-Wc99-extensions`).
- Boost: обрано O1, версія нижче 1.87, перевірена 1.85. Зі збіркою на Boost 1.87 aasdk падає: `IOContextWrapper.hpp:31: no type named 'io_service' in namespace 'boost::asio'`. 95 файлів у `include/` і `src/` використовують `io_service`.
- Спайк не перевіряв: Debian arm64 (ROCK 5B+), вбудовування через `add_subdirectory`, витік `CMAKE_BUILD_TYPE` і `CMAKE_INSTALL_PREFIX` у батьківський проєкт, лінкування в Qt-ціль. Перші три перевіряє AA-Z1-T2/T4.

Вбудовування: `option(AA_WITH_AASDK ... OFF)` у кореневому `CMakeLists.txt`, `add_subdirectory(third_party/aasdk EXCLUDE_FROM_ALL)`, без кроку install. Інструкції збірки: `README.md`, розділ Build.

- Перевірка вбудовування (AA-Z1-T2, 2026-10-22, macOS arm64, `cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DAA_WITH_AASDK=ON`, Boost 1.85): конфігурація і генерація пройшли без помилок і без `CMAKE_FIND_PACKAGE_TARGETS_GLOBAL`. У `CMakeCache.txt` `CMAKE_BUILD_TYPE=Debug`, `CMAKE_INSTALL_PREFIX=/usr/local`, `Boost_DIR` у `boost@1.85`. Команда компіляції `AasdkUsbPort.cpp` має `-std=gnu++20` без `-DNDEBUG`, `-DBOOST_ALL_DYN_LINK`, `-pedantic`, `-O3`. Команда `USBWrapper.cpp` з aasdk має `-std=gnu++17 -fPIC -Wall -pedantic`. Обидві йдуть через `/usr/bin/c++`. `cmake --build build/aa-on -j` завершився з кодом 0 (750 кроків), у `AasdkUsbPort.cpp` попереджень немає. `ctest` у `build/aa-on`: 3 з 3 пройшли. Запуск `InfinitiCoreApp` без телефона вивів `aa-usb: watching for phones`. Debian arm64 не перевірено.
- Відомий дефект: `AOAPDevice` в aasdk читає звільнений `interfaceDescriptor_` у деструкторі (`src/USB/AOAPDevice.cpp:59-73` і `:46`, коміт `9bf6adf`), а Кор знищує його при відключенні і завершенні. Опис і рішення власника до Z2: розділ "Відомий дефект aasdk: AOAPDevice" у `docs/tickets/Z1-aoa-contract.md`.
