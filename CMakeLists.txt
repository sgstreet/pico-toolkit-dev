cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
#include($ENV{PICO_TOOLKIT_PATH}/external/pico_toolkit_import.cmake)

project(pico-project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(hello-world)

#add_subdirectory(lib)
#add_subdirectory(rtos-toolkit-hog-test)

# preprocessor macros
#target_compile_definitions(app PRIVATE 
#    PICO_HEAP_SIZE=2048
#    PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
#)


# linker options
#target_link_options(app 
#    PRIVATE "LINKER:--print-memory-usage"
#)

