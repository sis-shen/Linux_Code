#include "comm.hpp"

using namespace std;

int main()
{
    int fd = open(FIFO_NAME,O_WRONLY);//只读模式打开
    if(fd <0)
    {
        perror("open");
        exit(FIFO_OPEN_ERR);
    }

    string line;
    while(true)
    {
        cout<<"Please enter@ ";
        getline(cin,line);//获取输入

        write(fd,line.c_str(),line.size());
    }

    close(fd);
    return 0;
}
