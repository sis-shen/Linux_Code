#include "Sem.hpp"
#include <unistd.h>
#include <wait.h>
int main()
{
    SemaphoreBuilder sb;
    auto sem = sb.setVal(1).build(BUILD_SEM,1);

    pid_t pid = fork();
    if(pid == 0)
    {
        while(true)
        {
            printf("C\n");
            usleep(150);
            printf("C\n");
            usleep(150);

        }
    }
    else 
    {
        while(true)
        {
            printf("C\n");
            usleep(150);
            printf("C\n");
            usleep(150);
        }
        waitpid(pid,nullptr,0);
    }

    return 0;
}