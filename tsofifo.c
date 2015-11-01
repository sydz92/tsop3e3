#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>

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
static unsigned int majorNumber;
//dev1
static char   d1Buff[4096];
static short  d1Len= 0;
static short  tsofifo0= 0;
static short  tsofifo1= 0;
static struct semaphore d1SemBuff;
static struct semaphore d1SemBuffEmpty;
static struct semaphore d1SemBuffFull;
static struct semaphore d1SemTSOs;
//dev2
static char   d2Buff[4096];
static short  d2Len= 0;
static short  tsofifo2= 0;
static short  tsofifo3= 0;
static struct semaphore d2SemBuff;
static struct semaphore d2SemBuffEmpty;
static struct semaphore d2SemBuffFull;
static struct semaphore d2SemTSOs;
//dev3
static char   d3Buff[4096];
static short  d3Len= 0;
static short  tsofifo4= 0;
static short  tsofifo5= 0;
static struct semaphore d3SemBuff;
static struct semaphore d3SemBuffEmpty;
static struct semaphore d3SemBuffFull;
static struct semaphore d3SemTSOs;
//dev4
static char   d4Buff[4096];
static short  d4Len= 0;
static short  tsofifo6= 0;
static short  tsofifo7= 0;
static struct semaphore d4SemBuff;
static struct semaphore d4SemBuffEmpty;
static struct semaphore d4SemBuffFull;
static struct semaphore d4SemTSOs;

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

module_param_named(major, majorNumber, int, 0);

//INICIALIZACION DEL MODULO
static int __init tsofifo_init(void){
   printk(KERN_INFO "TSOFIFO: Inicializando el TSOFIFO LKM\n");

   //init de semaforos
   sema_init(&d1SemBuff, 1);
   sema_init(&d1SemBuffEmpty, 0);
   sema_init(&d1SemBuffFull, 0);
   sema_init(&d1SemTSOs, 1);

   sema_init(&d2SemBuff, 1);
   sema_init(&d2SemBuffEmpty, 0);
   sema_init(&d2SemBuffFull, 0);
   sema_init(&d2SemTSOs, 1);

   sema_init(&d3SemBuff, 1);
   sema_init(&d3SemBuffEmpty, 0);
   sema_init(&d3SemBuffFull, 0);
   sema_init(&d3SemTSOs, 1);
   
   sema_init(&d4SemBuff, 1);
   sema_init(&d4SemBuffEmpty, 0);
   sema_init(&d4SemBuffFull, 0);
   sema_init(&d4SemTSOs, 1);

   if (majorNumber != 0)
   {
      printk(KERN_INFO "TSOFIFO major number pasado como parametro %d\n", majorNumber);

      // Registar el major namber dinamicamente
      i = register_chrdev(majorNumber, DEVICE_NAME, &fops);
      if (i<0){
         printk(KERN_INFO "TSOFIFO fallo al registar el major number\n");
         return majorNumber;
      }
      printk(KERN_INFO "TSOFIFO: registrado correctamente con major number %d\n", majorNumber);
   }
   else
   {
      printk(KERN_INFO "TSOFIFO major number asignado dinamicamente");

      // Registar el major namber dinamicamente
      majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
      if (majorNumber<0){
         printk(KERN_INFO "TSOFIFO fallo al registar el major number\n");
         return majorNumber;
      }
      printk(KERN_INFO "TSOFIFO: registrado correctamente con major number %d\n", majorNumber);
   }

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
      if (down_interruptible(&d1SemTSOs))
        return -ERESTARTSYS;   
      if (tsofifo0 == 0)
      {
         printk(KERN_INFO "TSOFIFO%d: abierto correctamente", minor);
         tsofifo0 = 1;
      }
      else
      {
         printk(KERN_INFO "TSOFIFO%d: dispositivo en uso", minor);
         up(&d1SemTSOs);
         return -1;
      }
      up(&d1SemTSOs);

   } else if (minor == 1)
   //tsofifo1
   {
      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
      if (down_interruptible(&d1SemTSOs))
        return -ERESTARTSYS; 
      if (tsofifo1 == 0)
      {
         printk(KERN_INFO "TSOFIFO%d: abierto correctamente", minor);
         tsofifo1 = 1;
      }
      else
      {
         printk(KERN_INFO "TSOFIFO%d: dispositivo en uso", minor);
         up(&d1SemTSOs);
         return -1;
      }
      up(&d1SemTSOs);

   } else if (minor == 2)
   //tsofifo2
   {
      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
      if (down_interruptible(&d2SemTSOs))
        return -ERESTARTSYS; 
      if (tsofifo2 == 0)
      {
         printk(KERN_INFO "TSOFIFO%d: abierto correctamente", minor);
         tsofifo2 = 1;
      }
      else
      {
         printk(KERN_INFO "TSOFIFO%d: dispositivo en uso", minor);
         up(&d2SemTSOs);
         return -1;
      }
      up(&d2SemTSOs);

   } else if (minor == 3)
   //tsofifo3
   {
      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
      if (down_interruptible(&d2SemTSOs))
        return -ERESTARTSYS;
      if (tsofifo3 == 0)
      {
         printk(KERN_INFO "TSOFIFO%d: abierto correctamente", minor);
         tsofifo3 = 1;
      }
      else
      {
         printk(KERN_INFO "TSOFIFO%d: dispositivo en uso", minor);
         up(&d2SemTSOs);
         return -1;
      }
      up(&d2SemTSOs);

   } else if (minor == 4)
   //tsofifo4
   {
      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
      if (down_interruptible(&d3SemTSOs))
        return -ERESTARTSYS;
      if (tsofifo4 == 0)
      {
         printk(KERN_INFO "TSOFIFO%d: abierto correctamente", minor);
         tsofifo4 = 1;
      }
      else
      {
         printk(KERN_INFO "TSOFIFO%d: dispositivo en uso", minor);
         up(&d3SemTSOs);
         return -1;
      }
      up(&d3SemTSOs);

   } else if (minor == 5)
   //tsofifo5
   {
      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
      if (down_interruptible(&d3SemTSOs))
        return -ERESTARTSYS;
      if (tsofifo5 == 0)
      {
         printk(KERN_INFO "TSOFIFO%d: abierto correctamente", minor);
         tsofifo5 = 1;
      }
      else
      {
         printk(KERN_INFO "TSOFIFO%d: dispositivo en uso", minor);
         up(&d3SemTSOs);
         return -1;
      }
      up(&d3SemTSOs);

   } else if (minor == 6)
   //tsofifo6
   {
      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
      if (down_interruptible(&d4SemTSOs))
        return -ERESTARTSYS;
      if (tsofifo6 == 0)
      {
         printk(KERN_INFO "TSOFIFO%d: abierto correctamente", minor);
         tsofifo6 = 1;
      }
      else
      {
         printk(KERN_INFO "TSOFIFO%d: dispositivo en uso", minor);
         up(&d4SemTSOs);
         return -1;
      }
      up(&d4SemTSOs);

   } else if (minor == 7)
   //tsofifo7
   {
      printk(KERN_INFO "TSOFIFO: open en dispositivo tsofifo%d", minor);
      if (down_interruptible(&d4SemTSOs))
        return -ERESTARTSYS;
      if (tsofifo7 == 0)
      {
         printk(KERN_INFO "TSOFIFO%d: abierto correctamente", minor);
         tsofifo7 = 1;
      }
      else
      {
         printk(KERN_INFO "TSOFIFO%d: dispositivo en uso", minor);
         up(&d4SemTSOs);
         return -1;
      }
      up(&d4SemTSOs);
   }

   return 0;
}

//READ
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
//CODIGO
   int error_count;
   int to_read;
   int minor = MINOR(filep->f_inode->i_rdev);

   if (minor == 0)
   //tsofifo0
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo escritura\n", minor);
      return -EPERM;

   } else if (minor == 1)
   //tsofifo1
   {
      printk(KERN_INFO "TSOFIFO%d: read en dispositivo", minor);
      
      if (down_interruptible(&d1SemBuff))
        return -ERESTARTSYS;
      while(1){
         if (d1Len > 0)
         {//Hay cosas en el buffer
            if (len > d1Len)
            {//leer todo el buffer
               to_read = d1Len;
            }
            else
            {//leer len caraceres
               to_read = len;
            }
            //COPIAR AL USER SPACE
            error_count = 0;
            error_count = copy_to_user(buffer, d1Buff, to_read);
            //MANEJO DE ERROR
            if (error_count==0)
            {//success
               //despertar al lector si el buffer esta lleno
               if (d1Len == 4069)
                  up(&d1SemBuffFull);
               //reacomoadar buffer
               d1Len -= to_read;
               for (i=0;i < d1Len;i++)
               {
                  d1Buff[i] = d1Buff[to_read+i];
               }

               printk(KERN_INFO "TSOFIFO%d: %d caracteres leidos\n", to_read, minor);
               up(&d1SemBuff);
               return (to_read);
            }
            else 
            {//error
               printk(KERN_INFO "TSOFIFO%d: Fallo al leer %d caracteres\n", error_count, minor);
               up(&d1SemBuff);
               return -EFAULT;
            }
         }
         else
         {//Buffer vacio
            printk(KERN_INFO "TSOFIFO%d: buffer vacio", minor);
            up(&d1SemBuff);

            if (down_interruptible(&d1SemTSOs))
               return -ERESTARTSYS;
            if (tsofifo0 == 0){
               up(&d1SemTSOs);
               return 0;
            }
            else
            {
               up(&d1SemTSOs);
               if (down_interruptible(&d1SemBuffEmpty))
                  return -ERESTARTSYS;
               if (down_interruptible(&d1SemBuff))
                  return -ERESTARTSYS;
            }
         }
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

      if (down_interruptible(&d2SemBuff))
                  return -ERESTARTSYS;
      while(1){
         if (d2Len > 0)
         {//Hay cosas en el buffer
            if (len > d2Len)
            {//leer todo el buffer
               to_read = d2Len;
            }
            else
            {//leer len caraceres
               to_read = len;
            }
            //COPIAR AL USER SPACE
            error_count = 0;
            error_count = copy_to_user(buffer, d2Buff, to_read);
            //MANEJO DE ERROR
            if (error_count==0)
            {//success
               //despertar al lector si el buffer esta lleno
               if (d2Len == 4069)
                  up(&d2SemBuffFull);
               //reacomoadar buffer
               d2Len -= to_read;
               for (i=0;i < d2Len;i++)
               {
                  d2Buff[i] = d2Buff[to_read+i];
               }

               printk(KERN_INFO "TSOFIFO%d: %d caracteres leidos\n", to_read, minor);
               up(&d2SemBuff);
               return (to_read);
            }
            else 
            {//error
               printk(KERN_INFO "TSOFIFO%d: Fallo al leer %d caracteres\n", error_count, minor);
               up(&d2SemBuff);
               return -EFAULT;
            }
         }
         else
         {//Buffer vacio
            printk(KERN_INFO "TSOFIFO%d: buffer vacio", minor);
            up(&d2SemBuff);

            if (down_interruptible(&d2SemTSOs))
                  return -ERESTARTSYS;
            if (tsofifo2 == 0){
               up(&d2SemTSOs);
               return 0;
            }
            else
            {
               up(&d2SemTSOs);
               if (down_interruptible(&d2SemBuffEmpty))
                  return -ERESTARTSYS;
               if (down_interruptible(&d2SemBuff))
                  return -ERESTARTSYS;
            }
         }
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

      if (down_interruptible(&d3SemBuff))
         return -ERESTARTSYS;
      while(1){
         if (d3Len > 0)
         {//Hay cosas en el buffer
            if (len > d3Len)
            {//leer todo el buffer
               to_read = d3Len;
            }
            else
            {//leer len caraceres
               to_read = len;
            }
            //COPIAR AL USER SPACE
            error_count = 0;
            error_count = copy_to_user(buffer, d3Buff, to_read);
            //MANEJO DE ERROR
            if (error_count==0)
            {//success
               //despertar al lector si el buffer esta lleno
               if (d3Len == 4069)
                  up(&d3SemBuffFull);
               //reacomoadar buffer
               d3Len -= to_read;
               for (i=0;i < d3Len;i++)
               {
                  d3Buff[i] = d3Buff[to_read+i];
               }

               printk(KERN_INFO "TSOFIFO%d: %d caracteres leidos\n", to_read, minor);
               up(&d3SemBuff);
               return (to_read);
            }
            else 
            {//error
               printk(KERN_INFO "TSOFIFO%d: Fallo al leer %d caracteres\n", error_count, minor);
               up(&d3SemBuff);
               return -EFAULT;
            }
         }
         else
         {//Buffer vacio
            printk(KERN_INFO "TSOFIFO%d: buffer vacio", minor);
            up(&d3SemBuff);

            if (down_interruptible(&d3SemTSOs))
               return -ERESTARTSYS;
            if (tsofifo4 == 0){
               up(&d3SemTSOs);
               return 0;
            }
            else
            {
               up(&d3SemTSOs);
               if (down_interruptible(&d3SemBuffEmpty))
                  return -ERESTARTSYS;
               if (down_interruptible(&d3SemBuff))
                  return -ERESTARTSYS;
            }
         }
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

      if (down_interruptible(&d4SemBuff))
         return -ERESTARTSYS;
      while(1){
         if (d4Len > 0)
         {//Hay cosas en el buffer
            if (len > d4Len)
            {//leer todo el buffer
               to_read = d4Len;
            }
            else
            {//leer len caraceres
               to_read = len;
            }
            //COPIAR AL USER SPACE
            error_count = 0;
            error_count = copy_to_user(buffer, d4Buff, to_read);
            //MANEJO DE ERROR
            if (error_count==0)
            {//success
               //despertar al lector si el buffer esta lleno
               if (d4Len == 4069)
                  up(&d4SemBuffFull);
               //reacomoadar buffer
               d4Len -= to_read;
               for (i=0;i < d4Len;i++)
               {
                  d4Buff[i] = d4Buff[to_read+i];
               }

               printk(KERN_INFO "TSOFIFO%d: %d caracteres leidos\n", to_read, minor);
               up(&d4SemBuff);
               return (to_read);
            }
            else 
            {//error
               printk(KERN_INFO "TSOFIFO%d: Fallo al leer %d caracteres\n", error_count, minor);
               up(&d4SemBuff);
               return -EFAULT;
            }
         }
         else
         {//Buffer vacio
            printk(KERN_INFO "TSOFIFO%d: buffer vacio", minor);
            up(&d4SemBuff);

            if (down_interruptible(&d4SemTSOs))
               return -ERESTARTSYS;
            if (tsofifo4 == 0){
               up(&d4SemTSOs);
               return 0;
            }
            else
            {
               up(&d4SemTSOs);
               if (down_interruptible(&d4SemBuffEmpty))
                  return -ERESTARTSYS;
               if (down_interruptible(&d4SemBuff))
                  return -ERESTARTSYS;
            }
         }
      }

   }

   return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

//CODIGO
   int to_write;
   int minor = MINOR(filep->f_inode->i_rdev);

   if (minor == 0)
   //tsofifo0
   {
      printk(KERN_INFO "TSOFIFO%d: write en dispositivo\n", minor);

      if (down_interruptible(&d1SemBuff))
         return -ERESTARTSYS;
      while(1){
         if (d1Len < 4096)
         {//hay espacio en el buffer 
            if (len> (4096 - d1Len))
            {//escribir lo que queda de baffer
               to_write = 4096 - d1Len;
            }
            else
            {//esribir todo
               to_write = len;
            }
            //COPIAR AL BUFFER
            for (i = 0;i < to_write;i++)
            {
               d1Buff[i+d1Len] = buffer[i];
            }
            //despertar al escritor si el buffer estaba vacio
            if (d1Len == 0)
               up(&d1SemBuffEmpty);
            d1Len+= to_write;

            printk(KERN_INFO "TSOFIFO%d: %d caracteres recibidos\n", minor, to_write);
            up(&d1SemBuff);
            return to_write;
         }
         else 
         {//el buffer esta lleno
            printk(KERN_INFO "TSOFIFO%d: buffer lleno\n", minor);
            up(&d1SemBuff);
            if (down_interruptible(&d1SemBuffFull))
               return -ERESTARTSYS;
            if (down_interruptible(&d1SemBuff))
               return -ERESTARTSYS;
         }
      }

   } else if (minor == 1)
   //tsofifo1
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo lectura\n", minor);
      return -EPERM;
   
   } else if (minor == 2)
   //tsofifo2
   {
      printk(KERN_INFO "TSOFIFO: write en dispositivo tsofifo%d\n", minor);

      if (down_interruptible(&d2SemBuff))
         return -ERESTARTSYS;
      while(1){
         if (d2Len < 4096)
         {//hay espacio en el buffer 
            if (len> (4096 - d2Len))
            {//escribir lo que queda de baffer
               to_write = 4096 - d2Len;
            }
            else
            {//esribir todo
               to_write = len;
            }
            //COPIAR AL BUFFER
            for (i = 0;i < to_write;i++)
            {
               d2Buff[i+d2Len] = buffer[i];
            }
            //despertar al escritor si el buffer estaba vacio
            if (d2Len == 0)
               up(&d2SemBuffEmpty);
            d2Len+= to_write;

            printk(KERN_INFO "TSOFIFO%d: %d caracteres recibidos\n", minor, to_write);
            up(&d2SemBuff);
            return to_write;
         }
         else 
         {//el buffer esta lleno
            printk(KERN_INFO "TSOFIFO%d: buffer lleno\n", minor);
            up(&d2SemBuff);
            if (down_interruptible(&d2SemBuffFull))
               return -ERESTARTSYS;
            if (down_interruptible(&d2SemBuff))
               return -ERESTARTSYS;
         }
      }

   } else if (minor == 3)
   //tsofifo3
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo lectura\n", minor);
      return -EPERM;

   } else if (minor == 4)
   //tsofifo4
   {
      printk(KERN_INFO "TSOFIFO: write en dispositivo tsofifo%d\n", minor);

      if (down_interruptible(&d3SemBuff))
         return -ERESTARTSYS;
      while(1){
         if (d3Len < 4096)
         {//hay espacio en el buffer 
            if (len> (4096 - d3Len))
            {//escribir lo que queda de baffer
               to_write = 4096 - d3Len;
            }
            else
            {//esribir todo
               to_write = len;
            }
            //COPIAR AL BUFFER
            for (i = 0;i < to_write;i++)
            {
               d3Buff[i+d3Len] = buffer[i];
            }
            //despertar al escritor si el buffer estaba vacio
            if (d3Len == 0)
               up(&d3SemBuffEmpty);
            d3Len+= to_write;

            printk(KERN_INFO "TSOFIFO%d: %d caracteres recibidos\n", minor, to_write);
            up(&d3SemBuff);
            return to_write;
         }
         else 
         {//el buffer esta lleno
            printk(KERN_INFO "TSOFIFO%d: buffer lleno\n", minor);
            up(&d3SemBuff);
            if (down_interruptible(&d3SemBuffFull))
               return -ERESTARTSYS;
            if (down_interruptible(&d3SemBuff))
               return -ERESTARTSYS;
         }
      }

   } else if (minor == 5)
   //tsofifo5
   {
      printk(KERN_INFO "El dispositivo tsofifo%d es de solo lectura\n", minor);
      return -EPERM;

   } else if (minor == 6)
   //tsofifo6
   {
      printk(KERN_INFO "TSOFIFO: write en dispositivo tsofifo%d\n", minor);

      if (down_interruptible(&d4SemBuff))
         return -ERESTARTSYS;
      while(1){
         if (d4Len < 4096)
         {//hay espacio en el buffer 
            if (len> (4096 - d4Len))
            {//escribir lo que queda de baffer
               to_write = 4096 - d4Len;
            }
            else
            {//esribir todo
               to_write = len;
            }
            //COPIAR AL BUFFER
            for (i = 0;i < to_write;i++)
            {
               d4Buff[i+d4Len] = buffer[i];
            }
            //despertar al escritor si el buffer estaba vacio
            if (d3Len == 0)
               up(&d3SemBuffEmpty);
            d4Len+= to_write;

            printk(KERN_INFO "TSOFIFO%d: %d caracteres recibidos\n", minor, to_write);
            up(&d4SemBuff);
            return to_write;
         }
         else 
         {//el buffer esta lleno
            printk(KERN_INFO "TSOFIFO%d: buffer lleno\n", minor);
            up(&d4SemBuff);
            if (down_interruptible(&d4SemBuffFull))
               return -ERESTARTSYS;
            if (down_interruptible(&d4SemBuff))
               return -ERESTARTSYS;
         }
      }

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
      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d\n", minor);
      
      if (down_interruptible(&d1SemTSOs))
         return -ERESTARTSYS;
      tsofifo0 = 0;
      if (tsofifo1 == 0)
      {
         d1Len = 0;
      }
      up(&d1SemTSOs);
      //Despierto al lector si esta dormido
      up(&d1SemBuffEmpty);

      printk(KERN_INFO "TSOFIFO%d: cerrado correctamente\n", minor);

   } else if (minor == 1)
   //tsofifo1
   {
      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d\n", minor);
      
      if (down_interruptible(&d1SemTSOs))
         return -ERESTARTSYS;
      tsofifo1 = 0;
      if (tsofifo0 == 0)
      {
         d1Len = 0;
      }
      up(&d1SemTSOs);

      printk(KERN_INFO "TSOFIFO%d: cerrado correctamente\n", minor);

   } else if (minor == 2)
   //tsofifo2
   {
      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d\n", minor);
      
      if (down_interruptible(&d2SemTSOs))
         return -ERESTARTSYS;
      tsofifo2 = 0;
      if (tsofifo3 == 0)
      {
         d2Len = 0;
      }
      up(&d2SemTSOs);
      //Despierto al lector si esta dormido
      up(&d2SemBuffEmpty);

      printk(KERN_INFO "TSOFIFO%d: cerrado correctamente\n", minor);

   } else if (minor == 3)
   //tsofifo3
   {
      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d\n", minor);
      
      if (down_interruptible(&d2SemTSOs))
         return -ERESTARTSYS;
      tsofifo3 = 0;
      if (tsofifo2 == 0)
      {
         d2Len = 0;
      }
      up(&d2SemTSOs);

      printk(KERN_INFO "TSOFIFO%d: cerrado correctamente\n", minor);

   } else if (minor == 4)
   //tsofifo4
   {
      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d\n", minor);
      
      if (down_interruptible(&d3SemTSOs))
         return -ERESTARTSYS;
      tsofifo4 = 0;
      if (tsofifo5 == 0)
      {
         d3Len = 0;
      }
      up(&d3SemTSOs);
      //Despierto al lector si esta dormido
      up(&d3SemBuffEmpty);

      printk(KERN_INFO "TSOFIFO%d: cerrado correctamente\n", minor);

   } else if (minor == 5)
   //tsofifo5
   {
      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d\n", minor);
      
      if (down_interruptible(&d3SemTSOs))
         return -ERESTARTSYS;
      tsofifo5 = 0;
      if (tsofifo4 == 0)
      {
         d3Len = 0;
      }
      up(&d3SemTSOs);

      printk(KERN_INFO "TSOFIFO%d: cerrado correctamente\n", minor);

   } else if (minor == 6)
   //tsofifo6
   {
      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d\n", minor);
      
      if (down_interruptible(&d4SemTSOs))
         return -ERESTARTSYS;
      tsofifo6 = 0;
      if (tsofifo7 == 0)
      {
         d4Len = 0;
      }
      up(&d4SemTSOs);
      //Despierto al lector si esta dormido
      up(&d4SemBuffEmpty);

      printk(KERN_INFO "TSOFIFO%d: cerrado correctamente\n", minor);

   } else if (minor == 7)
   //tsofifo7
   {
      printk(KERN_INFO "TSOFIFO: release en dispositivo tsofifo%d\n", minor);
      
      if (down_interruptible(&d4SemTSOs))
         return -ERESTARTSYS;
      tsofifo7 = 0;
      if (tsofifo6 == 0)
      {
         d4Len = 0;
      }
      up(&d4SemTSOs);

      printk(KERN_INFO "TSOFIFO%d: cerrado correctamente\n", minor);
   }
   
   return 0;
}

module_init(tsofifo_init);
module_exit(tsofifo_exit);  