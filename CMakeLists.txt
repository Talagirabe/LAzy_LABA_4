cmake_minimum_required(VERSION 3.20)
project(LAzy_LABA_4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Автоматически находим все заголовочные (.h) файлы проекта
file(GLOB_RECURSE HEADERS "src/*.h" "LABA_2/*.h")

# ==============================================================================
# ТАРГЕТ 1: Основное приложение (Твоя лабораторная работа)
# ==============================================================================
add_executable(LAzy_LABA_4 src/main.cpp ${HEADERS})


# ==============================================================================
# ТАРГЕТ 2: Ручные тесты ленивых последовательностей (БЕЗ Google Test)
# ==============================================================================
# Собираем этот таргет только в том случае, если файл test_lazy_manual.cpp существует
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/test_lazy_manual.cpp")
    add_executable(RunManualTests test/test_lazy_manual.cpp ${HEADERS})
endif()


# ==============================================================================
# ТАРГЕТ 3: Автоматические Google тесты (Кардиналы, Стримы, Окна кэша)
# ==============================================================================
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
# Отключаем установку GTest в систему, он нужен нам только внутри этого проекта
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 🌟 Автоматически находим ВСЕ .cpp файлы тестов в папке test/
file(GLOB GTEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

# Исключаем файл ручных тестов (test_lazy_manual.cpp) из автоматической сборки GTest,
# чтобы их функции main() не конфликтовали между собой
if(TARGET RunManualTests)
    list(REMOVE_ITEM GTEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/test_lazy_manual.cpp")
endif()

# Создаем исполняемый файл для Google тестов
add_executable(RunGTests
        ${GTEST_SOURCES}
        ${HEADERS}
)

# Линкуем Google Test. Библиотека gtest_main автоматически подставит точку входа main()!
target_link_libraries(RunGTests gtest gtest_main)