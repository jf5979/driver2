#include "cam_driver.h"
#include "usbvideo.h"

MODULE_LICENSE("Dual BSD/GPL");

int cam_Var = 0;
module_param(cam_Var, int, S_IRUGO);

EXPORT_SYMBOL_GPL(cam_Var);

struct class *cam_class;
struct completion urb_completed;

typedef struct cam_Dev {
    struct semaphore SemBuf;
    dev_t dev;
    struct cdev cdev;
    int nb_device;
    struct usb_interface *cam_int;
    unsigned int myStatus;
    unsigned int myLengthUsed;
    char * myData;
    unsigned int urb_completed;
    unsigned int cam_minor;
} CAMDev;
struct urb * myUrb[5];
static struct usb_device_id cam_ids[] = {
        {USB_DEVICE(0x046d, 0x0994)},
        {USB_DEVICE(0x1871, 0x0101)},//personnal webcam for testing
        {USB_DEVICE(0x046d, 0x08cc)},
        {}
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



struct file_operations cam_fops = {
        .owner = THIS_MODULE,
        .open = cam_open,
        .release = cam_release,
        .read = cam_read,
        .unlocked_ioctl = cam_ioctl,
};
struct usb_class_driver cam_class_driver = {
        .name = "cm_driver",
        .fops = &cam_fops,
        .minor_base = 0,
};
static DECLARE_WAIT_QUEUE_HEAD(reading_queue);

/**
 * cam_init
 * @brief Fonction qui permet l'initialisation du module
 * @return Retourne un code d'erreur ou 0 en cas de succes
 *
 */
static int __init cam_init (void) {
    int result=0,i;
    printk(KERN_ALERT"cam_init (%s:%u) => Initialising camera driver\n", __FUNCTION__, __LINE__);
    //Initialisation des variables importantes :
    cam_tool.nb_device=0;
    cam_tool.urb_completed=0;
    cam_tool.myData=NULL;
    init_completion(&urb_completed);
    sema_init(&(cam_tool.SemBuf),0);
    up(&(cam_tool.SemBuf));

    for(i=0;i<NB_URBS;i++){
         myUrb[i]=NULL;
    }

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
    struct usb_interface * cam_int_temp;
    int subminor;
    printk(KERN_WARNING "ELE784 -> Open (%s:%u)\n", __FUNCTION__, __LINE__);
    subminor =cam_tool.cam_minor;
    cam_int_temp = usb_find_interface(&cam_driver, subminor);
    if (!cam_int_temp) {
        printk(KERN_WARNING "ELE784 -> Open: Ne peux ouvrir le peripherique iminor %d (%s:%u)\n",subminor, __FUNCTION__, __LINE__);
        return -ENODEV;
    }
    filp->private_data = cam_int_temp;
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
 * @brief cam_read Fonction permettant la lecture de donnes graphique
 * @param flip Pointeur vers la structure file du fichier
 * @param ubuf Pointeur vers le buffer de lecture du user space
 * @param count Nombre de charactere a lire
 * @param f_ops Pointeur vers l'offset que le kernel a attribue au driver
 * @return Valeur negative du code d'erreur ou nombre de charatere lu
 */
static ssize_t cam_read(struct file *flip, char __user *ubuf, size_t count, loff_t *f_ops){
    struct usb_interface *intf;
    struct usb_device *dev ;
    unsigned int copied,i;
    intf=flip->private_data;
    dev= usb_get_intfdata(intf);

    wait_for_completion_interruptible(&urb_completed);

    printk(KERN_ALERT"Data value urb_complete %d my_status %d",cam_tool.urb_completed,cam_tool.myStatus);

    urb_completed.done=0;
    cam_tool.urb_completed=0;
    cam_tool.myStatus=0;

    printk(KERN_WARNING"DANS la fonction read(%s:%u)\n", __FUNCTION__, __LINE__);
    copied=copy_to_user(ubuf, cam_tool.myData, cam_tool.myLengthUsed);
    for (i = 0; i < NB_URBS; i++) {
        usb_kill_urb( myUrb[i]);
        usb_free_coherent(cam_usb_device,  myUrb[i]->transfer_buffer_length,  myUrb[i]->transfer_buffer, myUrb[i]->transfer_dma);
        usb_free_urb( myUrb[i]);
         myUrb[i]=NULL;
    }
    kfree(cam_tool.myData);
    copied=cam_tool.myLengthUsed-copied;
    cam_tool.myLengthUsed=0;

    return (ssize_t) copied;

}

/**
 * @brief cam_ioctl Fonction permettant l'interaction avec le peripherique USB
 * @param flip Pointeur vers la structure file du driver
 * @param cmd Commande pour la fonction ioctl defini dans cam_driver.h
 * @param arg argument pour les differentes commandes
 * @return retourne le parametre desirer ou un code d'erreur si la commande est inexistante
 */
long cam_ioctl (struct file *file, unsigned int cmd, unsigned long arg){
    struct usb_interface *intf = file->private_data;
    struct usb_device *dev = usb_get_intfdata(intf);
    int buffer,i,j,nbPackets,size,myPacketSize,ret;
    unsigned char commande[4]={0x00,0x00,0x00,0x00};
    struct usb_host_interface * cur_altsetting;
    struct usb_endpoint_descriptor endpointDesc;
    int reponse[4];
    char command;
    int teste[2]={0x30,0x30};

    switch(cmd){
        case IOCTL_GET:
            if(copy_from_user(reponse,(char*)arg,1)==0){
                command=reponse[0];
                ret=usb_control_msg(dev,usb_rcvctrlpipe(dev,0),GET_CUR,
                                   (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE),
                                   command<<8,0x0200,teste,2,0);
                if(ret<0){
                    reponse[0]=ret;
                } else{
                    reponse[0]=teste[0]<<8|teste[1];
                }
                ret=usb_control_msg(dev,usb_rcvctrlpipe(dev,0),GET_MIN,
                                           (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE),
                                           command<<8,0x0200,teste,2,0);
                if(ret<0){
                    reponse[1]=ret;
                } else{
                    reponse[1]=teste[0]<<8|teste[1];
                }
                ret=usb_control_msg(dev,usb_rcvctrlpipe(dev,0),GET_MAX,
                                           (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE),
                                           command<<8,0x0200,teste,2,0);
                if(ret<0){
                    reponse[2]=ret;
                } else{
                    reponse[2]=teste[0]<<8|teste[1];
                }
                ret=usb_control_msg(dev,usb_rcvctrlpipe(dev,0),GET_RES,
                                           (USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE),
                                           command<<8,0x0200,teste,2,0);
                if(ret<0){
                    reponse[3]=ret;
                } else{
                    reponse[3]=teste[0]<<8|teste[1];
                }

                if(copy_to_user((char*)arg,reponse,4)==0){
                    return 0;
                }
                else{
                    return -ENOEXEC;
                }
            }
            else{
                return -ENODATA;
            }
        case IOCTL_SET:
            if (copy_from_user(reponse, (char *) arg, 3) == 0) {
                return usb_control_msg(dev, usb_sndctrlpipe(dev, 0), SET_CUR,
                                       (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE),
                                       (reponse[0]<<8), 0x0200, &(reponse[1]), 2, 0);
            }

        case IOCTL_STREAMON:
            printk(KERN_ALERT"STREAMON\n");
            return  usb_control_msg(dev, usb_sndctrlpipe(dev,0),0x0B,
                                    (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE),
                                    0x0004,0x0001,NULL,0,0);
        case IOCTL_STREAMOFF:
            printk(KERN_ALERT"STREAMOFF\n");
            return  usb_control_msg(dev,usb_sndctrlpipe(dev,0),0x0B,
                                    (USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_INTERFACE),
                                    0x0000,0x0001,NULL,0,0);
        case IOCTL_GRAB:
            init_completion(&urb_completed);
            printk(KERN_ALERT"GRAB\n");
            cam_tool.myData=kmalloc(sizeof(char)*MY_LENGTH,GFP_KERNEL);
            if(cam_tool.myData==NULL){
                return -ENOEXEC;
            }

            cam_tool.myStatus=0;
            cam_tool.urb_completed=0;
            cam_tool.myLengthUsed=0;
            cur_altsetting = intf->cur_altsetting;
            endpointDesc = cur_altsetting->endpoint[0].desc;

            nbPackets = 40;  // The number of isochronous packets this urb should contain
            myPacketSize = le16_to_cpu(endpointDesc.wMaxPacketSize);
            size = myPacketSize * nbPackets;

            for (i = 0; i < NB_URBS; ++i) {
                usb_free_urb( myUrb[i]); // Pour être certain

                 myUrb[i] = usb_alloc_urb(nbPackets,GFP_ATOMIC);

                if ( myUrb[i] == NULL) {
                    return -ENOMEM;
                }
                 myUrb[i]->transfer_buffer = usb_alloc_coherent(cam_usb_device,size,GFP_ATOMIC,&( myUrb[i]->transfer_dma));

                if ( myUrb[i]->transfer_buffer == NULL) {
                    usb_free_coherent(cam_usb_device,size,&(endpointDesc.bEndpointAddress), myUrb[i]->transfer_dma);
                    return -ENOMEM;
                }
                 myUrb[i]->dev = cam_usb_device;
                 myUrb[i]->context = (CAMDev *) &cam_tool;
                 myUrb[i]->pipe = usb_rcvisocpipe(cam_usb_device, endpointDesc.bEndpointAddress);
                 myUrb[i]->transfer_flags = URB_ISO_ASAP | URB_NO_TRANSFER_DMA_MAP;
                 myUrb[i]->interval = endpointDesc.bInterval;
                 myUrb[i]->complete = &complete_callback;
                 myUrb[i]->number_of_packets = nbPackets;
                 myUrb[i]->transfer_buffer_length = size;
                for (j = 0; j < nbPackets; ++j) {
                     myUrb[i]->iso_frame_desc[j].offset = j * myPacketSize;
                     myUrb[i]->iso_frame_desc[j].length = myPacketSize;
                }
            }
            for(i = 0; i < NB_URBS; i++){
                if ((ret = usb_submit_urb( myUrb[i],GFP_ATOMIC)) < 0) {
                    printk(KERN_ALERT"Cannot submit urb : number %d error code %d",i,ret);

                    return ret;
                }
            }
            return 0;
        case IOCTL_PANTILT:
            switch (arg){
                case HAUT:
                    commande[0]=0x00;commande[1]=0x00;commande[2]=0x80;commande[3]=0xff;
                    break;
                case BAS:
                    commande[0]=0x00;commande[1]=0x00;commande[2]=0x80;commande[3]=0x00;
                    break;
                case GAUCHE:
                    commande[0]=0x80;commande[1]=0x00;commande[2]=0x00;commande[3]=0x00;
                    break;
                case DROIT:
                    commande[0]=0x80;commande[1]=0xFF;commande[2]=0x00;commande[3]=0x00;
                    break;
                default:
                    commande[0]=0x00;commande[1]=0x00;commande[2]=0x80;commande[3]=0xff;
                    break;
            }
            return usb_control_msg(dev,usb_sndctrlpipe(dev,0),0x01,
                                   (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE),
                                   0x0100,0x0900,(unsigned char *) commande ,4,0);
        case IOCTL_PANTILT_RESEST:
            buffer=0x03;
            return usb_control_msg(dev,usb_sndctrlpipe(dev,0),0x01,
                                   (USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE),
                                   0x0200,0x0900,(int *) &buffer,1,0);
        default:
            return 0;
    }

}
/**
 * @brief   Function Probe afin de verifier la compatibilite du device usb (extrait de ELE784 cours 5 p.27-28)
 * @param intf l'interface usb qui correspond au pilote
 * @param id identifiant du peripherique usb
 * @return retourne le code derreur ou 0 en cas de reussite
 */

int cam_probe(struct usb_interface *intf, const struct usb_device_id *id){
    struct usb_device *dev = interface_to_usbdev(intf);
    int retval = -ENOMEM;
    cam_usb_device = NULL;
    //les 3 premiers IF ont ete fortement inspirer par le travail
    //d'un collegue sur github https://github.com/ELE784/USB_Driver/blob/master/usbcam.c
    if (intf->altsetting->desc.bInterfaceClass == CC_VIDEO)
    {
        if (intf->altsetting->desc.bInterfaceSubClass == SC_VIDEOCONTROL)
            return 0;
        if (intf->altsetting->desc.bInterfaceSubClass == SC_VIDEOSTREAMING)
        {
            /* allocate memory for our device state and initialize it */
            cam_usb_device = kmalloc(sizeof(struct usb_device), GFP_KERNEL);
            if(cam_usb_device == NULL)
            {
                printk(KERN_WARNING "No memory available\n");
                return retval;
            }

            cam_usb_device= usb_get_dev(dev);
            usb_set_intfdata(intf, cam_usb_device);
            retval = usb_register_dev(intf, &cam_class_driver);
            if (retval < 0)
            {
                printk(KERN_WARNING"Not able register this device.\n");
                usb_set_intfdata(intf, NULL);
                return -ENOEXEC;
            }
            down_interruptible(&(cam_tool.SemBuf));
            cam_tool.nb_device+=1;
            up(&(cam_tool.SemBuf));
            usb_set_interface(dev, 1, 4);
            printk(KERN_WARNING "usbcam device now attached to usbcam-%d\n", intf->minor);
            cam_tool.cam_minor=intf->minor;
        }
        else
            retval = -ENODEV;
    }
    else
        retval = -ENODEV;

    return retval;

}



/**
 *@brief Fonction s'ocupant de la deconnection du peripherique
 * @param intf
 */
void  cam_disconnect(struct usb_interface *intf){
    if(intf->altsetting->desc.bInterfaceClass == CC_VIDEO) {
        if(intf->altsetting->desc.bInterfaceSubClass == SC_VIDEOSTREAMING) {
            down_interruptible(&(cam_tool.SemBuf));
            if (cam_tool.nb_device != 0) {
                cam_tool.nb_device = 0;
                usb_set_intfdata(intf, NULL);
                usb_deregister_dev(intf, &cam_class_driver);
                printk(KERN_WARNING"Device disconnected  (%s:%u)\n", __FUNCTION__, __LINE__);
            }
            up(&(cam_tool.SemBuf));
        }
    }

}

/**
 * @brief Fonction s'occupant du retour du urb
 * @param urb le urb soumit
 */
void complete_callback(struct urb *urb){

    int ret;
    int i;
    unsigned char * data;
    unsigned int len;
    unsigned int maxlen;
    unsigned int nbytes;
    void * mem;
    CAMDev * cam_tool_2;
    cam_tool_2 = (CAMDev *) (urb->context);
    if(urb->status == 0){
        for (i = 0; i < urb->number_of_packets; ++i) {

            if(cam_tool_2->myStatus == 1){
                continue;
            }
            if (urb->iso_frame_desc[i].status < 0) {
                printk(KERN_WARNING"Error ins transfer error code is %d",urb->iso_frame_desc[i].status);
                continue;
            }

            data = urb->transfer_buffer + urb->iso_frame_desc[i].offset;
            if(data[1] & (1 << 6)){
                continue;
            }
            len = urb->iso_frame_desc[i].actual_length;
            if (len < 2 || data[0] < 2 || data[0] > len){
                continue;
            }

            len -= data[0];
            maxlen = MY_LENGTH - cam_tool_2->myLengthUsed ;
            mem = cam_tool_2->myData + cam_tool_2->myLengthUsed;
            nbytes = min(len, maxlen);
            memcpy(mem, data + data[0], nbytes);
            cam_tool_2->myLengthUsed += nbytes;

            if (len > maxlen) {
                cam_tool_2->myStatus = 1; // DONE
            }

            /* Mark the buffer as done if the EOF marker is set. */
            if ((data[1] & (1 << 1)) && (cam_tool_2->myLengthUsed != 0)) {
                cam_tool_2->myStatus = 1; // DONE
            }
        }
        if (!(cam_tool_2->myStatus == 1)){
            if ((ret = usb_submit_urb(urb, GFP_ATOMIC)) < 0) {
                printk(KERN_WARNING "Failed to submit in callback");
            }
        }else{
            /**
             * Partie du code dans laquelle il faut synchroniser avec read
             */
             cam_tool_2->urb_completed++;
                if(cam_tool_2->urb_completed>=5){
                    cam_tool_2->urb_completed=0;
                    cam_tool_2->myStatus=0;
                    complete(&urb_completed);
                }
             cam_tool_2->myStatus=0;
        }
    }else{
        printk(KERN_WARNING "Failed at beginning");
    }
}

module_init(cam_init);
module_exit(cam_cleanup);

      	