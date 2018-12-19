//Implement a C function called ex4 such that it achieves 
//the same functionality as the machine code of objs/ex4_sol.o
//Note: you need to figure out ex4()'s function signature yourself; 
//the signature is not void ex4()
  
#include <assert.h>
//rdi, rsi, rdx, rcx
long
ex4(long a) {
	if(a >1){
		
		a = a -1;
		int x = ex4(a);
		a = a -1;
		int  y = ex4(a);
		y = x+ y;
		return  y;

		
	}
	else{
		return a;
	}

}
