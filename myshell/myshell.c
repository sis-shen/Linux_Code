#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LINE_SIZE 1024
#define ARGV_SIZE 32 
#define DLIM " \t"
#define EXIT_CODE 446

#define LEFT "["
#define RIGHT "]"
#define LABLE "# "

char cline[LINE_SIZE];
char* argv[ARGV_SIZE];
int last_code = 0;
char pwd[LINE_SIZE];


const char* getusername()
{
  return getenv("USER");
}

const char* gethostname()
{
  return getenv("HOSTNAME");
}

const char* showpwd()
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

void getpwd()
{
  getcwd(pwd,sizeof(pwd)-1);
}

void Splitcline(char*cline,char**argv,int* argv_n)
{
  memset(argv,0,sizeof(char*) * ARGV_SIZE);
  int i = 1;
  argv[0] = strtok(cline,DLIM);
  if(argv[0])
    while(argv[i++]=strtok(NULL,DLIM));

  *argv_n = i-1;
}

void Interact(char*cline,int size)
{

  printf(LEFT"%s@%s %s" RIGHT LABLE,getusername(),gethostname(),showpwd());
  char*s = fgets(cline,size,stdin);
  assert(s);
  (void) s;//使用一下s，防止报警告
  cline[strlen(cline)-1] = '\0';
  //optrintf("echo: %s\n",cline);
}

void ExeternalCommand()
{
  pid_t id =fork();
  if(id < 0)
  {
    perror("fort");
    return;
  }
  else  if(id == 0)
  {
    execvp(argv[0],argv+1);
    exit(446);
  }
  else 
  {
    int status = 0;
    pid_t rid = waitpid(id,&status,0);
    if(rid == id)
    {
      last_code = WEXITSTATUS(status);
    }
    return;
  }
}

int BuildCommand(char* _argv[],int _argv_n)//处理内建命令
{
  if(_argv_n == 2 && strcmp(_argv[0],"cd")== 0)
  {
    chdir(argv[1]);
    getpwd();
    sprintf(getenv("PWD"),"%s",pwd);
    
  }
  return 0;
}


int main()
{
  int quit = 0;
  while(!quit)
  {
    Interact(cline,sizeof(cline));
    int argv_n;
    Splitcline(cline,argv,&argv_n);

    if(argv_n == 0 )continue;
    
    int flag = BuildCommand(argv,argv_n);
    if(!flag) ExeternalCommand();


  }

  return 0;
}
