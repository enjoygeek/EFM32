################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../protocol/ecp/src/ecp_app.c \
../protocol/ecp/src/ecp_parse.c \
../protocol/ecp/src/ecp_process.c 

OBJS += \
./protocol/ecp/src/ecp_app.o \
./protocol/ecp/src/ecp_parse.o \
./protocol/ecp/src/ecp_process.o 

C_DEPS += \
./protocol/ecp/src/ecp_app.d \
./protocol/ecp/src/ecp_parse.d \
./protocol/ecp/src/ecp_process.d 


# Each subdirectory must supply rules for building sources it contributes
protocol/ecp/src/ecp_app.o: ../protocol/ecp/src/ecp_app.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb '-DEFM32GG280F512=1' '-DDEBUG=1' -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/CMSIS/Include" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/bsp" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/emlib/inc" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/drivers" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/Device/SiliconLabs/EFM32GG/Include" -I"D:\WS\EFM\EFM32GG280\FREERTOS\include" -I"D:\WS\EFM\EFM32GG280\hal\inc" -I"D:\WS\EFM\EFM32GG280\mid\inc" -I"D:\WS\EFM\EFM32GG280\rf\inc" -I"D:\WS\EFM\EFM32GG280\src" -I"D:\WS\EFM\EFM32GG280\task\inc" -I"D:\WS\EFM\EFM32GG280\protocol\icp\inc" -I"D:\WS\EFM\EFM32GG280\protocol\ecp\inc" -I"D:\WS\EFM\EFM32GG280\protocol\esam\inc" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"protocol/ecp/src/ecp_app.d" -MT"protocol/ecp/src/ecp_app.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

protocol/ecp/src/ecp_parse.o: ../protocol/ecp/src/ecp_parse.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb '-DEFM32GG280F512=1' '-DDEBUG=1' -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/CMSIS/Include" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/bsp" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/emlib/inc" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/drivers" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/Device/SiliconLabs/EFM32GG/Include" -I"D:\WS\EFM\EFM32GG280\FREERTOS\include" -I"D:\WS\EFM\EFM32GG280\hal\inc" -I"D:\WS\EFM\EFM32GG280\mid\inc" -I"D:\WS\EFM\EFM32GG280\rf\inc" -I"D:\WS\EFM\EFM32GG280\src" -I"D:\WS\EFM\EFM32GG280\task\inc" -I"D:\WS\EFM\EFM32GG280\protocol\icp\inc" -I"D:\WS\EFM\EFM32GG280\protocol\ecp\inc" -I"D:\WS\EFM\EFM32GG280\protocol\esam\inc" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"protocol/ecp/src/ecp_parse.d" -MT"protocol/ecp/src/ecp_parse.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

protocol/ecp/src/ecp_process.o: ../protocol/ecp/src/ecp_process.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb '-DEFM32GG280F512=1' '-DDEBUG=1' -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/CMSIS/Include" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/bsp" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/emlib/inc" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/drivers" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/Device/SiliconLabs/EFM32GG/Include" -I"D:\WS\EFM\EFM32GG280\FREERTOS\include" -I"D:\WS\EFM\EFM32GG280\hal\inc" -I"D:\WS\EFM\EFM32GG280\mid\inc" -I"D:\WS\EFM\EFM32GG280\rf\inc" -I"D:\WS\EFM\EFM32GG280\src" -I"D:\WS\EFM\EFM32GG280\task\inc" -I"D:\WS\EFM\EFM32GG280\protocol\icp\inc" -I"D:\WS\EFM\EFM32GG280\protocol\ecp\inc" -I"D:\WS\EFM\EFM32GG280\protocol\esam\inc" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"protocol/ecp/src/ecp_process.d" -MT"protocol/ecp/src/ecp_process.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


