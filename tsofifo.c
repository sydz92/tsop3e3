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
//dev1
static char   d1Buff[4096];
static short  d1Head = 0;
static short  d1Top= 0;
//dev2
static char   d2Buff[4096];
static short  d2Head = 0;
static short  d2Top= 0;
//dev3
static char   d3Buff[4096];
static short  d3Head = 0;
static short  d3Top= 0;
//dev4
static char   d4Buff[4096];
static short  d4Head = 0;
static short  d4Top= 0;

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

//INICIALIZACION DEL MODULO
static int __init tsofifo_init(void){
   printk(KERN_INFO "TSOFIFO: Inicializando el TSOFIFO LKM\n");

   // Registar el major namber dinamicamente
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_INFO "TSOFIFO fallo al registar el major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "TSOFIFO: registrado correctamente con major number %d\n", majorNumber);

   //Registar el device class
   tsofifoClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(tsofifoClass)){                // Manejo de error
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_INFO "Fallo al registar el device class\n");
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
         printk(KERN_INFO "Fallo al crear el device %d\n", i);
         return PTR_ERR(tsofifoDevice);
      }
      printk(KERN_INFO "TSOFIFO: device driver %d creado correctamente\n", i);
   }
   return 0;
}

//LIBERER RECURSOS
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

//OPEN
static int dev_open(struct inode *inodep, struct file *filep){
   
   int minor = MINOR(inodep->i_rdev);

   if (minor == 0)
   //tsofifo0
   {

      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
   } else if (minor == 1)
   //tsofifo1
   {

      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
   } else if (minor == 2)
   //tsofifo2
   {

      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
   } else if (minor == 3)
   //tsofifo3
   {

      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
   } else if (minor == 4)
   //tsofifo4
   {

      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
   } else if (minor == 5)
   //tsofifo5
   {

      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
   } else if (minor == 6)
   //tsofifo6
   {

      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
   } else if (minor == 7)
   //tsofifo7
   {

      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
   }

   return 0;
}

//READ
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
//CODIGO
   int error_count;
   int minor = MINOR(filep->f_inode->i_rdev);

   if (minor == 0)
   //tsofifo0
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo escritura\n", minor);
      return -EPERM;

   } else if (minor == 1)
   //tsofifo1
   {
      printk(KERN_INFO "TSOFIFO: read en dispositivo tsofifo%d", minor);
      
      //COPIAR AL USER SPACE
      error_count = 0;
      error_count = copy_to_user(buffer, d1Buff, d1Top);
      //MANEJO DE ERROR
      if (error_count==0)
      {//success
         printk(KERN_INFO "TSOFIFO: %d caracteres leidos\n", d1Top);
         return (d1Top=0);
      }
      else 
      {//error
         printk(KERN_INFO "TSOFIFO: Fallo al leer %d caracteres\n", error_count);
         return -EFAULT;
      }
   
   } else if (minor == 2)
   //tsofifo2
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo escritura\n", minor);
      return -EPERM;

   } else if (minor == 3)
   //tsofifo3
   {
      printk(KERN_INFO "TSOFIFO: read en dispositivo tsofifo%d", minor);

      //COPIAR AL USER SPACE
      error_count = 0;
      error_count = copy_to_user(buffer, d2Buff, d2Top);
      //MANEJO DE ERROR
      if (error_count==0)
      {//success
         printk(KERN_INFO "TSOFIFO: %d caracteres leidos\n", d2Top);
         return (d2Top=0);
      }
      else 
      {//error
         printk(KERN_INFO "TSOFIFO: Fallo al leer %d caracteres\n", error_count);
         return -EFAULT;
      }

   } else if (minor == 4)
   //tsofifo4
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo escritura\n", minor);
      return -EPERM;

   } else if (minor == 5)
   //tsofifo5
   {
      printk(KERN_INFO "TSOFIFO: read en dispositivo tsofifo%d", minor);

      //COPIAR AL USER SPACE
      error_count = 0;
      error_count = copy_to_user(buffer, d3Buff, d3Top);
      //MANEJO DE ERROR
      if (error_count==0)
      {//success
         printk(KERN_INFO "TSOFIFO: %d caracteres leidos\n", d3Top);
         return (d3Top=0);
      }
      else 
      {//error
         printk(KERN_INFO "TSOFIFO: Fallo al leer %d caracteres\n", error_count);
         return -EFAULT;
      }

   } else if (minor == 6)
   //tsofifo6
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo escritura\n", minor);
      return -EPERM;

   } else if (minor == 7)
   //tsofifo7
   {
      printk(KERN_INFO "TSOFIFO: read en dispositivo tsofifo%d", minor);

      //COPIAR AL USER SPACE
      error_count = 0;
      error_count = copy_to_user(buffer, d4Buff, d4Top);
      //MANEJO DE ERROR
      if (error_count==0)
      {//success
         printk(KERN_INFO "TSOFIFO: %d caracteres leidos\n", d4Top);
         return (d4Top=0);
      }
      else 
      {//error
         printk(KERN_INFO "TSOFIFO: Fallo al leer %d caracteres\n", error_count);
         return -EFAULT;
      }

   }

   return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

//CODIGO
   int minor = MINOR(filep->f_inode->i_rdev);

   if (minor == 0)
   //tsofifo0
   {
      printk(KERN_INFO "TSOFIFO: write en dispositivo tsofifo%d", minor);

      //COPIAR AL BUFFER
      sprintf(d1Buff, "%s", buffer);  
      d1Top = strlen(d1Buff);                 
      printk(KERN_INFO "TSOFIFO: %d caracteers recividos\n", len);
      return len;

   } else if (minor == 1)
   //tsofifo1
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo lectura\n", minor);
      return -EPERM;
   
   } else if (minor == 2)
   //tsofifo2
   {
      printk(KERN_INFO "TSOFIFO: write en dispositivo tsofifo%d", minor);

      //COPIAR AL BUFFER
      sprintf(d2Buff, "%s", buffer);  
      d2Top = strlen(d2Buff);                 
      printk(KERN_INFO "TSOFIFO: %d caracteers recividos\n", len);
      return len;

   } else if (minor == 3)
   //tsofifo3
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo lectura\n", minor);
      return -EPERM;

   } else if (minor == 4)
   //tsofifo4
   {
      printk(KERN_INFO "TSOFIFO: write en dispositivo tsofifo%d", minor);

      //COPIAR AL BUFFER
      sprintf(d3Buff, "%s", buffer);  
      d3Top = strlen(d3Buff);                 
      printk(KERN_INFO "TSOFIFO: %d caracteers recividos\n", len);
      return len;

   } else if (minor == 5)
   //tsofifo5
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo lectura\n", minor);
      return -EPERM;

   } else if (minor == 6)
   //tsofifo6
   {
      printk(KERN_INFO "TSOFIFO: write en dispositivo tsofifo%d", minor);

      //COPIAR AL BUFFER
      sprintf(d4Buff, "%s", buffer);  
      d4Top = strlen(d4Buff);                 
      printk(KERN_INFO "TSOFIFO: %d caracteers recividos\n", len);
      return len;

   } else if (minor == 7)
   //tsofifo7
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo lectura\n", minor);
      return -EPERM;

   }

   return 0;
}

// RELEASE
static int dev_release(struct inode *inodep, struct file *filep){
   
   int minor = MINOR(inodep->i_rdev);
   
   if (minor == 0)
   //tsofifo0
   {

      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d", minor);
   } else if (minor == 1)
   //tsofifo1
   {

      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d", minor);
   } else if (minor == 2)
   //tsofifo2
   {

      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d", minor);
   } else if (minor == 3)
   //tsofifo3
   {

      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d", minor);
   } else if (minor == 4)
   //tsofifo4
   {

      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d", minor);
   } else if (minor == 5)
   //tsofifo5
   {

      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d", minor);
   } else if (minor == 6)
   //tsofifo6
   {

      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d", minor);
   } else if (minor == 7)
   //tsofifo7
   {

      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d", minor);
   }
   
   return 0;
}

module_init(tsofifo_init);
module_exit(tsofifo_exit);