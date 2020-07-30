################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
modbus/mb_m.obj: ../modbus/mb_m.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/mb_m.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/mbcrc.obj: ../modbus/mbcrc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/mbcrc.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/mbfunccoils_m.obj: ../modbus/mbfunccoils_m.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/mbfunccoils_m.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/mbfuncdisc_m.obj: ../modbus/mbfuncdisc_m.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/mbfuncdisc_m.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/mbfuncholding_m.obj: ../modbus/mbfuncholding_m.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/mbfuncholding_m.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/mbfuncinput_m.obj: ../modbus/mbfuncinput_m.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/mbfuncinput_m.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/mbfuncother.obj: ../modbus/mbfuncother.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/mbfuncother.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/mbrtu_m.obj: ../modbus/mbrtu_m.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/mbrtu_m.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/mbutils.obj: ../modbus/mbutils.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/mbutils.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/modbus_main.obj: ../modbus/modbus_main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/modbus_main.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/portevent_m.obj: ../modbus/portevent_m.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/portevent_m.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/portserial_m.obj: ../modbus/portserial_m.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/portserial_m.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/porttimer_m.obj: ../modbus/porttimer_m.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/porttimer_m.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

modbus/user_mb_app_m.obj: ../modbus/user_mb_app_m.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --include_path="C:/ti" --include_path="C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04" --include_path="C:/ti/tirtos_tivac_2_14_00_10/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_tivac_2_14_00_10/packages/ti/drivers/wifi/cc3100/Simplelink/Include" -g --gcc --define=ccs="ccs" --define=PART_TM4C129ENCPDT --define=NET_SL --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="modbus/user_mb_app_m.pp" --obj_directory="modbus" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


