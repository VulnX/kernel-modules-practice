#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

enum mode_t { SHOW_MEMORY_ADDRESSES };

void real_pause() {
  puts("[PAUSED] Press enter to continue...");
  getchar();
}

int main() {
  int fd = open("/dev/kmem_explorer", O_RDONLY);
  assert(0 < fd);
  ioctl(fd, SHOW_MEMORY_ADDRESSES, NULL);
  real_pause();
  return EXIT_SUCCESS;
}