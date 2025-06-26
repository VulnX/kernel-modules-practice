#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_NAME "/dev/hello"
#define BUFFER_SIZE 100

int main() {
    int fd;
    char write_buffer[] = "Hello, kernel module!";
    char read_buffer[BUFFER_SIZE];

    // Open the device file
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return 1;
    }

    // Write data to the device
    printf("Writing to the device: %s\n", write_buffer);
    if (write(fd, write_buffer, strlen(write_buffer)) < 0) {
        perror("Failed to write to the device");
        close(fd);
        return 1;
    }

    // Read data from the device
    if (read(fd, read_buffer, BUFFER_SIZE) < 0) {
        perror("Failed to read from the device");
        close(fd);
        return 1;
    }

    printf("Read from the device: %s\n", read_buffer);

    // Close the device file
    close(fd);
    return 0;
}
