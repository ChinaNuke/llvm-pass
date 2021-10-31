int plus(int a, int b) {
   return a+b;
}

int clever() {
    int (*a_fptr)(int, int) = plus;

    int op1 =1,  op2=2;

    unsigned result = a_fptr(op1, op2);
    return 0;
}


///  10 : plus
