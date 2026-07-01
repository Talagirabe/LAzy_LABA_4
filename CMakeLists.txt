cmake_minimum_required(VERSION 3.20)
project(LAzy_LABA_4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Автоматически находим ВСЕ заголовочные файлы в проекте
# Это гарантирует, что CLion их увидит, и ничего не потеряется
file(GLOB_RECURSE HEADERS "src/*.h" "LABA_2/*.h")

# ==============================================================================
# ТАРГЕТ 1: Твоя основная программа
# ==============================================================================
add_executable(LAzy_LABA_4 src/main.cpp ${HEADERS})


# ==============================================================================
# ТАРГЕТ 2: Ручные тесты ленивых последовательностей (БЕЗ Google Test)
# ==============================================================================
# Умная проверка: собираем этот таргет, ТОЛЬКО если ты реально создал этот файл.
# Если файла пока нет — CMake просто проигнорирует этот блок и не упадет с ошибкой!
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/test_lazy_manual.cpp")
    add_executable(RunManualTests test/test_lazy_manual.cpp ${HEADERS})
endif()


# ==============================================================================
# ТАРГЕТ 3: Google Test (для кардиналов, потоков, fsm и т.д.)
# ==============================================================================
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 🌟 МАГИЯ: Автоматически забираем все твои файлы тестов из папки test/
# Ищет файлы, заканчивающиеся на _test.cpp или _tests.cpp
file(GLOB GTEST_SOURCES "test/*_test.cpp" "test/*_tests.cpp")

# Создаем исполняемый файл для GTest, передавая туда найденные файлы
add_executable(RunGTests
        ${GTEST_SOURCES}
        ${HEADERS}
)

# Линкуем GTest.
# gtest_main автоматически добавит функцию int main() для запуска тестов!
target_link_libraries(RunGTests gtest gtest_main)