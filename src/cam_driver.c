#include "cam_driver.h"



MODULE_LICENSE("Dual BSD/GPL");

int cam_driver_Var = 0;
module_param(cam_driver_Var, int, S_IRUGO);

EXPORT_SYMBOL_GPL(cam_driver_Var);

struct class *cam_driver_class;


typedef struct cam_driver_Dev {
    unsigned short *ReadBuf;
    unsigned short *WriteBuf;
    struct semaphore SemBuf;
    unsigned short numWriter;
    unsigned short numReader;
    dev_t dev;
    struct cdev cdev;
} BDev;



struct file_operations cam_driver_fops = {
        .owner = THIS_MODULE,
        .open = cam_driver_open,
        .release = cam_driver_release,
        .read = cam_driver_read,
        .unlocked_ioctl = cam_driver_ioctl,
};


/**
 * cam_driver_init
 * @brief Fonction qui permet l'initialisation du module
 * @return Retourne un code d'erreur ou 0 en cas de succes
 *
 */
static int __init cam_driver_init (void) {

    return 0;
}

/**
 *@brief cam_driver_cleanup Fonction qui permet de quitter le module kernel
 *
 */
static void __exit cam_driver_cleanup (void) {

}

/**
 * @brief Fonction qui permet l'ouverture du driver
 * @param inode Pointeur vers la structure inode qui contient les informations sur le fichier
 * @param filp Pointeur vers la structure file qui contient les drapeaux du fichier
 * @return 0 en cas de succes sinon la valeur negative du code d'erreur
 * **/
int cam_driver_open(struct inode *inode, struct file *filp) {

    return 0;
}
/**
 * @brief Fonction qui permet la fermeture du driver
 * @param inode Pointeur vers la structure inode qui contient les informations sur le fichier
 * @param filp Pointeur vers la structure file qui contient les drapeaux du fichier
 * @return 0 en cas de succes sinon la valeur negative du code d'erreur
 * **/
int cam_driver_release(struct inode *inode, struct file *filp) {

    return 0;
}

/**
 * @brief cam_driver_read Fonction permettant la lecture de donnes dans le buffer
 * @param flip Pointeur vers la structure file du fichier
 * @param ubuf Pointeur vers le buffer de lecture du user space
 * @param count Nombre de charactere a lire
 * @param f_ops Pointeur vers l'offset que le kernel a attribue au driver
 * @return Valeur negative du code d'erreur ou nombre de charatere lu
 */
static ssize_t cam_driver_read(struct file *flip, char __user *ubuf, size_t count, loff_t *f_ops){

    return 0;
}

/**
 * @brief cam_driver_ioctl Fonction permettant l'obtention et la modification de donnees sur le buffer
 * @param flip Pointeur vers la structure file du driver
 * @param cmd Commande pour la fonction ioctl defini dans buf.h
 * @param arg argument pour la taille du nouveau buffer
 * @return retourne le parametre desirer ou un code d'erreur si la commande est inexistante
 */
long cam_driver_ioctl (struct file *flip, unsigned int cmd, unsigned long arg){
    return 0;
}

module_init(cam_driver_init);
module_exit(cam_driver_cleanup);

      	