################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../task/src/init.c \
../task/src/mytask.c 

OBJS += \
./task/src/init.o \
./task/src/mytask.o 

C_DEPS += \
./task/src/init.d \
./task/src/mytask.d 


# Each subdirectory must supply rules for building sources it contributes
task/src/init.o: ../task/src/init.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb '-DEFM32GG280F512=1' '-DDEBUG=1' -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/CMSIS/Include" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/bsp" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/emlib/inc" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/drivers" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/Device/SiliconLabs/EFM32GG/Include" -I"D:\WS\EFM\EFM32GG280\FREERTOS\include" -I"D:\WS\EFM\EFM32GG280\hal\inc" -I"D:\WS\EFM\EFM32GG280\mid\inc" -I"D:\WS\EFM\EFM32GG280\rf\inc" -I"D:\WS\EFM\EFM32GG280\src" -I"D:\WS\EFM\EFM32GG280\task\inc" -I"D:\WS\EFM\EFM32GG280\protocol\icp\inc" -I"D:\WS\EFM\EFM32GG280\protocol\ecp\inc" -I"D:\WS\EFM\EFM32GG280\protocol\esam\inc" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"task/src/init.d" -MT"task/src/init.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

task/src/mytask.o: ../task/src/mytask.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb '-DEFM32GG280F512=1' '-DDEBUG=1' -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/CMSIS/Include" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/bsp" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/emlib/inc" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/drivers" -I"C:/Memo/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/Device/SiliconLabs/EFM32GG/Include" -I"D:\WS\EFM\EFM32GG280\FREERTOS\include" -I"D:\WS\EFM\EFM32GG280\hal\inc" -I"D:\WS\EFM\EFM32GG280\mid\inc" -I"D:\WS\EFM\EFM32GG280\rf\inc" -I"D:\WS\EFM\EFM32GG280\src" -I"D:\WS\EFM\EFM32GG280\task\inc" -I"D:\WS\EFM\EFM32GG280\protocol\icp\inc" -I"D:\WS\EFM\EFM32GG280\protocol\ecp\inc" -I"D:\WS\EFM\EFM32GG280\protocol\esam\inc" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"task/src/mytask.d" -MT"task/src/mytask.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


