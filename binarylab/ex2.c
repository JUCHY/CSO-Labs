//Implement a C function called ex2 such that it achieves 
//the same functionality as the machine code of objs/ex2_sol.o
//Note: you need to figure out ex2()'s function signature yourself; 
//the signature is not void ex2()
  
#include <assert.h>
//rdi, rsi, rdx, rcx
int
ex2(char *a) {
	int count = 0;
	int i = 0;
	while(a[i] != '\0'){
		if(a[i]==0x61){
			count++;
		}
		i++;
	}
	return count;
}