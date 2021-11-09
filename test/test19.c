#include <stdlib.h>

int plus(int a, int b) {
   return a+b;
}

int minus(int a,int b)
{
    return a-b;
}

int foo(int a,int b,int(* fptr)(int, int))
{
	return fptr(a,b);
}
int clever() {
  
    int (*af_ptr)(int ,int ,int(*)(int, int))=0;
    int (*pf_ptr)(int,int)=0;
    af_ptr=foo;
    int op1 =1,  op2=2;

    if( op1 > 0 ) //always 
    	pf_ptr=plus;
    else 
    	pf_ptr=minus;

    unsigned result = af_ptr(op1, op2,pf_ptr);
    return 0;
}
// 14 : plus
// 28 : foo