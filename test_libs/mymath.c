#include "mymath.h" 

int my_matherror = 0;

int add(int a, int b)
{
  return a+b;
}

int sub(int a,int b)
{
  return a-b;
}

double div(double a,double b) 
{
  if(b == 0)//防止除0错误
  {
    my_matherror = 1;
    return -1;
  }
  return a/b;
}
