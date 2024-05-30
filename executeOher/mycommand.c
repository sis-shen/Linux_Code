#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

const char* exe = "ohterEXE";

int main()
{
  pid_t id = fork();
  if(id == 0)//child
  {
    printf("before: I am a process,pid: %d,ppid:%d\n",getpid(),getppid());
    
    execl("./otherEXE",exe,NULL);

    printf("after: I am a process,pid: %d,ppid:%d\n",getpid(),getppid());
    exit(1);
  }
  else//father 
  {
    pid_t ret = waitpid(id,NULL,0);
    if(ret>0)printf("wait success,pid:%d,ppid:%d\n",getpid(),getppid());
  }

  return 0;
}
