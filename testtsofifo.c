#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 4096               ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

int main(){

   printf("Comienza el test...\n");
   int ret, fd;
   char * dev;
   char stringToSend[BUFFER_LENGTH];

   int i = 0;
   while(i<8)
   {
      
      printf("Abrendo dispositvo %d...\n", i);      
      sprintf (dev, "/dev/tsofifo%d", i);
      fd = open(dev, O_RDWR);           
      if (fd < 0)\
      {
         perror("Fallo a abrir el dispositvo...");
         return errno;
      }
      i++;

      printf("Escriba el mensaje para enviar al dispositvo:\n");
      scanf("%[^\n]%*c", stringToSend);                // Read in a string (with spaces)
      printf("Envidndo el mensaje [%s] al dispositvo.\n", stringToSend);
      ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
      if (ret < 0){
         perror("Fallo al enviar el mensaje.");
         return errno;
      }

      printf("Abrendo dispositvo %d...\n", i);      
      sprintf (dev, "/dev/tsofifo%d", i);
      fd = open(dev, O_RDWR);           
      if (fd < 0)\
      {
         perror("Fallo a abrir el dispositvo...");
         return errno;
      }
      i++;

      printf("Presionar ENTER parra leer el dispositvo...");
      getchar();

      printf("Leyendo el dispositvo...\n");
      ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
      if (ret < 0){
         perror("Fallo al leer el dispositvo.");
         return errno;
      }
      printf("El mensaje recibido es: [%s]\n\n\n", receive);
      
   }
   printf("Hasta pronto!\n");
   return 0;
}
