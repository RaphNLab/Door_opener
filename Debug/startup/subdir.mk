################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32l152xe.s 

OBJS += \
./startup/startup_stm32l152xe.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/Utilities/STM32L1xx_Nucleo" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/HAL_Driver/Inc/Legacy" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/Middlewares/Third_Party/FreeRTOS/Source/include" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/inc" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/CMSIS/device" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/CMSIS/core" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"/home/silvere/Work/Tuto/RTOS/RTOS_Workspace/DoorOpener/HAL_Driver/Inc" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


