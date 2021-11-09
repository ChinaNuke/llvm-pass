#include <stdlib.h>
int plus(int,int);

int minus(int,int);

int clever()
{
    int a=0;
    a=minus(plus(a,10),2);
    a=plus(a,2)?minus(6,a):0;
    return a;
}

int minus(int a,int b)
{
    return a-b;
}
int plus(int a, int b)
{
    return a+b;
}
//9 : minus, plus
//10 : plus, minus
