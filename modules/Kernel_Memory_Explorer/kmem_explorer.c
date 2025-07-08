#include <asm-generic/errno-base.h>
#include <linux/fs.h>
#include <linux/gfp_types.h>
#include <linux/kern_levels.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/uaccess.h>

#define MODULE_NAME "kmem_explorer"
enum mode_t { SHOW_MEMORY_ADDRESSES };

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VulnX");
MODULE_DESCRIPTION("Just a simple module to explore kernel memory");

struct miscdevice *misc;
struct file_operations *fops;

long kmem_explorer_ioctl(struct file *, unsigned int, unsigned long);
void show_memory_addresses(void);

void show_memory_addresses(void) {
  void *stack_var;
  static void *static_var;
  void *heap_var = kmalloc(0x100, GFP_KERNEL);
  void *module_func = kmem_explorer_ioctl;
  void *kernel_func = filp_open;
  printk(KERN_INFO "[kmem_explorer] :: stack_var = %#lx\n",
         (unsigned long)&stack_var);
  printk(KERN_INFO "[kmem_explorer] :: static_var = %#lx\n",
         (unsigned long)&static_var);
  printk(KERN_INFO "[kmem_explorer] :: heap_var = %#lx\n",
         (unsigned long)heap_var);
  printk(KERN_INFO "[kmem_explorer] :: module_func = %#lx\n",
         (unsigned long)module_func);
  printk(KERN_INFO "[kmem_explorer] :: kernel_func = %#lx\n",
         (unsigned long)kernel_func);
  kfree(heap_var);
}

long kmem_explorer_ioctl(struct file *fp, unsigned int mode,
                         unsigned long arg) {
  switch (mode) {
  case SHOW_MEMORY_ADDRESSES:
    show_memory_addresses();
    break;
  default:
    return -EINVAL;
  }
  return 0;
}

static int __init kmem_explorer_init(void) {
  fops = kzalloc(sizeof(struct file_operations), GFP_KERNEL);
  if (fops == NULL) {
    return -ENOMEM;
  }
  fops->unlocked_ioctl = kmem_explorer_ioctl;
  misc = kzalloc(sizeof(struct miscdevice), GFP_KERNEL);
  if (misc == NULL) {
    return -ENOMEM;
  }
  misc->name = MODULE_NAME;
  misc->fops = fops;
  misc->mode = S_IRUGO | S_IWUGO;
  misc_register(misc);
  printk(KERN_INFO "[kmem_explorer] :: loaded\n");
  return 0;
}

static void __exit kmem_explorer_exit(void) {
  misc_deregister(misc);
  kfree(fops);
  kfree(misc);
  printk(KERN_INFO "[kmem_explorer] :: unloaded\n");
}

module_init(kmem_explorer_init);
module_exit(kmem_explorer_exit);
