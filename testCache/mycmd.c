#include <stdio.h>
#include <unistd.h>

int main()
{
    fwrite("666",1,4,stdout);  
    fwrite("666",1,4,stderr);  
    _exit(0);
    return ;
}