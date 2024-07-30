set(SRCS 
    ./hardware/src/dma.c
    ./hardware/src/clock.c
    ./hardware/src/timer.c
    ./hardware/src/uart.c
    ./hardware/src/adc.c
    ./hardware/src/pwm.c
    ./hardware/src/spi.c
    ./hardware/src/i2c.c
)

# Create a static library from the collected source files
add_library(hardware STATIC ${SRCS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/hardware)
# Include directories for this target
target_include_directories(hardware PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc  ${CMAKE_SOURCE_DIR}/core)
