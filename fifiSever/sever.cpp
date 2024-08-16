#include "comm.hpp"

using namespace std;

int main()
{
    //创建信道
    int n = mkfifo(FIFO_NAME,MODE);
    if(n == -1)//创建失败
    {
        perror("mkfifo");
        exit(FIFO_CREAT_ERR);
    }

    //打开信道
    int fd = open(FIFO_NAME,O_RDONLY);//只读模式打开FIFO
    if(fd == -1)//打开失败
    {
        perror("open");
        exit(FIFO_OPEN_ERR);
    }

    //开始通信
    cout<<"[@]sever start running"<<endl;
    while(true)
    {
        char buffer[1024] = {0};
        int sz = read(fd,buffer,sizeof(buffer)-1);//给结尾\0预留位置
        if(sz > 0)
        {
            buffer[sz] = 0;
            cout<<"client say# "<<buffer<<endl;
        }
        else if(sz == 0)
        {
            //写端关闭
            cout<<"client quit, sever will quie later..." <<endl;
            break;
        }
        else break;
    }

    //关闭信道
    close(fd);
    int m = unlink(FIFO_NAME);
    if(m == -1)
    {
        perror("unlink");
        exit(FIFO_DELETE_ERR);
    }

    return 0;
}