#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "bmgrep.h"

/* allocate, initialize and return the delta1 array according to the BM paper. 
 * The delta1 array should have 128 integer elements, one for each valid ASCII character.
 * delta1[c] is set the distance from the right where c is found in the pattern.
 * if c is not found in the pattern, x = strlen(pattern).
 */
int*
init_delta1(const char *pattern) {
	int *c = malloc(sizeof(int*)*128);
	int n = strlen(pattern);
	for(int i = 0; i < 128; i++){
		c[i] = n;		
	}
	for(int x = 0; x < n; x++){
		printf("%c\n", pattern[x]);
		
			c[pattern[x]] = n - x -1;
			if(pattern[x] == 'a'){
				printf("test%d\n", x );
			}
		}
	// Your code here. Do not return NULL
	return c;
}

/* Let plen = strlen(pattern).
 * Return the largest position where
 * the terminal substring pattern[j+1..plen] re-occurs and is not 
 * preceded by the character pattern[j].
 *
 * The BM paper describes how to calculate this value, aka rpr(j),  
 * in Sec 4 (the last two paragraphs).
 * Note that in your implementation, positions start from 0. 
 * By contrast, in the BM paper, positions start from 1.
 */
int find_rpr(const char *pattern, int j) {
	int plen = strlen(pattern);
	for(int i = 0; i<j; i++){
		if(pattern[i] != pattern[j]){
			continue;
		}
		else{
			for(int x= 0; x< plen-j; x++){
				if(pattern[i+x] != pattern[j+x]){
					break;
				}
			}
			return i;
		}
	}
	// Your code here. Do not return strlen(pattern), which is never a valid return value.
	return 1;
}

/* Allocate, initialize and return the delta2 array according to the BM paper (page 765).
 * The delta2 array should contain strlen(pattern) integer elements, 
 * one corresponding to each position in the pattern. 
 * Let plen = strlen(pattern).
 * delta2[j] = plen - find_rpr(pattern,j) 
 * Note that the above formula uses "plen" instead of "patlen+1" as in the BM paper.
 * This is because string positions range from 1 to patlen in the BM paper,
 * however, in C, string positions range from 0 to strlen(pattern)-1.
 * Also note that delta2[plen-1] should always be 1.
 */
int*
init_delta2(const char *pattern) {
	int plen = strlen(pattern);
	int *c = malloc(sizeof(int*)*plen);
	for(int j = 0; j < plen; j++){
		c[j] = plen- find_rpr(pattern,j);
	}

	printf("%d\n", c[plen-1]);

	// Your code here. Do not return NULL
	return c;
}


/* Return the position of the first occurance of the pattern in doc using Boyer-Moore algorithm.
 * If the pattern is not found, return -1.
 * You need to use helper functions (init_delta1, init_delta2) and follow the algorithm in Sec 4.
 * Note: do not use any goto statements in your code!
 *
 * Additionally, return the number of alignments you've done during the matching process by writing to
 * the deferenced pointer n_aligns.
 * The number of alignments is the number of times you've advanced the "i" pointer to the right, 
 * including its initialization.
 * For example, the number of alignments done for the example on page 764 is 5.
 */
int 
boyer_moore(const char *pattern, const char *doc, int *n_aligns) {

	int* delta1 = init_delta1(pattern);
	int i = 0;
	int n = strlen(doc);
	int m = strlen(pattern);
	int count = 0;
	while(i<=(n-m)){
		int match = 1;
		for(int j = m-1; j>=0; j--){
			if (pattern[j] != doc[i+j])
			{
				int x = delta1[doc[i+j]];
				i = i +x;
				match = 0;
				break;
			}
		}
		if ((match))
		{
			count++;
			/* code */
		}
	}
	
	if(!count){
		return -1;
	}
}

