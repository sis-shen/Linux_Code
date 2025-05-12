#include "MyOS.h"

using namespace sup;

void MyOS::createproc(const std::string&proc_name,uint64_t TTL,uint64_t mem_sz,uint64_t io_start,uint64_t io_end)
{
    proc_creat(proc_name,TTL,mem_sz,io_start,io_end);
}

void MyOS::proc_creat(const std::string&proc_name,uint64_t TTL,uint64_t mem_sz,uint64_t io_start,uint64_t io_end)
{
    PCB pcb;
    int64_t pid = proc_create_pid();
    if(pid == -1) return ;//创建失败了
    pcb._PID = pid;
    pcb._TTL = TTL;
    pcb._lifelong = 0;
    pcb._name = proc_name;
    if(io_start > io_end)
    {
        pcb._io_start = io_start;//这里存的是相对时间
        pcb._io_time = io_end-io_start;
        pcb._io_end = 0;        //这里存的是系统时间
        pcb._has_io = true;
    }
    else{
        pcb._has_io = false;
    }

    size_t npages = PageStruct::sz_to_npages(mem_sz);
    void* pstart = _page_table.allocate(npages);
    if(pstart == nullptr) return;   //内存申请失败了
    auto mem = new MemoryStruct;
    mem->total_size = mem_sz;
    mem->_npage = PageStruct::ptr_to_npage(pstart,pmem);
    pcb._memory_struct = mem;
    pcb._status = PCB::CREATE;
    pcb._slice_len = ROUND_LEN;
    if(!_ready_list.empty())pcb._status = PCB::READY;
    _ready_list.push_back(pcb);
}

void MyOS::killproc(pid_t pid)
{
    proc_kill(pid);

}
void MyOS::psproc()
{
    printf("name    pid    status    address    size\n");
    for(auto& pcb: _ready_list)
    {
        printf("%-8s%-7u%-10s%-11u%u\n",pcb._name.c_str(),pcb._PID,pcb.getStatStr().c_str(),npage2addr(pcb._memory_struct->_npage),pcb._memory_struct->total_size);
    }

    for(auto& pcb: _blocked_list)
    {
        printf("%-8s%-7u%-10s%-11u%u\n",pcb._name.c_str(),pcb._PID,pcb.getStatStr().c_str(),npage2addr(pcb._memory_struct->_npage),pcb._memory_struct->total_size);
    }
}

void MyOS::mem()
{
    printf("总内存: %u字节\t 已使用: %u字节\t 空闲: %u字节\n",_page_table.get_total_pages()*PAGE_SIZE,_page_table.get_used_pages()*PAGE_SIZE,_page_table.get_free_pages()*PAGE_SIZE);
}

void MyOS::createfile(const std::string&filename)
{

}
void MyOS::deletefile(const std::string&filenmae)
{

}
void MyOS::run_one_cycle()
{

    //进程调度
    proc_schedule();
    //运行当前进程
    _time++;
    if(!_ready_list.empty())
    {
        auto& pcb = _ready_list.front();
        pcb._TTL--;
        pcb._lifelong++;
        pcb._slice_len--;
        if(pcb._TTL == 0)
        {
            pcb._status = PCB::EXIT;
            proc_kill(pcb._PID);
        }
    }
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
        _max_pid++;
        return ret;
    }
}

void MyOS::proc_kill(pid_t pid)
{
    //释放内存
    {
        auto it = _ready_list.begin();
        while(it != _ready_list.end())
        {
            if(it->_PID == pid)
            {
                realse_npage(it->_memory_struct->_npage,it->_memory_struct->total_size);
                delete it->_memory_struct;
                break;
            }
        }
        {    
        auto it = _blocked_list.begin();
        while(it != _blocked_list.end())
        {
            if(it->_PID == pid)
            {
                realse_npage(it->_memory_struct->_npage,it->_memory_struct->total_size);
                delete it->_memory_struct;
                break;
            }
        }}
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
    
        {        
            auto it = _blocked_list.begin();
        while(it != _blocked_list.end())
        {
            if(it->_PID == pid)
            {
                _blocked_list.erase(it);
                return;
            }
        }}
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
    //调度就绪队列
    auto it = _ready_list.begin();
    while(it != _ready_list.end())
    {
        //时间片不足
        if(it->_slice_len == 0)
        {
            it->_status = PCB::READY;
            it->_slice_len = ROUND_LEN;
            _ready_list.push_back(*it);
            it = _ready_list.erase(it);
        }
        //触发IO时间
        else if(it->_has_io && it->_io_start == it->_lifelong)
        {
            it->_status = PCB::BLOCKED;
            it->_io_end = _time + it->_io_time;
            it->_slice_len = ROUND_LEN;
            _blocked_list.push_back(*it);
            it = _ready_list.erase(it);
        }
        else 
        {
            it->_status = PCB::RUN;
            break;//退出调度
        }
    }
    _ready_list.front()._status = PCB::RUN;
    //调度阻塞队列
    if(!_blocked_list.empty())
    {
        for(auto it = _blocked_list.begin();it != _blocked_list.end();)
        {
            if(it->_has_io == true && it->_io_end == _time)
            {
                it->_has_io = false;
                it->_slice_len = ROUND_LEN;
                _ready_list.push_back(*it);
                it = _blocked_list.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}
uint64_t MyOS::npage2addr(uint64_t npage)
{
    return (npage << PAGE_SHIFT);
}


void MyOS::realse_npage(uint64_t npage,uint64_t size)
{
    _page_table.free_mem(npage,PageStruct::sz_to_npages(size));
}
void MyOS::swapin()
{

}
void MyOS::swapout()
{

}

void MyOS::set_mem()
{
    pmem = (char*) malloc(MEMORY_SIZE);
}
void MyOS::set_disk()
{
    pdisk = (char*) malloc(DISK_SIZE);
}