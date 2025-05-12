#pragma once
#include "common.h"
#include <memory.h>
#include <stdlib.h>
#include <cassert>
namespace sup
{
    class PageStruct
    {
    public:

        static size_t ptr_to_npage(void* ptr,void* pmem)
        {
            assert(ptr >= pmem); // 确保指针在内存范围内
            return ((char*)ptr - (char*)pmem) / PAGE_SIZE;
        }

                // 将页号转换为指针
        static void* npage_to_ptr(size_t npage, void* pmem)
        {
            return (char*)pmem + npage * PAGE_SIZE;
        }

        static size_t sz_to_npages(size_t sz)
        {
            return (sz + PAGE_SIZE - 1) / PAGE_SIZE;
        }
    };


    class PageTable
    {
    public:
        PageTable()
        :_pmem(nullptr),_total_use(0)
        {
            _total_pages = MEMORY_SIZE / PAGE_SIZE;
            size_t bitmap_size = (_total_pages + 7) / 8; // 计算需要的字节数
            _bitmap = (char*)malloc(bitmap_size);
            memset(_bitmap, 0, bitmap_size); // 初始化位图为全0
        }    

        void set_mem(void* pmem)
        {
            _pmem = (char*)pmem;
        }

        void* allocate(size_t npages)
        {
            size_t total_pages = MEMORY_SIZE / PAGE_SIZE;
            size_t free_pages = 0;
            size_t start_pos = 0;

            // 查找连续npages个空闲页
            for (size_t i = 0; i < total_pages; ++i) {
                size_t pos = i / 8;
                char mask = 1 << (i % 8);

                if (!(_bitmap[pos] & mask)) {
                    if (free_pages == 0) {
                        start_pos = i;
                    }
                    free_pages++;
                    if (free_pages == npages) {
                        // 找到足够的连续空闲页
                        for (size_t j = 0; j < npages; ++j) {
                            size_t shift = start_pos + j;
                            size_t p = shift / 8;
                            char m = 1 << (shift % 8);
                            _bitmap[p] |= m; // 标记为已分配
                        }
                        _total_use += npages;
                        return _pmem + start_pos * PAGE_SIZE;
                    }
                } else {
                    free_pages = 0;
                }
            }
            return nullptr; // 没有足够的连续空闲页
        }

        void free_mem(size_t start_page,size_t npages)
        {
            // if (!ptr) return;

            // size_t start_page = ((char*)ptr - _pmem) / PAGE_SIZE;
            for (size_t i = 0; i < npages; ++i) {
                size_t shift = start_page + i;
                size_t pos = shift / 8;
                char mask = ~(1 << (shift % 8));
                _bitmap[pos] &= mask; // 清除位图标记
            }
            _total_use -= npages;
        }

        // 获取总页数
        size_t get_total_pages() const { return _total_pages; }
        
        // 获取已使用页数
        size_t get_used_pages() const { return _total_use; }
        
        // 获取空闲页数
        size_t get_free_pages() const { return _total_pages - _total_use; }

        ~PageTable()
        {
            free(_bitmap);
        }

        char* _bitmap;
        char* _pmem;
        size_t _total_use;
        size_t _total_pages;
    };

}