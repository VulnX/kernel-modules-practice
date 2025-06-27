#include <linux/module.h>

static int __init kernel_memory_explorer_init(void) {
	printk(KERN_INFO "[kernel_memory_explorer] :: loaded\n");
	return 0;
}

static void __exit kernel_memory_explorer_exit(void) {
	printk(KERN_INFO "[kernel_memory_explorer] :: unloaded\n");
}

module_init(kernel_memory_explorer_init);
module_exit(kernel_memory_explorer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VulnX");