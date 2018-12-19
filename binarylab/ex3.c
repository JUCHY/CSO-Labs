//Implement a C function called ex3 such that it achieves 
//the same functionality as the machine code of objs/ex3_sol.o
//Note: you need to figure out ex3()'s function signature yourself; 
//the signature is not void ex3()
  
#include <assert.h>
//rdi, rsi, rdx, rcx
char *
ex3(char* arr1, char* arr2, int n) {
	int count = 0;
	for(int i = 0;i < n; i++){
		count = i;
		char x = arr1[i];
		char y = arr2[i];
		arr1[i] = y;
		arr2[i] = x;

	}

	return arr2 + count;

}
