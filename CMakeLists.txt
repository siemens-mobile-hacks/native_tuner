cmake_minimum_required(VERSION 3.28.2)

include(../sdk/config.cmake)
project(sieelf_native_tuner C)

link_libraries(crt crt_helper gcc c)
file(GLOB_RECURSE SOURCES "src/*.c")

add_definitions(-DSWILIB_MODERN)

add_executable(NativeTuner_ELKA ${SOURCES})
target_sdk_setup(NativeTuner_ELKA ELKA)
target_compile_options(NativeTuner_ELKA BEFORE PUBLIC -std=c11 -Wall -Werror)
set_target_properties(NativeTuner_ELKA PROPERTIES OUTPUT_NAME "NativeTuner")
