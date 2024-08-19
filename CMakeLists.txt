cmake_minimum_required(VERSION 3.10)

include(arm-none-eabi.cmake)
set(TARGET STM32F4XX)
# Set the project name
project(${TARGET} C)

# Specify the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add subdirectory
add_subdirectory(lib)


# Add the main executable
add_executable(${TARGET} ${SRCS})

target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/inc)
target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/core)
# Link the hardware library
target_link_libraries(${TARGET} PRIVATE hardware)

target_compile_definitions(${TARGET} PUBLIC STM32F446xx)

option(MOCK "Mock Include" ON)

if(MOCK)
  message(STATUS "TEST is ON")
else()
  message(STATUS "TEST is OFF")
endif()

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
endif()

target_compile_options(
  ${TARGET} PRIVATE 
  $<$<CONFIG:Debug>:-g>
)

target_link_options(${TARGET} PRIVATE
                    -T${LINKER_SCRIPT}
                    -Wl,--gc-sections
                    -Wl,--print-memory-usage
                    -Wl,-Map=${TARGET}.map
                    # -specs=nano.specs
                    -lstdc++
                    -lc 
                    -lm
                    )

add_custom_command(
  TARGET ${TARGET}
  POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET}.elf ${TARGET}.bin
  COMMENT "Converting ELF to BIN"
)

add_custom_target(flash
    DEPENDS ${TARGET}.bin
    COMMAND st-flash write ${TARGET}.bin 0x8000000
    COMMENT "Flashing the binary to the device"
)
                  