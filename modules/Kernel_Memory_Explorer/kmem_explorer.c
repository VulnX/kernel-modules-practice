#include <asm-generic/errno-base.h>
#include <asm-generic/memory_model.h>
#include <asm/current.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/pgtable_types.h>
#include <linux/fs.h>
#include <linux/gfp_types.h>
#include <linux/kern_levels.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/pgtable.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <vdso/page.h>

#define MODULE_NAME "kmem_explorer"
enum mode_t {
  SHOW_MEMORY_ADDRESSES = 0x11111111,
  KERN_VIRT_TO_PHYS = 0x22222222,
  USER_VIRT_TO_PHYS = 0x33333333
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VulnX");
MODULE_DESCRIPTION("Just a simple module to explore kernel memory");

struct miscdevice *misc;
struct file_operations *fops;

long kmem_explorer_ioctl(struct file *, unsigned int, unsigned long);
void show_memory_addresses(void);
void kernel_virt_to_phys(void);
void user_virt_to_phys(size_t);

static phys_addr_t my_virt_to_phys(void *addr) {
  struct page *page;
  if (is_vmalloc_addr(addr)) {
    page = vmalloc_to_page(addr);
  } else {
    page = virt_to_page(addr);
  }
  return page_to_phys(page) | ((size_t)addr & ~PAGE_MASK);
}

long kmem_explorer_ioctl(struct file *fp, unsigned int mode,
                         unsigned long arg) {
  switch (mode) {
  case SHOW_MEMORY_ADDRESSES:
    show_memory_addresses();
    break;
  case KERN_VIRT_TO_PHYS:
    kernel_virt_to_phys();
    break;
  case USER_VIRT_TO_PHYS:
    user_virt_to_phys(arg);
    break;
  default:
    return -EINVAL;
  }
  return 0;
}

void show_memory_addresses(void) {
  void *stack_var;
  static void *static_var;
  void *heap_var = kmalloc(0x100, GFP_KERNEL);
  void *module_func = kmem_explorer_ioctl;
  void *kernel_func = filp_open;
  printk(KERN_INFO "[show_kmem] :: stack_var = %#lx\n", (size_t)&stack_var);
  printk(KERN_INFO "[show_kmem] :: static_var = %#lx\n", (size_t)&static_var);
  printk(KERN_INFO "[show_kmem] :: heap_var = %#lx\n", (size_t)heap_var);
  printk(KERN_INFO "[show_kmem] :: module_func = %#lx\n", (size_t)module_func);
  printk(KERN_INFO "[show_kmem] :: kernel_func = %#lx\n", (size_t)kernel_func);
  kfree(heap_var);
}

void kernel_virt_to_phys(void) {
  char *heap_str = kzalloc(32, GFP_KERNEL);
  strcpy(heap_str, "HEAP STRING");
  char stack_str[128];
  strcpy(stack_str, "STACK STRING");
  printk(KERN_INFO "[kernel_v2p] :: virtual address(heap): %#lx\n",
         (size_t)heap_str);
  printk(KERN_INFO "[kernel_v2p] :: physical address(heap): %#lx\n",
         (size_t)my_virt_to_phys(heap_str));
  printk(KERN_INFO "[kernel_v2p] :: virtual address(stack): %#lx\n",
         (size_t)stack_str);
  printk(KERN_INFO "[kernel_v2p] :: physical address(stack): %#lx\n",
         (size_t)my_virt_to_phys(stack_str));
  kfree(heap_str);
}

void user_virt_to_phys(size_t uaddr) {
  size_t ret = 0;
  struct mm_struct *mm = current->mm;
  down_read(&mm->mmap_lock);
  pgd_t *pgd = pgd_offset(mm, uaddr);
  if (pgd_none(*pgd)) {
    goto out;
  }
  p4d_t *p4d = p4d_offset(pgd, uaddr);
  if (p4d_none(*p4d)) {
    goto out;
  }
  pud_t *pud = pud_offset(p4d, uaddr);
  if (pud_none(*pud)) {
    goto out;
  }
  pmd_t *pmd = pmd_offset(pud, uaddr);
  if (pmd_none(*pmd)) {
    goto out;
  }
  pte_t *pte = pte_offset_kernel(pmd, uaddr);
  if (!pte_present(*pte)) {
    goto out;
  }
  struct page *page = pte_page(*pte);
  phys_addr_t phys = page_to_phys(page) | (uaddr & ~PAGE_MASK);
  ret = *(size_t *)(page_offset_base + phys);
  printk(KERN_INFO "[user_v2p] :: virtual address(userspace): %#lx\n", uaddr);
  printk(KERN_INFO
         "[user_v2p] :: physical address(userspace): [%#lx] => %#lx\n",
         (size_t)phys, ret);
out:
  up_read(&mm->mmap_lock);
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
