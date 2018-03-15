################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../synergy/ssp/src/driver/r_dtc/r_dtc.c 

OBJS += \
./synergy/ssp/src/driver/r_dtc/r_dtc.o 

C_DEPS += \
./synergy/ssp/src/driver/r_dtc/r_dtc.d 


# Each subdirectory must supply rules for building sources it contributes
synergy/ssp/src/driver/r_dtc/%.o: ../synergy/ssp/src/driver/r_dtc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	C:\Renesas\Synergy\e2studio_v5.4.0.023_ssp_v1.3.3\eclipse\../Utilities/isdebuild arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -D_RENESAS_SYNERGY_ -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy_cfg\ssp_cfg\bsp" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy_cfg\ssp_cfg\driver" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy\ssp\inc" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy\ssp\inc\bsp" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy\ssp\inc\bsp\cmsis\Include" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy\ssp\inc\driver\api" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy\ssp\inc\driver\instances" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\src" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\src\synergy_gen" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy_cfg\ssp_cfg\framework\el" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy\ssp\inc\framework\el" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy\ssp\src\framework\el\tx" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy_cfg\ssp_cfg\framework" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy\ssp\inc\framework\api" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy\ssp\inc\framework\instances" -I"D:\workdir\forth\renesas_rtos_training\workdir\lab02\synergy\ssp\src\framework\el\ux" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


