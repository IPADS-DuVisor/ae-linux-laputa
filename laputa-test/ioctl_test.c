#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>

#include "../../include/uapi/misc/laputa_dev.h"

#define IOCTL_DRIVER_NAME "/dev/laputa_dev"
            
int open_driver(const char* driver_name) {
    printf("* Open Driver\n");

    int fd_driver = open(driver_name, O_RDWR);
    if (fd_driver == -1) {
        printf("ERROR: could not open \"%s\".\n", driver_name);
        printf("    errno = %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return fd_driver;
}

void close_driver(const char* driver_name, int fd_driver) {
    printf("* Close Driver\n");

    int result = close(fd_driver);
    if (result == -1) {
        printf("ERROR: could not close \"%s\".\n", driver_name);
        printf("    errno = %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

#include "log.h"
#include "mmio.h"
#include "stats.h"
#include "virtio_mmio.h"
#include "virtio_type.h"

#define PATH_MAX 4096

void remove_virtio_driver(const char *name) {
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/sys/bus/virtio/devices/%s/driver/unbind", name);
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        debug("no driver binded");
        return;
    }
    if (write(fd, name, strlen(name)) != (ssize_t)strlen(name)) {
        error("failed to unbind driver for %s", name); 
    } else {
        debug("succeed to remove the driver of %s", name);
    }
    close(fd);
}
    
#include "memory.c"
#include "virtio.c"
#include "stats.c"
#include "pktgen.c"
// #include "pcap.c"
// #include "forward.c"

int main(void) {
    int fd_ioctl;
    /* VIRTIO MMIO GPA: 0x10008000 - 0x10008fff */
    void *mmio_addr = (void *)0x3000008000UL;
    void *test_buf;
    size_t test_buf_size = 0x1000;

    fd_ioctl = open_driver(IOCTL_DRIVER_NAME);
    test_buf = mmap(mmio_addr, test_buf_size, 
            PROT_READ | PROT_WRITE, MAP_SHARED, fd_ioctl, 0);
    if (test_buf == MAP_FAILED) {
        perror("MAP_FAILED");
        return EXIT_FAILURE;
    } else if (test_buf != mmio_addr) {
        printf("ERROR: test_buf: %p, expected: %p\n", test_buf, mmio_addr);
        return EXIT_FAILURE;
    } else {
        printf("MAP_SUCCEED\n");
        printf("read STATUS: %x\n", *(char *)(0x3000008000 + 18));
        printf("read QUEUE_NUM: %x\n", *(short *)(0x3000008000 + 12));
    }

    struct virtio_device *dev = virtio_init("virtio0", 1, 1);
    pktgen(dev);
    // pcap(dev);
    // forward(dev);

    close_driver(IOCTL_DRIVER_NAME, fd_ioctl);
    return EXIT_SUCCESS;
}
