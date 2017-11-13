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

static int cam_driver_init(void);

static void cam_driver_cleanup(void);

int cam_driver_open (struct inode *inode, struct file *flip);

int cam_driver_release (struct inode *inode, struct file *flip);

static ssize_t cam_driver_read(struct file *flip, char __user *ubuf, size_t count, loff_t *f_ops);

long cam_driver_ioctl (struct file *flip, unsigned int cmd, unsigned long arg);

void cam_driver_disconnect(struct usb_interface *intf);