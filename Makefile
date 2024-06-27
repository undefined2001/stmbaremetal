TARGET = final


# Configure micro-controller
MCU_FAMILY	= STM32F446xx
LDSCRIPT 	= STM32F446RETX_FLASH.ld
CPU			= cortex-m4
INSTR_SET	= thumb
FLOAT_ABI	= soft

# compiler option
OPT			:= -Os
CSTD		?= c11
CXXSTD		:= c++20

# Project specific configuration
BUILD_DIR 	:= build
BUILD_TYPE	?= Debug
SRC_DIR 	:= ./src
INC_DIRS	:= ./core ./inc


PREFIX		?= arm-none-eabi
CC			:= $(PREFIX)-gcc
CXX			:= $(PREFIX)-g++
LD			:= $(PREFIX)-gcc
AR			:= $(PREFIX)-ar
AS			:= $(PREFIX)-as
SIZE		:= $(PREFIX)-size
OBJCOPY		:= $(PREFIX)-objcopy
OBJDUMP		:= $(PREFIX)-objdump
GDB			:= $(PREFIX)-gdb

# collect source files and generate object files
SRCS 		:= $(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.c')	
OBJS 		:= $(addsuffix .o,$(basename $(SRCS)))			# replace .c with .o
OBJS 		:= $(addprefix $(BUILD_DIR)/,$(OBJS))			# replace .c with .o


# Define stm32 family macro
DEFS		+= -D$(MCU_FAMILY)

# header library include flsgs
INC_FLAGS 	= $(addprefix -I,$(INC_DIRS))


# Target-specific flags
CPU_FLAGS	?= -mfloat-abi=$(FLOAT_ABI) -m$(INSTR_SET) -mcpu=$(CPU)

CPPFLAGS	?=$(DEFS) $(INC_FLAGS)
CFLAGS		?=$(CPU_FLAGS) #$(OPT)
CXXFLAGS 	:=$(CFLAGS) -fno-exceptions -fno-rtti
LDFLAGS		?=$(CPU_FLAGS)

# Do not link stdlib with executable
CFLAGS		+= -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections #-nostdlib 
CXXFLAGS	+=   -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections #-nostdlib
LDFLAGS		+=   -fno-tree-loop-distribute-patterns #-nostdlib


# Warning options for C and CXX compiler
CFLAGS		+= -Wall -Werror#-Wextra -Wundef -Wshadow -Wredundant-decls -Wmissing-prototypes #-Wstrict-prototypes
CXXFLAGS	+= -Wall -Werror #-Wextra -Wundef -Wshadow -Wredundant-decls -Weffc++ -Werror


LDFLAGS		+= $(addprefix -T, ${LDSCRIPT}) --specs=nosys.specs -Wl,--gc-sections,--print-memory-usage 
LDFLAGS		+= -Wl,-Map=$(basename $@).map

all: bin
elf: $(BUILD_DIR)/$(TARGET).elf
bin: $(BUILD_DIR)/$(TARGET).bin
hex: $(BUILD_DIR)/$(TARGET).hex
srec: $(BUILD_DIR)/$(TARGET).srec
list: $(BUILD_DIR)/$(TARGET).list


%.bin: %.elf
	@$(OBJCOPY) -Obinary $(*).elf $(*).bin


$(BUILD_DIR)/%.o:%.c
	@mkdir -p $(dir $@)
	@echo "Compiling" $(notdir $@) "-->" $(notdir $@)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	@echo "Linking sources into " $(notdir $@)
	@$(CC) $(LDFLAGS) -o $@ $^ -lgcc


flash: bin
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000
	@st-flash reset
	
debug: CFLAGS 		+= -g -gdwarf-2
debug: CXXFLAGS 	+= -g -gdwarf-2
debug: all

clean:
	rm -rf build


test:
	@echo ${SRCS}