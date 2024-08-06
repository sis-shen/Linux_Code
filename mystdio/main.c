#include "Mystdio.h"

int main()
{
    _FILE* fp = _fopen("test.txt","w");
    if(fp == NULL) return -1;

    const char*msg = "hello mystdio";

    _fwrite(fp,msg,strlen(msg));
    _fclose(fp);
    return 0;
}