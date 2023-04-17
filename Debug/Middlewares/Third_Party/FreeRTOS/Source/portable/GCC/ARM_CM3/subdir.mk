################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c 

OBJS += \
./Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.o 

C_DEPS += \
./Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/port.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/%.o: ../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32L1 -DSTM32L152RETx -DNUCLEO_L152RE -DDEBUG -DSTM32L152xE -DUSE_HAL_DRIVER -DUSE_RTOS_SYSTICK -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/Utilities/STM32L1xx_Nucleo" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/Drivers/includes" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/HAL_Driver/Inc/Legacy" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/Middlewares/Third_Party/FreeRTOS/Source/include" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/inc" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/CMSIS/device" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/CMSIS/core" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/HAL_Driver/Inc" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


