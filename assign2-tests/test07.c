int plus(int a, int b) {
   return a+b;
}

int minus(int a, int b) {
   return a-b;
}

int clever(int a, int b, int (*a_fptr)(int, int)) {
    return a_fptr(a, b);
}


int moo(char x, int op1, int op2) {
    int (*a_fptr)(int, int) = plus;
    int (*s_fptr)(int, int) = minus;
    int (*t_fptr)(int, int) = 0;
    int (*af_ptr)(int ,int ,int(*)(int, int)) = clever;
    unsigned result = 0;
    if (x == '+') {
       t_fptr = a_fptr;
    }else
    {
       t_fptr = s_fptr;
    }

    if(op1 > 0 )
       result= af_ptr(op1, op2, t_fptr);
    else
       result= af_ptr(op1, op2, a_fptr);
    return result;
}

/// 10 : plus, minus
/// 28 : clever
/// 30 : clever
