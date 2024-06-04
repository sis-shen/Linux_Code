#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define LINE_SIZE 1024
#define ARGV_SIZE 1024

#define LEFT "["
#define RIGHT "]"
#define LABLE "# "

char cline[LINE_SIZE];
char* argv[ARGV_SIZE];


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
  const char*pwd = getenv("PWD");
  int n = strlen(pwd);
  while(n)//找最后一个/
  {
    if(pwd[n] == '/')break;
    n--;
  }
  return (pwd+n+1);
}

void Splitcline(char*cline,char**argv,int* argv_n)
{
  memset(argv,0,sizeof(char*)*ARGV_SIZE);
  int i = 1;
  argv[0] = strtok(cline," ");
  for(;i<ARGV_SIZE;i++)
  {
    argv[i] = strtok(NULL," ");
    if(argv[i] == NULL) break;
  }

  *argv_n = i;
}

void Interact(char*cline,int size)
{

  printf(LEFT"%s@%s %s" RIGHT LABLE,getusername(),gethostname(),getpwd());
  char*s = fgets(cline,size,stdin);
  assert(s);
  (void) s;//使用一下s，防止报警告
  cline[strlen(cline)-1] = '\0';
  printf("echo: %s\n",cline);
}

int main()
{
  int quit = 0;
  while(!quit)
  {
    Interact(cline,sizeof(cline));
    int argv_n;
    Splitcline(cline,argv,&argv_n);

    


  }

  return 0;
}
