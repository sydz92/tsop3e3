#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

// Nombre y clase de dispositivo
#define  DEVICE_NAME "tsofifo"
#define  CLASS_NAME  "tsofifo"

//Informacion del Driver
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santiago Diaz");
MODULE_DESCRIPTION("Ejericio 3, Practico 3, Taller de sistemas operativos, Facultad de Ingenieria, Universidad de la Republica.");
MODULE_VERSION("1.0");

//Estructura de datos del driver
int i;
static int    majorNumber;
static char   message[4096];
static short  head = 0;
static short  top= 0;

//Devices structs
static struct class*  tsofifoClass  = NULL;
static struct device* tsofifoDevice = NULL;

// Operaciones que define el driver
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

//fops
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int __init tsofifo_init(void){
   printk(KERN_INFO "TSOFIFO: Inicializando el TSOFIFO LKM\n");

   // Registar el major namber dinamicamente
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "TSOFIFO fallo al registar el major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "TSOFIFO: registrado correctamente con major number %d\n", majorNumber);

   //Registar el device class
   tsofifoClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(tsofifoClass)){                // Manejo de error
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Fallo al registar el device class\n");
      return PTR_ERR(tsofifoClass);
   }
   printk(KERN_INFO "TSOFIFO: device class registrado correctamente\n");

   // Registar el devices 
   for(i=0;i<8;i++)
   {
      tsofifoDevice = device_create(tsofifoClass, NULL, MKDEV(majorNumber, i), NULL, "%s%d", DEVICE_NAME, i);
      if (IS_ERR(tsofifoDevice)){               // Manejo de error
         class_destroy(tsofifoClass);
         unregister_chrdev(majorNumber, DEVICE_NAME);
         printk(KERN_ALERT "Fallo al crear el device %d\n", i);
         return PTR_ERR(tsofifoDevice);
      }
      printk(KERN_INFO "TSOFIFO: device driver %d creado correctamente\n", i);
   }
   return 0;
}

static void __exit tsofifo_exit(void){
   for(i=0;i<8;i++)
   {
      device_destroy(tsofifoClass, MKDEV(majorNumber, i));
   }     // eliminar el devices
   class_unregister(tsofifoClass);                          // desregistar el device class
   class_destroy(tsofifoClass);                             // eliminar el device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // desregitar el major number
   printk(KERN_INFO "TSOFIFO: Chau \n");
}

static int dev_open(struct inode *inodep, struct file *filep){
 
   printk(KERN_INFO "EBBChar: Device has been opened");
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, top);

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", top);
      return (top=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(message, "%s(%d letters)", buffer, len);   // appending received string with its length
   top = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "EBBChar: Received %d characters from the user\n", len);
   return len;
}

static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "EBBChar: Device successfully closed\n");
   return 0;
}

module_init(tsofifo_init);
module_exit(tsofifo_exit);