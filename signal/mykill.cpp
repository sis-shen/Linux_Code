#include <iostream>
#include<unistd.h>
#include <signal.h>
#include <string>
#include <sys/types.h>

using namespace std;

void Usage(string proc)
{
    cout<<"Usage:\n\t"<<proc<< " signum pid\n";
}

int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    int signum = stoi(argv[1]);
    pid_t pid = stoi(argv[2]);

    int n = kill(pid,signum);
    if(n == -1)
    {
        perror("kill");
        exit(2);
    }
    return 0;
}