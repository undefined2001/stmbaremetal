set(COMPILER_PREFIX arm-none-eabi)

set(SRCS 
    src/startup_stm32f446retx.c
    src/system_stm32f4xx.c
    src/main.c
    src/syscalls.c
    src/debug.c
    src/dht.c
)


set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F446RETX_FLASH.ld)


set(CMAKE_C_COMPILER ${COMPILER_PREFIX}-gcc)
set(CMAKE_ASM_COMPILER ${COMPILER_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${COMPILER_PREFIX}-g++)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

set(CMAKE_C_FLAGS "-mcpu=cortex-m4 -mfloat-abi=hard -mthumb")


set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)