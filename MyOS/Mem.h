#pragma once
#include "PageTable.h"


class MemoryStruct
{
public:
////////////////////////
// 自定义内容

uint64_t _npage;
uint64_t total_size;

////////////////////////////
unsigned long task_size;/* size of task vm space */
unsigned long start_code, end_code, start_data, end_data;
unsigned long start_brk, brk, start_stack;
unsigned long arg_start, arg_end, env_start, env_end;
unsigned long mmap_base;/* base of mmap area */
unsigned long total_vm;/* Total pages mapped */
unsigned long locked_vm;/* Pages that have PG_mlocked set */
unsigned long pinned_vm; /* Refcount permanently increased */
unsigned long data_vm;/* VM_WRITE & ~VM_SHARED & ~VM_STACK */
unsigned long exec_vm;/* VM_EXEC & ~VM_WRITE & ~VM_STACK */
unsigned long stack_vm;/* VM_STACK */

};