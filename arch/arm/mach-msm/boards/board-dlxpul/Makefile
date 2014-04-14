ifdef CONFIG_BT
	obj-$(CONFIG_MACH_DLXP_UL) += board-dlxp_ul-rfkill.o htc_bdaddress.o htc_4335_wl_reg.o
endif

board-dlxp_ul-all-objs += board-dlxp_ul.o board-dlxp_ul-pmic.o board-dlxp_ul-storage.o board-dlxp_ul-gpiomux.o board-dlxp_ul-display.o board-dlxp_ul-gpu.o board-dlxp_ul-keypad.o board-dlxp_ul-wifi.o  board-dlxp_ul-audio.o

ifdef CONFIG_FB_MSM
board-dlxp_ul-all-objs += board-dlxp_ul-display.o board-dlxp_ul-gpu.o
endif

ifdef CONFIG_MSM_CAMERA
board-dlxp_ul-all-objs += board-dlxp_ul-camera.o
endif

obj-$(CONFIG_MACH_DLXP_UL) += board-dlxp_ul-all.o board-dlxp_ul-regulator.o
