#pragma once

#define PAGE_SIZE (4*1024)      //4kB的页大小
#define MEMORY_SIZE (2LL*1024*1024*1024)    //内存的大小
#define DISK_SIZE (8LL*1024*1024*1024)      //外存的大小

#define PAGE_SHIFT (12)             //页大小对应的偏移量

#define ROUND_LEN (10)              //进程调度的时间片长度

//文件系统参数
#define BLOCK_SIZE (4*1024) //4kB的数据块大小
#define MAX_FILENAME_LEN (256)    //文件名最大长度
#define MAX_BLOCK_POINTERS (12)   //最大直接指针的数量