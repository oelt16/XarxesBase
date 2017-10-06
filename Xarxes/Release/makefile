################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/msp430_4.2.1

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./sensor.obj" \
"./halwaitsensor.obj" \
"./hal_wait.obj" \
"./hal_spi.obj" \
"./hal_rf_wait_for_crystal_oscillator.obj" \
"./hal_rf_set_channel.obj" \
"./hal_TempCalculation.obj" \
"../lnk_msp430f1611.cmd" \
-l"libc.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: tmote2.out

# Tool invocations
tmote2.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP430 Linker'
	"C:/ti/ccsv5/tools/compiler/msp430_4.2.1/bin/cl430" -vmsp -g -O0 --define=__MSP430F1611__ --diag_warning=225 --printf_support=minimal -z -m"tmote2.map" --stack_size=80 --heap_size=80 --use_hw_mpy=16 --warn_sections -i"C:/ti/ccsv5/ccs_base/msp430/include" -i"C:/ti/ccsv5/tools/compiler/msp430_4.2.1/lib" -i"C:/ti/ccsv5/tools/compiler/msp430_4.2.1/include" --reread_libs --xml_link_info="tmote2_linkInfo.xml" --rom_model -o "tmote2.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(MSP430_EXECUTABLE_OUTPUTS__QUOTED) "tmote2.out"
	-$(RM) "hal_TempCalculation.pp" "hal_rf_set_channel.pp" "hal_rf_wait_for_crystal_oscillator.pp" "hal_spi.pp" "hal_wait.pp" "halwaitsensor.pp" "sensor.pp" 
	-$(RM) "hal_TempCalculation.obj" "hal_rf_set_channel.obj" "hal_rf_wait_for_crystal_oscillator.obj" "hal_spi.obj" "hal_wait.obj" "halwaitsensor.obj" "sensor.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

