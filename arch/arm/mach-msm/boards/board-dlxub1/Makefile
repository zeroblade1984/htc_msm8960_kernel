ifdef CONFIG_BT
	obj-$(CONFIG_MACH_DELUXE_UB1) += board-deluxe_ub1-rfkill.o
endif

board-deluxe_ub1-all-objs += board-deluxe_ub1.o board-deluxe_ub1-pmic.o board-deluxe_ub1-storage.o board-deluxe_ub1-gpiomux.o board-deluxe_ub1-keypad.o board-deluxe_ub1-wifi.o board-deluxe_ub1-audio.o

ifdef CONFIG_FB_MSM
board-deluxe_ub1-all-objs += board-deluxe_ub1-display.o board-deluxe_ub1-gpu.o
endif

ifdef CONFIG_MSM_CAMERA
board-deluxe_ub1-all-objs += board-deluxe_ub1-camera.o
endif

obj-$(CONFIG_MACH_DELUXE_UB1) += board-deluxe_ub1-all.o board-deluxe_ub1-regulator.o
