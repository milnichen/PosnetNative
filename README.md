# Posnet Native API — внешняя компонента 1С на C++

Компонента для обмена с фискальным принтером Posnet по протоколу TCP (кадр STX + тело + ETX + CRC16). Реализована на **Native API** 1С:Предприятие 8 (вместо прежней COM/C#).

## Важно для подключения в 1С

- Реализован **полный интерфейс IComponentBase** (Init, setMemManager, GetInfo, Done, RegisterExtensionAs, GetNMethods, FindMethod, GetMethodName, GetNParams, GetParamDefValue, HasRetVal, CallAsFunc и др.) по документации 1С.
- Возвращаемые строки выделяются **только через менеджер памяти** (setMemManager / AllocMemory), иначе платформа не освобождает память и возможны сбои.
- Экспорты DLL по умолчанию с конвенцией **__stdcall** (32-bit); в `.def` для x86 указаны декорированные имена (_GetClassNames@0 и т.д.). Если при загрузке из макета платформа падает без сообщения, соберите x86 с **cdecl** (см. ниже).
- `SetPlatformCapabilities` возвращает **eAppCapabilities1** (1), иначе компонента может не инициализироваться.

## Интерфейс для 1С

- **Имя компоненты:** `Posnet`
- **Класс:** `PosnetPrinter`
- **Метод:** `SendCommand(IP, Порт, Команда)` — возвращает строку: `"OK"`, `"ERR_*"` или тело ответа принтера.

В коде 1С использование не меняется:

```bsl
ПодключитьВнешнююКомпоненту("ОбщийМакет.DriverPosnetFiscalPrinter", "Posnet", ТипВнешнейКомпоненты.Native);
Принтер = Новый("AddIn.Posnet.PosnetPrinter");
Ответ = Принтер.SendCommand("192.168.30.80", 6666, "trstat");
```

## Сборка

### Требования

- Windows (x86 и/или x64)
- CMake 3.10+
- Компилятор: Visual Studio 2017+ (или MSVC из Build Tools), либо MinGW с поддержкой C++11
- Заголовок `ComponentBase.h` в каталоге `include/` — минимальный вариант включён в репозиторий. Для полной совместимости с платформой 1С можно подставить официальные заголовки из поставки 1С (каталог «Компоненты» / Native API).

### Сборка x86 (Posnet_x86.dll)

```bat
cmake -B build_x86 -A Win32
cmake --build build_x86 --config Release
```

Результат: `build_x86\Release\Posnet_x86.dll`

### Сборка x64 (Posnet_x64.dll)

```bat
cmake -B build_x64 -A x64
cmake --build build_x64 --config Release
```

Результат: `build_x64\Release\Posnet_x64.dll`

### Сборка x86 с cdecl (если при загрузке из макета платформа падает)

Если при подключении компоненты из общего макета 1С «висит» и затем падает без подробного сообщения, возможна несовместимость конвенции вызова (платформа ожидает cdecl на x86). Соберите 32-битную DLL с опцией **POSNET_CDECL**:

```bat
cmake -DPOSNET_CDECL=ON -B build_x86_cdecl -A Win32
cmake --build build_x86_cdecl --config Release
```

Подставьте полученную `Posnet_x86.dll` в архив макета и проверьте подключение снова.

### Подключение в 1С

1. Упакуйте в один архив (ZIP):
   - `manifest.xml` (из корня PosnetNative, с `type="native"` и путями к `Posnet_x86.dll`, `Posnet_x64.dll`)
   - `Posnet_x86.dll`
   - `Posnet_x64.dll`
2. Загрузите этот архив в общий макет конфигурации (например, `DriverPosnetFiscalPrinter`, тип «Двоичные данные»).
3. В коде вызывайте `ПодключитьВнешнююКомпоненту("ОбщийМакет.DriverPosnetFiscalPrinter", "Posnet", ТипВнешнейКомпоненты.Native)`.

Файл `manifest.xml` в репозитории уже настроен на `type="native"` и имена `Posnet_x86.dll` / `Posnet_x64.dll`.

## Структура проекта

- `include/ComponentBase.h` — минимальный заголовок Native API (при необходимости замените на официальный из 1С).
- `src/posnet_crc.cpp`, `posnet_crc.h` — CRC-16 для протокола Posnet.
- `src/posnet_protocol.cpp`, `posnet_protocol.h` — построение кадра, разбор ответа, TCP-обмен.
- `src/PosnetPrinter.cpp`, `PosnetPrinter.h` — класс компоненты, метод `SendCommand`, конвертация CP1250 ↔ Unicode.
- `src/AddInNative.cpp` — экспорты GetClassNames, GetClassObject, DestroyObject, SetPlatformCapabilities.
- `PosnetNative.def` / `Posnet_x64.def` — экспорт имён для DLL.

## Протокол Posnet

- Кадр: `STX (0x02)` + тело команды + `ETX (0x03)` + CRC16 в HEX (4 символа). Кодировка тела: CP1250.
- Ответ: один байт `ACK (0x06)` → строка `"OK"`; `NAK (0x15)` → `"ERR_CRC"`; либо кадр `STX…ETX+CRC` → строка тела ответа.

Логика портирована из прежней C#-компоненты (Posnet1CComponent).

## VC++ Runtime

При сборке с **динамической** линковкой CRT (по умолчанию в CMake) на компьютере, где запускается 1С, должен быть установлен **Microsoft Visual C++ Redistributable** той же версии и разрядности, что и компилятор:

- **Visual Studio 2017 / 2019 / 2022** (и Build Tools) → распространяемый пакет **VC++ 2015–2022** (одна поставка для 2015/2017/2019/2022):
  - [Скачать x86](https://aka.ms/vs/17/release/vc_redist.x86.exe)
  - [Скачать x64](https://aka.ms/vs/17/release/vc_redist.x64.exe)

Для **Posnet_x86.dll** нужен x86 Redistributable, для **Posnet_x64.dll** — x64 (на 64-разрядной ОС обычно ставят оба).

Чтобы **не требовать** установки Redistributable, пересоберите с статической линковкой CRT: в `CMakeLists.txt` раскомментируйте строку с `CMAKE_MSVC_RUNTIME_LIBRARY` (линковка `/MT`). Тогда CRT будет включён в DLL и отдельный VC++ Runtime на целевом ПК не нужен.
