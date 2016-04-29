# Put your stlink folder here so make burn will work.
STLINK=~/stm32/stlink

# Put your source files here (or *.c, etc)
SRCS =src/main.c src/midi.c lib/stm32f4_discovery_audio_codec.c
SRCS +=src/organ.c src/io.c lib/system_stm32f4xx.c
SRCS +=lib/startup_stm32f4xx.c
SRCS +=lib/misc.c lib/stm32f4xx_dac.c
SRCS +=lib/stm32f4xx_dma.c lib/stm32f4xx_gpio.c
SRCS +=lib/stm32f4xx_i2c.c lib/stm32f4xx_rcc.c
SRCS +=lib/stm32f4xx_spi.c lib/stm32f4xx_usart.c
# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME=Syna

# Put your STM32F4 library code directory here
STM_COMMON=../stm32_discovery_arm_gcc/STM32F4-Discovery_FW_V1.1.0

# Normally you shouldn't need to change anything below this line!
#######################################################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS  = -g -O2 -Wall -Tstm32_flash.ld 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -I. -I./lib -I./src -DUSE_STDPERIPH_DRIVER

# Include files from STM libraries
#CFLAGS += -I$(STM_COMMON)/Utilities/STM32F4-Discovery
#CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include -I$(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Include
#CFLAGS += -I$(STM_COMMON)/Libraries/STM32F4xx_StdPeriph_Driver/inc

# add startup file to build
#SRCS += $(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f4xx.s 
OBJS = $(SRCS:.c=.o)


.PHONY: proj

all: proj

proj: $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -specs=nosys.specs -o $@ 
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	rm -f *.o $(PROJ_NAME).elf $(PROJ_NAME).hex $(PROJ_NAME).bin

# Flash the STM32F4
burn: proj
	$(STLINK)/st-flash write $(PROJ_NAME).bin 0x8000000
