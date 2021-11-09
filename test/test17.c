#include <stdlib.h>
int plus(int,int);

int minus(int,int);

int clever()
{
    int a=0;
    for(int i=0;i<10;i++)
    {
        if(i%2)
            a=plus(a,i);
        else
            a=minus(a,i);
    }
    return a;
}
int foo(int(*pf_ptr)(void))
{
    return pf_ptr();
}

int moo(int (*uf_ptr)(int(*)(void)),int(*pf_ptr)(void))
{
    return uf_ptr(pf_ptr);
}
int use()
{
	int (*uf_ptr)(int (*uf_ptr)(int(*)(void)),int(*)(void));
	uf_ptr=moo;
	return uf_ptr(foo,clever);
}
int minus(int a,int b)
{
    return a-b;
}
int plus(int a, int b)
{
    return a+b;
}
// 12 : plus
// 14 : minus
// 20 : clever
// 25 : foo
// 31 : moo
