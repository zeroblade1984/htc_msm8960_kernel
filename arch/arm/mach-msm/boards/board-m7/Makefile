ifdef CONFIG_BT
	obj-$(CONFIG_MACH_M7_UL) += board-m7-rfkill.o htc_4335_wl_reg.o
	obj-$(CONFIG_MACH_M7_WLS) += board-m7wl-rfkill.o htc_4335_wl_reg.o
	obj-$(CONFIG_MACH_M7_WLV) += board-m7wl-rfkill.o htc_4335_wl_reg.o
	obj-$(CONFIG_MACH_M7_WLJ) += board-m7wl-rfkill.o htc_4335_wl_reg.o
endif

board-m7-all-objs += board-m7.o board-m7-pmic.o board-m7-storage.o board-m7-gpiomux.o board-m7-keypad.o board-m7-wifi.o board-m7-audio.o
board-m7wls-all-objs += board-m7wls.o board-m7wls-pmic.o board-m7wl-storage.o board-m7wl-gpiomux.o board-m7wl-keypad.o board-m7wl-wifi.o board-m7wl-audio.o
board-m7wlv-all-objs += board-m7wlv.o board-m7wlv-pmic.o board-m7wl-storage.o board-m7wl-gpiomux.o board-m7wl-keypad.o board-m7wl-wifi.o board-m7wl-audio.o
board-m7wlj-all-objs += board-m7wlj.o board-m7wlj-pmic.o board-m7wlj-storage.o board-m7wlj-gpiomux.o board-m7wlj-keypad.o board-m7wl-wifi.o board-m7wlj-audio.o
board-m7dcg-all-objs += board-m7dcg.o board-m7dcg-pmic.o board-m7china-storage.o board-m7china-gpiomux.o board-m7dcg-keypad.o board-m7china-wifi.o board-m7china-audio.o board-m7evm-pmic.o board-m7evm.o
board-m7dug-all-objs += board-m7dug.o board-m7dug-pmic.o board-m7china-storage.o board-m7china-gpiomux.o board-m7dug-keypad.o board-m7china-wifi.o board-m7china-audio.o board-m7evm-pmic.o board-m7evm.o
board-m7dtu-all-objs += board-m7dtu.o board-m7dtu-pmic.o board-m7china-storage.o board-m7china-gpiomux.o board-m7dtu-keypad.o board-m7china-wifi.o board-m7china-audio.o board-m7evm-pmic.o board-m7evm.o
board-m7cdtu-all-objs += board-m7cdtu.o board-m7cdtu-pmic.o board-m7china-storage.o board-m7china-gpiomux.o board-m7cdtu-keypad.o board-m7china-wifi.o board-m7china-audio.o board-m7evm-pmic.o board-m7evm.o
board-m7cdug-all-objs += board-m7cdug.o board-m7cdug-pmic.o board-m7china-storage.o board-m7china-gpiomux.o board-m7cdug-keypad.o board-m7china-wifi.o board-m7china-audio.o board-m7evm-pmic.o board-m7evm.o
board-m7cdwg-all-objs += board-m7cdwg.o board-m7cdwg-pmic.o board-m7china-storage.o board-m7china-gpiomux.o board-m7cdwg-keypad.o board-m7china-wifi.o board-m7china-audio.o board-m7evm-pmic.o board-m7evm.o

ifdef CONFIG_FB_MSM
board-m7-all-objs += board-m7-display.o board-m7-gpu.o
board-m7wls-all-objs += board-m7wl-display.o board-m7wl-gpu.o
board-m7wlv-all-objs += board-m7wl-display.o board-m7wl-gpu.o
board-m7wlj-all-objs += board-m7wl-display.o board-m7wl-gpu.o
board-m7dcg-all-objs += board-m7china-display.o board-m7china-gpu.o
board-m7dug-all-objs += board-m7china-display.o board-m7china-gpu.o
board-m7dtu-all-objs += board-m7china-display.o board-m7china-gpu.o
board-m7cdtu-all-objs += board-m7china-display.o board-m7china-gpu.o
board-m7cdug-all-objs += board-m7china-display.o board-m7china-gpu.o
board-m7cdwg-all-objs += board-m7china-display.o board-m7china-gpu.o
endif

ifdef CONFIG_MSM_CAMERA
board-m7-all-objs += board-m7-camera.o
board-m7wls-all-objs += board-m7wl-camera.o
board-m7wlv-all-objs += board-m7wlv-camera.o
board-m7wlj-all-objs += board-m7wlj-camera.o
board-m7dcg-all-objs += board-m7china-camera.o
board-m7dug-all-objs += board-m7china-camera.o
board-m7dtu-all-objs += board-m7china-camera.o
board-m7cdtu-all-objs += board-m7china-camera.o
board-m7cdug-all-objs += board-m7china-camera.o
board-m7cdwg-all-objs += board-m7china-camera.o
endif

obj-$(CONFIG_MACH_M7_UL) += board-m7-all.o board-m7-regulator.o
obj-$(CONFIG_MACH_M7_WLS) += board-m7wls-all.o board-m7wl-regulator.o
obj-$(CONFIG_MACH_M7_WLV) += board-m7wlv-all.o board-m7wl-regulator.o
obj-$(CONFIG_MACH_M7_WLJ) += board-m7wlj-all.o board-m7wlj-regulator.o
obj-$(CONFIG_MACH_M7_DCG) += board-m7dcg-all.o board-m7china-regulator.o
obj-$(CONFIG_MACH_M7_DUG) += board-m7dug-all.o board-m7china-regulator.o
obj-$(CONFIG_MACH_M7_DTU) += board-m7dtu-all.o board-m7china-regulator.o
obj-$(CONFIG_MACH_M7C_DTU) += board-m7cdtu-all.o board-m7china-regulator.o
obj-$(CONFIG_MACH_M7C_DUG) += board-m7cdug-all.o board-m7china-regulator.o
obj-$(CONFIG_MACH_M7C_DWG) += board-m7cdwg-all.o board-m7china-regulator.o
