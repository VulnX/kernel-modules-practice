#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

enum mode_t {
  SHOW_MEMORY_ADDRESSES = 0x11111111,
  KERN_VIRT_TO_PHYS = 0x22222222,
  USER_VIRT_TO_PHYS = 0x33333333
};

void real_pause() {
  puts("[PAUSED] Press enter to continue...");
  getchar();
}

int main() {
  int fd = open("/dev/kmem_explorer", O_RDONLY);
  assert(0 < fd);
  ioctl(fd, SHOW_MEMORY_ADDRESSES, NULL);
  puts("");
  ioctl(fd, KERN_VIRT_TO_PHYS, NULL);
  puts("");
  ioctl(fd, USER_VIRT_TO_PHYS, "AAAABBBB");
  real_pause();
  return EXIT_SUCCESS;
}