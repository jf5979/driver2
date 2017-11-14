#include "cam_driver.h"



MODULE_LICENSE("Dual BSD/GPL");

int cam_Var = 0;
module_param(cam_Var, int, S_IRUGO);

EXPORT_SYMBOL_GPL(cam_Var);

struct class *cam_class;


typedef struct cam_Dev {
    struct semaphore SemBuf;
    dev_t dev;
    struct cdev cdev;
} CAMDev;

static struct usb_device_id cam_ids[] = {
        {USB_DEVICE(0x046d, 0x08cc)},
        {USB_DEVICE(0x1871, 0x0101)},//personnal webcam for testing
        {},
};
MODULE_DEVICE_TABLE(usb, cam_ids);
struct usb_device * cam_usb_device;
struct usb_driver cam_driver = {
        .name = "camera_driver",
        .id_table = cam_ids,
        .probe = cam_probe,
        .disconnect = cam_disconnect,
};

CAMDev cam_tool;

struct usb_interface *cam_int;

struct file_operations cam_fops = {
        .owner = THIS_MODULE,
        .open = cam_open,
        .release = cam_release,
        .read = cam_read,
        .unlocked_ioctl = cam_ioctl,
};
struct usb_class_driver cam_class_driver = {
        .name = "cam_driver",
        .fops = &cam_fops,
        .minor_base = 0,
};

/**
 * cam_init
 * @brief Fonction qui permet l'initialisation du module
 * @return Retourne un code d'erreur ou 0 en cas de succes
 *
 */
static int __init cam_init (void) {
    int result=0;
    printk(KERN_ALERT"cam_init (%s:%u) => Initialising camera driver\n", __FUNCTION__, __LINE__);
    //Reserve quatre instance mineure du driver si besoin est pour ouverture simultane
    cam_tool.dev=MKDEV(250,0);
    if (!MAJOR(cam_tool.dev)) {
        result = alloc_chrdev_region(&(cam_tool.dev), MINOR(cam_tool.dev), 0, "my_cam");
    }
    else {
        result = register_chrdev_region(cam_tool.dev, 0, "my_cam");
    }
    if (result < 0)
        printk(KERN_ALERT"cam_init ERROR IN regist_chrdev_region error code is : %d (%s:%s:%u)\n", result,__FILE__, __FUNCTION__, __LINE__);
    else
        printk(KERN_ALERT"cam_init : MAJOR = %u MINOR = %u (cam_Var = %u)\n", MAJOR(cam_tool.dev), MINOR(cam_tool.dev), cam_Var);

    cam_class = class_create(THIS_MODULE, "cam_class");
    device_create(cam_class, NULL, cam_tool.dev, NULL, "cam_node");
    cdev_init(&(cam_tool.cdev), &cam_fops);
    cam_tool.cdev.owner = THIS_MODULE;
    if (cdev_add(&(cam_tool.cdev), cam_tool.dev, 1) < 0)
        printk(KERN_ALERT"cam_init ERROR IN cdev_add (%s:%s:%u)\n", __FILE__, __FUNCTION__, __LINE__);
    result=usb_register(&cam_driver);
    if(result){
        printk(KERN_ALERT"Wasn't able to register USB driver (%s:%u)\n", __FUNCTION__, __LINE__);
        return result;
    }

    //Initialisation des variables importantes :

    sema_init(&(cam_tool.SemBuf),0);
    up(&(cam_tool.SemBuf));

    return 0;
}

/**
 *@brief cam_cleanup Fonction qui permet de quitter le module kernel
 *
 */
static void __exit cam_cleanup (void) {
    usb_deregister(&cam_driver);
    cdev_del(&(cam_tool.cdev));
    unregister_chrdev_region(cam_tool.dev, 0);
    device_destroy (cam_class, cam_tool.dev);
    class_destroy(cam_class);

    printk(KERN_ALERT "cam_cleanup (%s:%u) => Module unloaded successfully\n", __FUNCTION__, __LINE__);
}

/**
 * @brief Fonction qui permet l'ouverture du driver
 * @param inode Pointeur vers la structure inode qui contient les informations sur le fichier
 * @param filp Pointeur vers la structure file qui contient les drapeaux du fichier
 * @return 0 en cas de succes sinon la valeur negative du code d'erreur
 * **/
int cam_open(struct inode *inode, struct file *filp) {
    int subminor;
    printk(KERN_WARNING "ELE784 -> Open (%s:%u)\n", __FUNCTION__, __LINE__);
    subminor = iminor(inode);
    cam_int = usb_find_interface(&cam_driver, subminor);
    if (!cam_int) {
        printk(KERN_WARNING "ELE784 -> Open: Ne peux ouvrir le peripherique iminor %d (%s:%u)\n",subminor, __FUNCTION__, __LINE__);
        return -ENODEV;
    }
    filp->private_data = cam_int;
    return 0;
}
/**
 * @brief Fonction qui permet la fermeture du driver
 * @param inode Pointeur vers la structure inode qui contient les informations sur le fichier
 * @param filp Pointeur vers la structure file qui contient les drapeaux du fichier
 * @return 0 en cas de succes sinon la valeur negative du code d'erreur
 * **/
int cam_release(struct inode *inode, struct file *filp) {
    printk(KERN_WARNING"Driver released(%s:%u)\n", __FUNCTION__, __LINE__);
    return 0;
}

/**
 * @brief cam_read Fonction permettant la lecture de donnes dans le buffer
 * @param flip Pointeur vers la structure file du fichier
 * @param ubuf Pointeur vers le buffer de lecture du user space
 * @param count Nombre de charactere a lire
 * @param f_ops Pointeur vers l'offset que le kernel a attribue au driver
 * @return Valeur negative du code d'erreur ou nombre de charatere lu
 */
static ssize_t cam_read(struct file *flip, char __user *ubuf, size_t count, loff_t *f_ops){
    printk(KERN_WARNING"DANS la fonction read(%s:%u)\n", __FUNCTION__, __LINE__);
    return 0;
}

/**
 * @brief cam_ioctl Fonction permettant l'obtention et la modification de donnees sur le buffer
 * @param flip Pointeur vers la structure file du driver
 * @param cmd Commande pour la fonction ioctl defini dans buf.h
 * @param arg argument pour la taille du nouveau buffer
 * @return retourne le parametre desirer ou un code d'erreur si la commande est inexistante
 */
long cam_ioctl (struct file *flip, unsigned int cmd, unsigned long arg){
    printk(KERN_WARNING"DANS ioctl\n");
    switch(cmd){
        case IOCTL_GET:
            return 0;
        case IOCTL_SET:
            return 0;
        case IOCTL_STREAMON:
            return 0;
        case IOCTL_STREAMOFF:
            return 0;
        case IOCTL_GRAB:
            return 0;
        case IOCTL_PANTILT:
            return 0;
        case IOCTL_PANTILT_RESEST:
            return 0;
        default:
            return 0;
    }

}
/**
 * @brief   Function Probe afin de verifier la compatibilite du device usb (extrait de ELE784 cours 5 p.27-28)
 * @param intf
 * @param id
 * @return
 */

int cam_probe(struct usb_interface *intf, const struct usb_device_id *id){
    struct usb_host_interface * interface;
    struct usb_device *dev = interface_to_usbdev (intf);
    printk(KERN_WARNING"CAMERA PLUGGED IN (%04X:%04X) (%s:%u)\n",id->idVendor, id->idProduct,__FUNCTION__, __LINE__);
    interface=&(intf->altsetting[0]);

    printk(KERN_WARNING"probing (%s:%u)\n", __FUNCTION__, __LINE__);

    usb_set_intfdata (intf, dev);
    usb_register_dev (intf, &cam_class_driver);
    usb_set_interface(dev, 1, 4);
    return 0;
}

/**
 *
 * @param intf
 */
void  cam_disconnect(struct usb_interface *intf){
   // usb_deregister_dev(&notre_usb_dev);
    printk(KERN_WARNING"Device disconnected  (%s:%u)\n", __FUNCTION__, __LINE__);

}

module_init(cam_init);
module_exit(cam_cleanup);

      	