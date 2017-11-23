//
// Created by jf on 11/16/17.
//

#ifndef DRIVER2_TESTBENCH_H
#define DRIVER2_TESTBENCH_H

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

#define HAUT    1
#define BAS     2
#define GAUCHE  3
#define DROIT   4

#endif //DRIVER2_TESTBENCH_H
