#include "MyOS.h"

using namespace sup;

void MyOS::createproc(const string&proc_name,uint64_t TTL,uint64_t mem_sz,uint64_t io_start,uint64_t io_end)
{
    proc_creat(proc_name,TTL,mem_sz,io_start,io_end);
}

void MyOS::proc_creat(const string&proc_name,uint64_t TTL,uint64_t mem_sz,uint64_t io_start,uint64_t io_end)
{
    PCB pcb;
    int64_t pid = proc_create_pid();
    if(pid == -1) return ;//创建失败了
    pcb._PID = pid;
    pcb._start = _time;
    pcb._end = (_time + TTL)%OS_MAX_TIME;
    if(io_start > io_end)
    {
        pcb._io_start = pcb._start + io_start;
        pcb._io_start = pcb._start + io_end;
        pcb._has_io = true;
    }
    else{
        pcb._has_io = false;
    }

    auto mem = new MemoryStruct;
    mem->total_size = mem_sz;
    pcb._memory_struct = mem;
    pcb._status = PCB::READY;
    pcb._slice_len = ROUND_LEN;
    _ready_list.push_back(pcb);
}

void MyOS::killproc(pid_t pid)
{
    proc_kill(pid);

}
void MyOS::psproc()
{
    printf("pid    status    address    size");
    for(auto& pcb: _ready_list)
    {
        printf("%-7u%-10s%-11u%u\n",pcb._PID,pcb.getStatStr(),npage2addr(pcb._memory_struct->_npage),pcb._memory_struct->total_size);
    }

    for(auto& pcb: _blocked_list)
    {
        printf("%-7u%-10s%-11u%u\n",pcb._PID,pcb.getStatStr(),npage2addr(pcb._memory_struct->_npage),pcb._memory_struct->total_size);
    }
}
void MyOS::mem()
{

}
void MyOS::createfile(const string&filename)
{

}
void MyOS::deletefile(const string&filenmae)
{

}
void MyOS::run_one_cycle()
{
    //进程调度
    proc_schedule();
    //运行当前进程
    if(_ready)

    //检查阻塞队列是否有要唤醒的

    
}

uint64_t MyOS::getTime()
{
    return _time;
}

int32_t MyOS::proc_create_pid()
{
    int32_t ret = _max_pid;
    if(ret == INT32_MAX)
    {
        return -1;//进程数超标了
    }
    else 
    {
        return ret;
    }
}

void MyOS::proc_kill(pid_t pid)
{
    proc_kill(pid);

    //释放内存
    {
        auto it = _ready_list.begin();
        while(it != _ready_list.end())
        {
            if(it->_PID == pid)
            {
                realse_npage(it->_memory_struct->_npage,it->_memory_struct->total_size);
                delete it->_memory_struct;
                return;
            }
        }
    
        auto it = _blocked_list.begin();
        while(it != _blocked_list.end())
        {
            if(it->_PID == pid)
            {
                realse_npage(it->_memory_struct->_npage,it->_memory_struct->total_size);
                delete it->_memory_struct;
                return;
            }
        }
    }
    //移出队列
    {
        auto it = _ready_list.begin();
        while(it != _ready_list.end())
        {
            if(it->_PID == pid)
            {
                _ready_list.erase(it);
                return;
            }
        }
    
        auto it = _blocked_list.begin();
        while(it != _blocked_list.end())
        {
            if(it->_PID == pid)
            {
                _blocked_list.erase(it);
                return;
            }
        }
    }

}
void MyOS::proc_block(pid_t id)
{
    auto it = _ready_list.begin();
    while(it != _ready_list.end())
    {
        if(it->_PID == id)
        {
            it->_status = PCB::BLOCKED;
            it->_slice_len = ROUND_LEN;
            _blocked_list.push_back(*it);

            _ready_list.erase(it);
            return;
        }
    }
}
void MyOS::proc_wakeup(pid_t id)
{        
    auto it = _blocked_list.begin();
    while(it != _blocked_list.end())
    {
        if(it->_PID == id)
        {
            it->_status = PCB::READY;
            _ready_list.push_back(*it);
            _blocked_list.erase(it);
            return;
        }
    }

}
void MyOS::proc_schedule()
{
    auto it = _ready_list.begin();
    if(it != _ready_list.end())
    {
        if(it->_slice_len == 0)
        {
            it->_status = PCB::READY;
            _ready_list.push_back(*it);
            _ready_list.pop_front();
        }
    }

    _ready_list.front()._status = PCB::RUN;
}
uint64_t MyOS::npage2addr(uint64_t npage)
{
    return (npage << PAGE_SHIFT);
}