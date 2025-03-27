#pragma once

#include <iostream>
#include <memory>
#include <vector>
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

//产品类
class Semaphore
{
public:
    Semaphore(int semid):_semid(semid){}
    ~Semaphore(){}

    int id()
    {
        return _semid;
    }
    void P(int who)
    {
        _PV(who,-1);
    }
    void V(int who)
    {
        _PV(who,1);
    }
private:
    void _PV(int who,int data)
    {
        struct sembuf sem_buf;
        sem_buf.sem_num = who;//index
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
//建造者父类
class SemaphoreBuilder
{
public:
    virtual ~SemaphoreBuilder() {};
    virtual void buildKey();
    virtual void setPermission(int perm) = 0;
    virtual void setSemNum(int num) = 0;
    virtual void setInitVal(std::vector<int> init_vals) = 0;
    virtual void build(int flag)=0;
    virtual void initSem() = 0;
    virtual std::shared_ptr<Semaphore> getSem() = 0;
};

class ConcreteSemaphoreBuilder:public SemaphoreBuilder
{
public:
    ConcreteSemaphoreBuilder(){}

    virtual ~ConcreteSemaphoreBuilder(){}
    virtual void buildKey() override
    {
        //构建键值
        std::cout<<"build a Semphore"<<std::endl;
        _key = ftok(SEM_PATH.c_str(),SEM_PROJ_ID);
        if(_key<0)
        {
            std::cerr<<"ftok failed"<<std::endl;
            exit(-1);
        }
        std::cout<<"Got key" << intToHexStr(_key)<<std::endl;
    }
    virtual void setPermission(int perm)override
    {
        _perm = perm;
    }
    virtual void setSemNum(int num)override
    {
        _num = num;
    }
    virtual void setInitVal(std::vector<int> init_vals)
    {
        _init_vals = init_vals;
    }
    virtual void build(int flag)
    {
        //获取信号量集合
        int _semid = semget(_key,_num, flag);
        if(_semid < 0)
        {
            std::cerr<< "semget failed\n";
            exit(-1);
        }
        std::cout<<"get smid: "<<_semid<<std::endl;
        _sem = std::make_shared<Semaphore>(_semid);
    }

    virtual void initSem()override
    {
        if(_num > 0 && _num == _init_vals.size())
        {
            for(int i = 0;i<_init_vals.size();++i)
            {
                if(!_init(_sem->id(),i,_init_vals[i]))
                {
                    std::cerr<<"初始化失败\n";
                    exit(-1);
                }
            }
        }
        else
        {
            std::cerr<< "参数设置错误\n";
        }
    }

    std::shared_ptr<Semaphore> getSem()override{return _sem;}
private:
    bool _init(int smid,int index,int val)
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

private:
    std::shared_ptr<Semaphore> _sem;
    key_t _key;
    int _perm;
    int _num;
    std::vector<int> _init_vals;
};

class Director
{
public:
    void Construct(std::shared_ptr<SemaphoreBuilder> builder,int flag,int perm = 0666,int num = default_num,std::vector<int>init_vals = {1})
    {
        builder->buildKey();
        builder->setPermission(perm);
        builder->setSemNum(num);
        builder->setInitVal(init_vals);
        builder->build(flag);
        if(flag == BUILD_SEM)
        {
            builder->initSem();
        }
    }
};