#include <stdlib.h>
int plus(int,int);

int minus(int,int);
extern int clever();
int use()
{
    return clever();
}
int minus(int a,int b)
{
    return a-b;
}
int plus(int a, int b)
{
    return a+b;
}
// 8 : clever