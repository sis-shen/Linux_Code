#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// int main()
// {
//     int status;
//     pid_t id = fork();
//     if(id == 0)
//     {
//         for(int i = 1;i<=5;i++)
//         {
//             printf("child running [%d]s\n",i);
//             sleep(1);
//         }
//         printf("child exited\n");
//         exit(0);
//     }
//     else
//     {
//         waitpid(id,&status,0);
//         if(WIFEXITED(status))
//             printf("father wait success, exit code: %d\n",WEXITSTATUS(status));
//         else
//             printf("child failed to exit\n");
//     }
//     return 1;
// }

//非阻塞轮询
int main()
{
    int status;
    pid_t id = fork();
    if(id == 0)
    {
        sleep(5);
        printf("child exited\n");
        exit(0);
    }
    else
    {
        while(1)//轮询
        {
            pid_t rid = waitpid(id,&status,WNOHANG);
            if(rid > 0)
            {
                if(WIFEXITED(status))
                    printf("father wait success, exit code: %d\n",WEXITSTATUS(status));
                else
                    printf("child failed to exit\n");

                break;
            }
            else if(rid == 0)
            {
                printf("等待子进程中...\n");
                sleep(1);
            }
            else//rid<0
            {
                printf("wait failed\n");
                break;
            }
        }
    }
    return 1;
}