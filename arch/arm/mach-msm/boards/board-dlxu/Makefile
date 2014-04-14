ifdef CONFIG_BT
	obj-$(CONFIG_MACH_DELUXE_U) += board-deluxe_u-rfkill.o
endif

board-deluxe_u-all-objs += board-deluxe_u.o board-deluxe_u-pmic.o board-deluxe_u-storage.o board-deluxe_u-gpiomux.o board-deluxe_u-display.o board-deluxe_u-gpu.o board-deluxe_u-keypad.o board-deluxe_u-wifi.o board-deluxe_u-audio.o

ifdef CONFIG_MSM_CAMERA
board-deluxe_u-all-objs += board-deluxe_u-camera.o
endif

obj-$(CONFIG_MACH_DELUXE_U) += board-deluxe_u-all.o board-deluxe_u-regulator.o
