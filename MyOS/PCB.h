#pragma once
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <vector>
#include "FileStruct.h"
namespace sup{
    using namespace std;

    class PCB
    {
        typedef void (*pfunc_t)();
    public:
        enum Status
        {
            CREATE,
            WITHDRAW,
            READY,
            BLOCK,
            RUN
        };
        
        Status _status;
        pid_t _PID;
        vector<FileStruct> _file_table;
        pfunc_t* _signal_handler_table;

    };
}