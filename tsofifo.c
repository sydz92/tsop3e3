#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

// Nombre y clase de dispositivo
#define  DEVICE_NAME "tsofifo"
#define  CLASS_NAME  "tso"

//Informacion del Driver
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santiago Diaz");
MODULE_DESCRIPTION("Ejericio 3, Practico 3, Taller de sistemas operativos, Facultad de Ingenieria, Universidad de la Republica.");
MODULE_VERSION("1.0");

//Estructura de datos del driver
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

   // Registar el device driver
   tsofifoDevice = device_create(tsofifoClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(tsofifoDevice)){               // Manejo de error
      class_destroy(tsofifoClass);
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Fallo al crear el device\n");
      return PTR_ERR(tsofifoDevice);
   }
   printk(KERN_INFO "TSOFIFO: device driver creado correctamente\n");
   return 0;
}

static void __exit ebbchar_exit(void){
   device_destroy(tsofifoClass, MKDEV(majorNumber, 0));     // eliminar el device
   class_unregister(tsofifoClass);                          // desregistar el device class
   class_destroy(tsofifoClass);                             // eliminar el device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // desregitar el major number
   printk(KERN_INFO "TSOFIFO: Chau \n");
}

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "EBBChar: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(message, "%s(%d letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "EBBChar: Received %d characters from the user\n", len);
   return len;
}

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "EBBChar: Device successfully closed\n");
   return 0;
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(ebbchar_init);
module_exit(ebbchar_exit);