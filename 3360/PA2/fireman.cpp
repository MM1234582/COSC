#include <unistd.h>
#include <iostream>
#include <sys/wait.h>

void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0)
      ;
}

int main()
{

   signal(SIGCHLD, fireman);
   int nChild = 0;
   while (true)
   {

      if (fork() == 0)
      {
         sleep(5);
         _exit(0);
      }
      std::cin.get();
      /*   nChild ++;
         if (nChild == 10)
         {
            break;
         }*/
   }

   return 0;
}