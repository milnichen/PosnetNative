@echo off
setlocal

:: Путь к папке сборки
set BUILD_DIR_X86=build_x86
set BUILD_DIR_X64=build_x64

:: Проверка наличия cmake в PATH
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo ОШИБКА: Команда cmake не найдена.
    echo Пожалуйста, запустите этот скрипт через "Developer Command Prompt для Visual Studio".
    pause
    exit /b 1
)

:: Очистка старых кешей, если проект был перемещен
if exist %BUILD_DIR_X86% rd /s /q %BUILD_DIR_X86%
if exist %BUILD_DIR_X64% rd /s /q %BUILD_DIR_X64%

echo === Сборка для x86 (32-bit) ===
cmake -B %BUILD_DIR_X86% -A Win32 || exit /b 1
cmake --build %BUILD_DIR_X86% --config Release || exit /b 1

echo.
echo === Сборка для x64 (64-bit) ===
cmake -B %BUILD_DIR_X64% -A x64 || exit /b 1
cmake --build %BUILD_DIR_X64% --config Release || exit /b 1

echo.
echo === Готово! ===
echo DLL x86: %BUILD_DIR_X86%\Release\Posnet_x86.dll
echo DLL x64: %BUILD_DIR_X64%\Release\Posnet_x64.dll
echo.
echo Файлы для макета 1С:
echo 1. %~dp0%BUILD_DIR_X86%\Release\Posnet_x86.dll
echo 2. %~dp0%BUILD_DIR_X64%\Release\Posnet_x64.dll
echo 3. %~dp0manifest.xml
echo.
echo Упакуйте эти три файла в ZIP-архив.

pause