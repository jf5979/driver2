#ifndef CAM_H
#define CAM_H


#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/usb.h>


/* Use 'W' as magic number */
#define CAM_IOC_MAGIC  'W'
/* Please use a different 8-bit number in your code */

#define CAM_IOCRESET    _IO(BUFF_IOC_MAGIC, 0)

#define IOCTL_GET               _IO(CAM_IOC_MAGIC,  0x10)
#define IOCTL_SET               _IO(CAM_IOC_MAGIC,  0x20)
#define IOCTL_STREAMON          _IO(CAM_IOC_MAGIC,  0x30)
#define IOCTL_STREAMOFF         _IO(CAM_IOC_MAGIC,  0x40)
#define IOCTL_GRAB              _IO(CAM_IOC_MAGIC,  0x50)
#define IOCTL_PANTILT           _IO(CAM_IOC_MAGIC,  0x60)
#define IOCTL_PANTILT_RESEST    _IO(CAM_IOC_MAGIC,  0x70)

static int cam_init(void);

static void cam_cleanup(void);

int cam_open (struct inode *inode, struct file *flip);

int cam_release (struct inode *inode, struct file *flip);

static ssize_t cam_read(struct file *flip, char __user *ubuf, size_t count, loff_t *f_ops);

long cam_ioctl (struct file *flip, unsigned int cmd, unsigned long arg);

int cam_probe(struct usb_interface *intf, const struct usb_device_id *id);

void cam_disconnect(struct usb_interface *intf);


#endif