#ifndef __LAPUTA_DEV_H__
#define __LAPUTA_DEV_H__

#include <linux/types.h>
#include <linux/ioctl.h>

/* TODO: ensure no conflict */
#define LAPUTA_MAGIC 'k'
#define IOCTL_LAPUTA_GET_API_VERSION \
    _IOR(LAPUTA_MAGIC, 1, unsigned long)
/* TODO: base addr & size */
#define IOCTL_LAPUTA_REGISTER_SHARED_MEM \
    _IOW(LAPUTA_MAGIC, 2, unsigned long [2])
#define IOCTL_LAPUTA_REQUEST_DELEG \
    _IOW(LAPUTA_MAGIC, 3, unsigned long [2])
#define IOCTL_LAPUTA_REGISTER_VCPU \
    _IO(LAPUTA_MAGIC, 4)
#define IOCTL_LAPUTA_UNREGISTER_VCPU \
    _IO(LAPUTA_MAGIC, 5)
#define IOCTL_LAPUTA_QUERY_PFN \
    _IOWR(LAPUTA_MAGIC, 6, unsigned long)
#define IOCTL_LAPUTA_RELEASE_PFN \
    _IOW(LAPUTA_MAGIC, 7, unsigned long)
#define IOCTL_REMOTE_FENCE \
    _IOR(LAPUTA_MAGIC, 8, unsigned long [2])
#define IOCTL_LAPUTA_GET_VMID \
    _IOR(LAPUTA_MAGIC, 9, unsigned long)
#define IOCTL_LAPUTA_GET_VINTERRUPT_ADDR \
    _IOR(LAPUTA_MAGIC, 10, unsigned long)
#define IOCTL_LAPUTA_GET_CPUID \
    _IOR(LAPUTA_MAGIC, 11, unsigned long)
#define IOCTL_LAPUTA_SET_VINTERRUPT \
    _IOR(LAPUTA_MAGIC, 12, unsigned long)
#define IOCTL_LAPUTA_VPLIC_CLAIM \
    _IOR(LAPUTA_MAGIC, 13, unsigned long)
#define IOCTL_LAPUTA_NET \
    _IOR(LAPUTA_MAGIC, 14, unsigned long)
#define IOCTL_LAPUTA_TTY \
    _IOR(LAPUTA_MAGIC, 15, unsigned long)
#define IOCTL_LAPUTA_DEBUG \
    _IO(LAPUTA_MAGIC, 16)
#endif
