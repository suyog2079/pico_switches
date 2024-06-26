# initialize the S
# note: this must happen before project()
cmake_minimum_required(VERSION 3.12)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_switches C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(pico_switches
  Core/src/main.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(pico_switches 
  pico_stdlib 
  hardware_gpio
)

pico_enable_stdio_uart(pico_switches 1)
pico_enable_stdio_usb(pico_switches 1)
# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(pico_switches)

