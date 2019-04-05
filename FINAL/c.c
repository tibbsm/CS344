#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
   pid_t spawnpid = fork();
   switch (spawnpid)
   {
      case -1: exit(1); break;
      case 0: exit(0); break;
      default:  break;
   }
   printf("XYZZY\n");
}