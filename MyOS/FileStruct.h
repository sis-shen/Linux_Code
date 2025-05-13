#pragma once
#include <string>
#include "common.h"
#include <map>
#include <ctime>
#include <string>
#include <vector>
#include <cstring>
#include <cassert>

using namespace std;

namespace sup
{

    class BlockManager
    {
    public:
        BlockManager()
        :_bitmap(0),_use_count(0)
        {

        }
        void set_pdisk(void* ptr,size_t sz)
        {
            _pdisk_start = (char*)ptr;
            _nblocks = sz / BLOCK_SIZE;
            _bitmap = 0; // 重置位图
            _use_count = 0;
        }

        int get_one_block()
        {
            if (_use_count >= _nblocks) {
                return -1; // 没有空闲块了
            }

            // 查找第一个空闲位
            for (unsigned int i = 0; i < _nblocks; ++i) {
                if (!(_bitmap & (1ULL << i))) {
                    _bitmap |= (1ULL << i); // 标记为已用
                    ++_use_count;
                    return i; // 返回块号
                }
            }
            return -1;//防止报警
        }

        bool free_block(unsigned int block_no)
        {
            if (block_no >= _nblocks || !(_bitmap & (1ULL << block_no))) {
                return false; // 无效块号或已经是空闲块
            }

            _bitmap &= ~(1ULL << block_no); // 清除位图标记
            --_use_count;
            return true;
        }

        // 获取块指针
        char* get_block_ptr(unsigned int block_no) const
        {
            if (block_no >= _nblocks) {
                return nullptr;
            }
            return _pdisk_start + block_no * BLOCK_SIZE;
        }

        // 获取使用统计
        unsigned int get_used_count() const { return _use_count; }
        unsigned int get_total_count() const { return _nblocks; }
        unsigned int get_free_count() const { return _nblocks - _use_count; }


        uint64_t _bitmap;
        uint64_t _nblocks;
        uint64_t _use_count;
        char* _pdisk_start;
    };

// inode结构(存储文件元数据和数据块指针)
struct Inode {
    uint64_t inode_id;
    size_t size;                     // 文件大小(字节)
    unsigned short link_count;       // 硬链接计数
    unsigned short block_count;      // 使用的块数
    unsigned int blocks[MAX_BLOCK_POINTERS]; // 直接块指针

//============================================================
    time_t create_time;              // 创建时间
    time_t modify_time;              // 修改时间
    time_t access_time;              // 最后访问时间
    unsigned int indirect_block;     // 一级间接块指针
    unsigned int double_indirect_block; // 二级间接块指针
    unsigned int mode;               // 文件类型和权限
//============================================

    Inode(uint64_t id = 0) {
        inode_id = id;
        size = 0;
        link_count = 1;
        block_count = 0;
        memset(blocks, 0, sizeof(blocks));
        
        //=======================
        create_time = modify_time = access_time = time(nullptr);
        indirect_block = double_indirect_block = 0;
        mode = 0;
        //==========================
    }

    void init_size(BlockManager& bm,uint64_t size)
    {
        _bm = &bm;
        int cnt = size/BLOCK_SIZE;
        for(int i = 0;i<cnt;++i)
        {
            int block_no = bm.get_one_block();
            if(block_no < 0) assert(false);//数据块申请失败
            if(add_block_pointer(block_no) == false)
            {
                if(block_count > 12)
                {
                    std::cerr<<"文件过大！请升级文件系统\n";
                }
                else
                {
                    std::cerr<< "debug: block_no: "<<block_no<<"cout:"<<block_count <<std::endl;
                }
                assert(false);
            }
        }
        this->size = size;
    }

    void clear()
    {
        for(int i = 0;i<block_count;++i)
        {
            _bm->free_block(_bm->free_block(blocks[i]));
        }
    }

    // 添加数据块指针
    bool add_block_pointer(unsigned int block_no) {
        if (block_count < MAX_BLOCK_POINTERS) {
            blocks[block_count++] = block_no;
            return true;
        }
        // 处理间接块的扩展先不作了
        return false;
    }

    BlockManager* _bm;
};

// 轻量级FCB(主要存储文件名和inode指针)
struct FCB {
    std::string filename;  // 文件名
    unsigned int inode_no;           // inode编号
    unsigned char attributes;        // 文件属性(位掩码)
    
    // 文件属性位掩码定义
    enum Attributes {
        READ_ONLY = 0x01,
        HIDDEN    = 0x02,
        SYSTEM    = 0x04,
        ARCHIVE   = 0x08
    };

    FCB() : inode_no(0), attributes(0) {

    }

    // 设置/获取属性
    void set_attribute(Attributes attr) {
        attributes = attr;
    }

};

// 文件系统管理inode的类
class InodeManager {
    std::map<uint64_t,Inode> inodes;
    unsigned int next_inode_no;
    
public:
    InodeManager() : next_inode_no(1) {}
    
    // 创建新inode
    uint64_t create_inode() {
        inodes[next_inode_no] = Inode(next_inode_no);
        return next_inode_no++;
    }
    
    // 获取inode
    Inode* get_inode(unsigned int inode_no) {
        if (inode_no == 0 || inode_no > inodes.size()) {
            return nullptr;
        }
        return &inodes[inode_no-1];
    }

    bool decount_inode(uint64_t inode_no)
    {
        if (inodes.find(inode_no) == inodes.end()) {
            return false;
        }
        inodes[inode_no].link_count--;
        if(inodes[inode_no].link_count == 0)
        {
            free_inode(inode_no);
        }
        return true;
    }
    
    // 释放inode
    bool free_inode(uint64_t inode_no) {
        if (inodes.find(inode_no) == inodes.end()) {
            return false;
        }
        inodes[inode_no].clear();
        inodes.erase(inode_no);
        return true;
    }
};

    class FileSystem
    {
    public:
        FileSystem()
        {}

        void set_pdisk(void* pdisk)
        {
            _bm.set_pdisk(pdisk,DISK_SIZE);
        }

        bool create_file(const std::string& filename,uint64_t file_size)    
        {
            if(_name_to_fcb.find(filename) != _name_to_fcb.end() )
            {
                printf("文件已存在!\n");
                return false;
            }

            FCB fcb;
            fcb.filename = filename;
            fcb.inode_no = _im.create_inode();
            Inode* inode = _im.get_inode(fcb.inode_no);
            inode->init_size(_bm,file_size);
            _name_to_fcb[filename] = fcb;
            return true;
        }

        void delete_file(const std::string& name)
        {
            if(_name_to_fcb.find(name) == _name_to_fcb.end())
            {
                return;
            }

            _im.decount_inode(_name_to_fcb[name].inode_no);
            _name_to_fcb.erase(name);
        }

        void ls_file(const std::string& name)
        {
            if(_name_to_fcb.find(name) == _name_to_fcb.end()) 
            {
                printf("文件:%s不存在!\n",name.c_str());
                return;
            }
            uint64_t id =_name_to_fcb[name].inode_no;
            printf("id:%lu\tfilename:%s\tblock_no:%u\n",id,name.c_str(),(_im.get_inode(id))->blocks[0]);
        }


        std::map<std::string,FCB> _name_to_fcb;
        InodeManager _im;
        BlockManager _bm;
    };

    //描述内存级文件
    class FileStruct
    {

    };
}
