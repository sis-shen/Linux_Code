#pragma once

#include <iostream>
#include <memory>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

const std::string SEM_PATH="/tmp";
const int SEM_PROJ_ID=0x0721;
const int default_num = 1;

#define GET_SEM (IPC_CREAT)
#define BUILD_SEM (IPC_CREAT | IPC_EXCL | 0666)

std::string intToHexStr(int num)
{
    char hex[64] = {0};
    snprintf(hex,sizeof(hex),"0x%x",num);
    return std::string(hex);
}

class Semaphores
{
public:
    Semaphores(int semid):_semid(semid)
    {

    }

    ~Semaphores()
    {
        semctl(_semid,0,IPC_RMID);
    }
    int getId()
    {
        return _semid;
    }
    void P()
    {
        _PV(-1);
    }
    void V()
    {
        _PV(1);
    }
private:
    void _PV(int data)
    {
        struct sembuf sem_buf;
        sem_buf.sem_num = 0;//index
        sem_buf.sem_op = data;
        sem_buf.sem_flg = SEM_UNDO;
        int n = semop(_semid,&sem_buf,1);
        if(n < 0)
        {
            std::cerr<<"semop pv:"<<data<<  "failed"<<std::endl;
        }
    }
private:
    int _semid;
};

class SemaphoreBuilder
{

public:
    SemaphoreBuilder()
    {

    }
    ~SemaphoreBuilder()
    {

    }

    SemaphoreBuilder& setVal(int val)
    {
        _val = val;
        return *this;
    }

    bool init(int smid,int index,int val)
    {
        union semun {
            int              val;    /* Value for SETVAL */
            struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
            unsigned short  *array;  /* Array for GETALL, SETALL */
            struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                        (Linux-specific) */
        }un;
        un.val = 1;
        int n = semctl(smid,index,SETVAL,un);
        if(n < 0)
        {
            std::cerr<<"初始化失败"<<std::endl;
            return false;
        }
        return true;
    }

    std::shared_ptr<Semaphores> build(int flag,int num = default_num)
    {
        //构建键值
        std::cout<<"build a Semphore"<<std::endl;
        key_t k = ftok(SEM_PATH.c_str(),SEM_PROJ_ID);
        if(k<0)
        {
            std::cerr<<"ftok failed"<<std::endl;
            return nullptr;
        }
        std::cout<<"Got key" << intToHexStr(k)<<std::endl;
        //获取信号量集合
        int smid = semget(k,num, flag);
        if(smid < 0)
        {
            std::cerr<< "semget failed\n";
        }
        std::cout<<"get smid: "<<smid<<std::endl;
        if(flag == BUILD_SEM)
            init(smid,0,_val);
        return std::make_shared<Semaphores>(smid);
    }

private:
    int _val = 1;
};