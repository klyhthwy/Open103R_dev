DIR := $(shell pwd)

OUTPUT_FILENAME := Open103R_dev
DEVICE := STM32F103xE

GNU_INSTALL_ROOT := /usr/local/gcc-arm-none-eabi-5_4-2016q3
GNU_VERSION := 5.4.1
GNU_PREFIX := arm-none-eabi

CMSIS_DIR := $(DIR)/STM32Cube_FW_F1_V1.4.0/Drivers/CMSIS
SYSTEM_DIR := $(CMSIS_DIR)/Device/ST/STM32F1xx
RTOS_DIR := $(DIR)/STM32Cube_FW_F1_V1.4.0/Middlewares/Third_Party/FreeRTOS
OUTPUT_DIRECTORY := $(DIR)/out

# =====================================================
# =================== Linker Script ===================
# =====================================================
LINKER_SCRIPT := $(DIR)/STM32F103XC_FLASH.ld


# =====================================================
# =================== Include paths ===================
# =====================================================
TEMP_INC_PATHS := -I$(DIR)/Include \
	-I$(CMSIS_DIR)/Include \
	-I$(SYSTEM_DIR)/Include \
	-I$(RTOS_DIR)/Source/CMSIS_RTOS \
	-I$(RTOS_DIR)/Source/include \
	-I$(RTOS_DIR)/Source/portable/GCC/ARM_CM3 \




# =====================================================
# =================== Source files ====================
# =====================================================
TEMP_SRC_FILES := $(SYSTEM_DIR)/Source/Templates/system_stm32f1xx.c \
	$(DIR)/Source/main.c \
	$(RTOS_DIR)/Source/portable/GCC/ARM_CM3/port.c \
	$(RTOS_DIR)/Source/portable/MemMang/heap_1.c \
	$(RTOS_DIR)/Source/CMSIS_RTOS/cmsis_os.c \
	$(RTOS_DIR)/Source/tasks.c \
	$(RTOS_DIR)/Source/queue.c \
	$(RTOS_DIR)/Source/list.c \




# =====================================================
# =================== Assembly files ==================
# =====================================================
TEMP_ASM_FILES := $(SYSTEM_DIR)/Source/Templates/gcc/startup_stm32f103xe.s \



#Setting up Toolchain Commands
# Toolchain commands
CC              := $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc
AS              := $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as
AR              := $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar
LD              := $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld
NM              := $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm
OBJDUMP         := $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump
OBJCOPY         := $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy
SIZE            := $(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size
MK 				:= mkdir
RM 				:= rm -rf
CT				:= cat

COMPILER_OVERRIDES := CC="$(CC)" AS="$(AS)" AR="$(AR)" LD="$(LD)" NM="$(NM)" \
							OBJDUMP="$(OBJDUMP)" OBJCOPY="$(OBJCOPY)" SIZE="$(SIZE)"

ARCHFLAG := -mthumb -mcpu=cortex-m3 -mfloat-abi=soft

# Linker flags
LDFLAGS := $(ARCHFLAG)
LDFLAGS += -T$(LINKER_SCRIPT)
LDFLAGS += -Xlinker -Map=$(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).map
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
LDFLAGS += --specs=nano.specs

# Compiler flags
CFLAGS := $(ARCHFLAG) -D$(DEVICE) --std=gnu99 -Wall -Werror
ASMFLAGS := $(ARCHFLAG) -x assembler-with-cpp

#INFO files
SIZE_FILE := Final_Size.txt
NM_FILE := Object_Info.txt

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OUTPUT_DIRECTORY))
C_SOURCE_FILES := $(sort $(TEMP_SRC_FILES))
INCLUDEPATHS := $(sort $(TEMP_INC_PATHS))
ASSEMBLER_SOURCE_FILES := $(sort $(TEMP_ASM_FILES))

# Get file paths
C_SOURCE_PATHS = $(sort $(dir $(C_SOURCE_FILES)))
ASSEMBLER_SOURCE_PATHS = $(sort $(dir $(ASSEMBLER_SOURCE_FILES)))

#Determine the Objects we Need to Build.
C_SOURCE_FILENAMES = $(notdir $(C_SOURCE_FILES) )
ASSEMBLER_SOURCE_FILENAMES = $(notdir $(ASSEMBLER_SOURCE_FILES) )
C_OBJECTS = $(addprefix $(OUTPUT_DIRECTORY)/, $(C_SOURCE_FILENAMES:.c=.o) )
ASSEMBLER_OBJECTS = $(addprefix $(OUTPUT_DIRECTORY)/, $(ASSEMBLER_SOURCE_FILENAMES:.s=.o) )

# Set source lookup paths
vpath %.c $(C_SOURCE_PATHS)
vpath %.s $(ASSEMBLER_SOURCE_PATHS)

# Include automatically previously generated dependencies
-include $(addprefix $(OUTPUT_DIRECTORY)/, $(COBJS:.o=.d))


# Stuff for programming.
OPENOCD_DIR := "/Applications/GNU ARM Eclipse/OpenOCD/0.9.0-201505191004"
OPENOCD := $(OPENOCD_DIR)/bin/openocd
STLINK_CFG := $(OPENOCD_DIR)/scripts/interface/stlink-v2.cfg
STM_CFG := $(OPENOCD_DIR)/scripts/target/stm32f1x.cfg


#Targets
debug: clean
debug: CFLAGS += -g3 -O0
debug: ASMFLAGS += -g3 -O0
debug: $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).bin $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).hex

release: clean
release: CFLAGS += -O3
release: ASMFLAGS += -O3
release: $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).bin $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).hex


.PHONY: clean flash

echostuff:
	@echo C_OBJECTS: [$(C_OBJECTS)]
	@echo C_SOURCE_FILES: [$(C_SOURCE_FILES)]
	@echo C_SOURCE_PATHS: [$(C_SOURCE_PATHS)]

## Create build directories
$(BUILD_DIRECTORIES):
	$(MK) $@

## Create objects from C source files
$(OUTPUT_DIRECTORY)/%.o: %.c
 # Build header dependencies
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -M $< -MF "$(@:.o=.d)" -MT $@
 # Do the actual compilation
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -c -o $@ $<

## Assemble .s files
$(OUTPUT_DIRECTORY)/%.o: %.s
	$(CC) $(ASMFLAGS) $(INCLUDEPATHS) -c -o $@ $<

## Link C and assembler objects to an .out file
$(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(C_OBJECTS) $(ASSEMBLER_OBJECTS)
	$(CC) $(LDFLAGS) $(C_OBJECTS) $(ASSEMBLER_OBJECTS) -o $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NM) -S -t d $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).out > $(NM_FILE)
	$(SIZE) $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).out > $(SIZE_FILE)

## Create binary .bin file from the .out file
$(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(OBJCOPY) -O binary $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(OBJCOPY) -O ihex $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).hex
	$(SIZE) $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).out

clean:
	$(RM) $(OUTPUT_DIRECTORY) $(NM_FILE) $(SIZE_FILE)

flash: $(OUTPUT_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(OPENOCD) -f $(STLINK_CFG) -f $(STM_CFG) -c "program $< verify reset exit"
