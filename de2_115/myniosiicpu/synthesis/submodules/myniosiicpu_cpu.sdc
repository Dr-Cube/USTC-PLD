# Legal Notice: (C)2017 Altera Corporation. All rights reserved.  Your
# use of Altera Corporation's design tools, logic functions and other
# software and tools, and its AMPP partner logic functions, and any
# output files any of the foregoing (including device programming or
# simulation files), and any associated documentation or information are
# expressly subject to the terms and conditions of the Altera Program
# License Subscription Agreement or other applicable license agreement,
# including, without limitation, that your use is for the sole purpose
# of programming logic devices manufactured by Altera and sold by Altera
# or its authorized distributors.  Please refer to the applicable
# agreement for further details.

#**************************************************************
# Timequest JTAG clock definition
#   Uncommenting the following lines will define the JTAG
#   clock in TimeQuest Timing Analyzer
#**************************************************************

#create_clock -period 10MHz {altera_reserved_tck}
#set_clock_groups -asynchronous -group {altera_reserved_tck}

#**************************************************************
# Set TCL Path Variables 
#**************************************************************

set 	myniosiicpu_cpu 	myniosiicpu_cpu:*
set 	myniosiicpu_cpu_oci 	myniosiicpu_cpu_nios2_oci:the_myniosiicpu_cpu_nios2_oci
set 	myniosiicpu_cpu_oci_break 	myniosiicpu_cpu_nios2_oci_break:the_myniosiicpu_cpu_nios2_oci_break
set 	myniosiicpu_cpu_ocimem 	myniosiicpu_cpu_nios2_ocimem:the_myniosiicpu_cpu_nios2_ocimem
set 	myniosiicpu_cpu_oci_debug 	myniosiicpu_cpu_nios2_oci_debug:the_myniosiicpu_cpu_nios2_oci_debug
set 	myniosiicpu_cpu_wrapper 	myniosiicpu_cpu_jtag_debug_module_wrapper:the_myniosiicpu_cpu_jtag_debug_module_wrapper
set 	myniosiicpu_cpu_jtag_tck 	myniosiicpu_cpu_jtag_debug_module_tck:the_myniosiicpu_cpu_jtag_debug_module_tck
set 	myniosiicpu_cpu_jtag_sysclk 	myniosiicpu_cpu_jtag_debug_module_sysclk:the_myniosiicpu_cpu_jtag_debug_module_sysclk
set 	myniosiicpu_cpu_oci_path 	 [format "%s|%s" $myniosiicpu_cpu $myniosiicpu_cpu_oci]
set 	myniosiicpu_cpu_oci_break_path 	 [format "%s|%s" $myniosiicpu_cpu_oci_path $myniosiicpu_cpu_oci_break]
set 	myniosiicpu_cpu_ocimem_path 	 [format "%s|%s" $myniosiicpu_cpu_oci_path $myniosiicpu_cpu_ocimem]
set 	myniosiicpu_cpu_oci_debug_path 	 [format "%s|%s" $myniosiicpu_cpu_oci_path $myniosiicpu_cpu_oci_debug]
set 	myniosiicpu_cpu_jtag_tck_path 	 [format "%s|%s|%s" $myniosiicpu_cpu_oci_path $myniosiicpu_cpu_wrapper $myniosiicpu_cpu_jtag_tck]
set 	myniosiicpu_cpu_jtag_sysclk_path 	 [format "%s|%s|%s" $myniosiicpu_cpu_oci_path $myniosiicpu_cpu_wrapper $myniosiicpu_cpu_jtag_sysclk]
set 	myniosiicpu_cpu_jtag_sr 	 [format "%s|*sr" $myniosiicpu_cpu_jtag_tck_path]

#**************************************************************
# Set False Paths
#**************************************************************

set_false_path -from [get_keepers *$myniosiicpu_cpu_oci_break_path|break_readreg*] -to [get_keepers *$myniosiicpu_cpu_jtag_sr*]
set_false_path -from [get_keepers *$myniosiicpu_cpu_oci_debug_path|*resetlatch]     -to [get_keepers *$myniosiicpu_cpu_jtag_sr[33]]
set_false_path -from [get_keepers *$myniosiicpu_cpu_oci_debug_path|monitor_ready]  -to [get_keepers *$myniosiicpu_cpu_jtag_sr[0]]
set_false_path -from [get_keepers *$myniosiicpu_cpu_oci_debug_path|monitor_error]  -to [get_keepers *$myniosiicpu_cpu_jtag_sr[34]]
set_false_path -from [get_keepers *$myniosiicpu_cpu_ocimem_path|*MonDReg*] -to [get_keepers *$myniosiicpu_cpu_jtag_sr*]
set_false_path -from *$myniosiicpu_cpu_jtag_sr*    -to *$myniosiicpu_cpu_jtag_sysclk_path|*jdo*
set_false_path -from sld_hub:*|irf_reg* -to *$myniosiicpu_cpu_jtag_sysclk_path|ir*
set_false_path -from sld_hub:*|sld_shadow_jsm:shadow_jsm|state[1] -to *$myniosiicpu_cpu_oci_debug_path|monitor_go
