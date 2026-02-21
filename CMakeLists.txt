cmake_minimum_required(VERSION 3.10)

# Имя вашего проекта
project(PosnetNative)

# 1. ВКЛЮЧАЕМ СТАТИЧЕСКУЮ ЛИНКОВКУ (библиотеки MSVC будут внутри DLL)
# Это заменяет динамическую загрузку vcruntime140.dll и msvcp140.dll
if(MSVC)
    # Для новых версий CMake (3.15+)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    # Для старых версий CMake (на всякий случай)
    set(variables 
        CMAKE_CXX_FLAGS_RELEASE 
        CMAKE_CXX_FLAGS_DEBUG 
    )
    foreach(variable ${variables})
        if(${variable} MATCHES "/MD")
            string(REPLACE "/MD" "/MT" ${variable} "${${variable}}")
        endif()
    endforeach()
endif()

# 2. Указываем стандарт C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Список всех исходных файлов
set(SOURCE_FILES 
    src/AddInNative.cpp 
    src/PosnetPrinter.cpp 
    src/posnet_protocol.cpp 
    src/posnet_crc.cpp
)

# 4. Путь к заголовочным файлам (где лежат ComponentBase.h и ваши .h файлы)
include_directories(include)
include_directories(src)

# 5. Создаем DLL
add_library(PosnetNative SHARED ${SOURCE_FILES})

# 6. Подключаем системные библиотеки Windows
if(WIN32)
    # Библиотека ws2_32 нужна для работы с сокетами (TCP), которые используются в posnet_protocol.cpp
    target_link_libraries(PosnetNative PRIVATE ws2_32)
    
    # Задаем имя выходного файла в зависимости от разрядности для удобства макета
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set_target_properties(PosnetNative PROPERTIES OUTPUT_NAME "Posnet_x64")
    else()
        set_target_properties(PosnetNative PROPERTIES OUTPUT_NAME "Posnet_x86")
    endif()
endif()