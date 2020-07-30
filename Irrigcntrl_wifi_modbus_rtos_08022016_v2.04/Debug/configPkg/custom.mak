## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em4f linker.cmd package/cfg/Irrigation_Controller_v1.0_pem4f.oem4f

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/Irrigation_Controller_v1.0_pem4f.xdl
	$(SED) 's"^\"\(package/cfg/Irrigation_Controller_v1.0_pem4fcfg.cmd\)\"$""\"C:/Users/Mac-Karan/IRC_221115/Irrigcntrl_wifi_modbus_rtos_08022016_v2.04/Debug/configPkg/\1\""' package/cfg/Irrigation_Controller_v1.0_pem4f.xdl > $@
	-$(SETDATE) -r:max package/cfg/Irrigation_Controller_v1.0_pem4f.h compiler.opt compiler.opt.defs
