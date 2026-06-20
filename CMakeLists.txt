cmake_minimum_required(VERSION 4.2)
project(LAzy_LABA_4)

set(CMAKE_CXX_STANDARD 20)

add_executable(LAzy_LABA_4
        src/lazy/lazy_sequence.h
        src/generators/generator.h
        src/generators/rule_generator.h
        src/generators/map_generator.h
        src/generators/filter_generator.h
        src/generators/take_generator.h
        src/generators/concat_generator.h
        src/utils/cardinal.h
        src/streams/sequence_streams.h
        src/utils/pair.h
        src/generators/zip_generator.h
        src/generators/append_generator.h
        src/generators/prepend_generator.h
        src/generators/insert_at_generator.h
        src/utils/windowed_cache.h
        src/fsm/state_machine.h
        src/generators/sub_generator.h
        test/test.cpp
       # src/main.cpp
)


# =========================
# Google Test
# =========================
