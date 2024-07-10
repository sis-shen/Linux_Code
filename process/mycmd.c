#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

// int main()
// {
//   pid_t id = fork();
//   if(id == 0)//子进程
//   {
//     sleep(30);
//     printf("child exit\n");
//   }
//   else//父进程
//   {
//     sleep(1);
//     printf("father exit\n");
//   }
//   return 0;
// }

int main()
{
  while(1)
  {
    printf("mypid: %d\n",getpid());
    sleep(1);
  }
  return 0;
}