#ifndef __LINUX_PL_SENSOR_H
#define __LINUX_PL_SENSOR_H

extern struct blocking_notifier_head psensor_notifier_list;

extern int register_notifier_by_psensor(struct notifier_block *nb);
extern int unregister_notifier_by_psensor(struct notifier_block *nb);
int psensor_enable_by_touch_driver(int on);
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS_SWEEP2WAKE
int pocket_detection_check(void);
#endif
int power_key_check_in_pocket(void);
#endif

