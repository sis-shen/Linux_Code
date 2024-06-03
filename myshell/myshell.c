#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_SIZE 1024


#define LEFT "["
#define RIGHT "]"
#define LABLE "#"

char cline[LINE_SIZE];


const char* getusername()
{
  return getenv("USER");
}

const char* gethostname()
{
  return getenv("HOSTNAME");
}

const char* getpwd()
{
  return getenv("PWD");
}



int main()
{

  printf(LEFT"%s@%s %s"RIGHT,getusername(),gethostname(),getpwd());
  scanf("%s",cline);
  printf("echo: %s\n",cline);
  return 0;
}
