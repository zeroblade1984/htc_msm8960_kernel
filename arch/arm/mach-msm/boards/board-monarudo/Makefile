ifdef CONFIG_BT
	obj-$(CONFIG_MACH_MONARUDO) += board-monarudo-rfkill.o
endif

board-monarudo-all-objs += board-monarudo.o board-monarudo-pmic.o board-monarudo-storage.o board-monarudo-gpiomux.o board-monarudo-display.o board-monarudo-gpu.o board-monarudo-keypad.o board-monarudo-wifi.o board-monarudo-audio.o

ifdef CONFIG_MSM_CAMERA
board-monarudo-all-objs += board-monarudo-camera.o
endif

obj-$(CONFIG_MACH_MONARUDO) += board-monarudo-all.o board-monarudo-regulator.o
