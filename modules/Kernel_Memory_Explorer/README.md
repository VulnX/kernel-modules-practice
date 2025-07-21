# Kernel Memory Explorer

TODO

# Learnt

## General

- [Which error codes to use and when](https://fastbitlab.com/linux-device-driver-programming-lecture-40-understanding-error-codes/)

## Virtual Memory

### Translating virtual (kernel) to physical

Typically for translating a kernel virtual memory address to physical memory address, we would use [`virt_to_phys`](https://elixir.bootlin.com/linux/v6.15.5/source/arch/x86/include/asm/io.h#L116) ( or its counterpart [`phys_to_virt`](https://elixir.bootlin.com/linux/v6.15.5/source/arch/x86/include/asm/io.h#L135) for the other way around ).

But they only work for *directly mapped memory* addresses or addresses obtained via [`kmalloc`](https://elixir.bootlin.com/linux/v6.15.5/source/tools/lib/slab.c#L14).

This means that traditional heap address will work well with it but stack addresses will not, since they are allocated via [`vmalloc`](https://elixir.bootlin.com/linux/v6.15.5/source/include/linux/vmalloc.h#L139) based on my kernel config ( `CONFIG_VMAP_STACK=y` ).

> NOTE: For difference between `kmalloc` and `vmalloc` read [this](https://stackoverflow.com/questions/116343/what-is-the-difference-between-vmalloc-and-kmalloc) and [this](https://medium.com/@aravindchetla/kmalloc-v-s-vmalloc-13cb60746bcc)

Hence I have made a little wrapper function `my_virt_to_phys` to get the corresponding page via either [`vmalloc_to_page`](https://elixir.bootlin.com/linux/v6.15.5/source/mm/vmalloc.c#L739) or [`virt_to_page`](https://elixir.bootlin.com/linux/v6.15.5/source/arch/x86/include/asm/page.h#L68) depending on the context, and later return the resolved phyiscal address of that page via [`page_to_phys`](https://elixir.bootlin.com/linux/v6.15.5/source/include/asm-generic/memory_model.h#L77). It's not perfect but works well for stack and heap addresses at least.

### Inspecting physical addresses in GDB

As mentioned [here](https://www.kernel.org/doc/Documentation/x86/x86_64/mm.txt), the kernel maintains a 1:1 map of all physical memory into the kernel's virtual address space at `page_offset_base`

This means by just adding the physical address as an "offset", we can start reading data from RAM.

dmesg:

```
[   37.900978] [kmem_explorer] :: virtual address(heap): 0xffff888004678f60
[   37.901267] [kmem_explorer] :: physical address(heap): 0x4678f60
[   37.901516] [kmem_explorer] :: virtual address(stack): 0xffffc900001c7e60
[   37.901973] [kmem_explorer] :: physical address(stack): 0x478be60
```

GDB:

```
pwndbg> x/s 0xffff888004678f60
0xffff888004678f60: "HEAP STRING"
pwndbg> x/s page_offset_base+0x4678f60
0xffff888004678f60: "HEAP STRING"
pwndbg> x/s 0xffffc900001c7e60
0xffffc900001c7e60: "STACK STRING"
pwndbg> x/s page_offset_base+0x478be60
0xffff88800478be60: "STACK STRING"
```

### Translating virtual (userspace) to physical

For translating userspace virtual to physical memory addresses, to my knowledge, there is no way other than walking the page tables manually *(please let me know if otherwise)*.

Useful articles regarding this topic:

- [zolutal's blog](https://blog.zolutal.io/understanding-paging/) to walk the page tables manually in GDB

- [OSDev Wiki](https://wiki.osdev.org/Paging), particularly 64-bit paging section

- [Page Tables](https://docs.kernel.org/mm/page_tables.html) from the official kernel docs