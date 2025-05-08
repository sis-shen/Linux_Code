#pragma once
#include "common.h"
namespace sup
{
    class PageStruct
    {
    public:
        static unsigned long page_to_pfn(void* page)
        {
            return ((unsigned long)page >> PAGE_SHIFT);
        }

        static PageStruct* pfn_to_page(unsigned long pfn,PageStruct* mem_map)
        {
            return (mem_map + pfn);
        }
    };


    class PageTable
    {
    
    };

}