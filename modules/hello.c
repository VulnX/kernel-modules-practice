#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>

#define DEVICE_NAME "hello"

ssize_t hello_read(struct file *, char __user *, size_t, loff_t *);
ssize_t hello_write(struct file *, const char __user *, size_t, loff_t *);
static int __init hello_init(void);
static void __exit hello_exit(void);

static struct file_operations fops = {
	.read = hello_read,
	.write = hello_write,
};
static struct miscdevice hello_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &fops,
	.mode = 0666,
};

#define BUFFER_MAX_SIZE 100

typedef struct {
	char buffer[BUFFER_MAX_SIZE];
	size_t size;
} data_t;

static data_t data;

ssize_t hello_read(struct file *fp, char __user *buf, size_t size, loff_t *off) {
	int ret = copy_to_user(buf, data.buffer, data.size);
	printk(KERN_INFO "Sent data back to userland!\n");
	return ret;
}

ssize_t hello_write(struct file *fp, const char __user *buf, size_t size, loff_t *off) {
	size = min(size, BUFFER_MAX_SIZE);
	if (copy_from_user(data.buffer, buf, size)) {
		return -EFAULT;
	}
	data.size = size;
	printk(KERN_INFO "Saved your data\n");
	return 0;
}

static int __init hello_init(void) {
	printk(KERN_INFO "Hello module got loaded\n");
	if (misc_register(&hello_device)) {
		printk(KERN_ERR "Unable to register misc device\n");
		return -EFAULT;
	}
	return 0;
}

static void __exit hello_exit(void) {
	misc_deregister(&hello_device);
	printk(KERN_INFO "Bye bye hello module\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VulnX");
MODULE_DESCRIPTION("Nothing fancy, just a simple module");