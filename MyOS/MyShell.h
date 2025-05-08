#pragma once
#include "MyOS.h"
#include <string>
#include <sstream>
#include <iostream>
using namespace std;
class MyShell
{
public:
    void run()
    {
        sup::MyOS& os = sup::MyOS::getInstance();
        string cmd_line;

        while(true)
        {

            cout << "[time:"<< os.getTime()<<"]MyOS> "; 
            getline(cin, cmd_line);
    
            stringstream ss(cmd_line);
            string cmd;
            ss >> cmd;

            if(cmd == "creatproc")
            {
                string proc_name;
                uint64_t TTL, mem_sz, io_start, io_end;
                
                if(ss >> proc_name >> TTL >> mem_sz >> io_start >> io_end) {
                    os.createproc(proc_name, TTL, mem_sz, io_start, io_end);
                } else {
                    cout << "用法: creatproc <名称> <生存周期> <内存大小> <IO起始> <IO结束>\n";
                }
            }
            else if(cmd == "killproc")
            {
                pid_t pid;
                if(ss >> pid) {
                    os.killproc(pid);
                } else {
                    cout << "用法: killproc <进程ID>\n";
                }
            }
            else if(cmd == "psproc")
            {
                os.psproc();
            }
            else if(cmd == "mem")
            {
                os.mem();
            }
            else if(cmd == "creatfile")
            {
                string filename;
                if(ss >> filename) {
                    os.createfile(filename);
                } else {
                    cout << "用法: creatfile <文件名>\n";
                }
            }
            else if(cmd == "deletefile") 
            {
                string filename;
                if(ss >> filename) {
                    os.deletefile(filename);
                } else {
                    cout << "用法: deletefile <文件名>\n";
                }
            }
            else if(cmd == "exit")
            {
                cout << "MyOS: bye~\n";
                return;
            }
            else if(!cmd.empty()) 
            {
                cout << "未知指令，可用命令:\n"
                     << "  creatproc <name> <TTL> <mem> <io_start> <io_end>\n"
                     << "  killproc <pid>\n"
                     << "  psproc\n"
                     << "  mem\n"
                     << "  creatfile <name>\n"
                     << "  deletefile <name>\n"
                     << "  exit\n";
            }

            
        }

    }
};