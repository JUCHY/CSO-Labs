/* NYU Computer Systems Organization Lab 2
 * Rabin-Karp Substring Matching
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

#include "rkgrep.h"
#include "bloom.h"

#define PRIME 961748941

// calculate modulo addition, i.e. (a+b) % PRIME
long long
madd(long long a, long long b)
{
	return (a + b) % PRIME;
}

// calculate modulo substraction, i.e. (a-b) % PRIME
long long
msub(long long a, long long b)
{
	return (a>b)?(a-b):(a+PRIME-b);
}

// calculate modulo multiplication, i.e. (a*b) % PRIME
long long
mmul(long long a, long long b)
{
	return (a*b) % PRIME;
}

/* naive_substring_match returns number of positions in the document where
 * the pattern has been found.  In addition, it stores the first position 
 * where the pattern is found in the variable pointed to by first_match_ind
 *
 * Both its arguments "pattern" and "doc" are null-terminated C strings.
 */
int
naive_substring_match(const char *pattern, const char *doc, int *first_match_ind)
{
	/* Your code here */
	int i = 0;
	int count = 0;
	int match = 0;
	int first_index = 0;

	while(doc[i]!= '\0' ){
		int x = 0;
		while(pattern[x] != '\0' ){
			if(pattern[x]== doc[i+x]){
				match = 1;
			}
			else{
				match = 0;
				break;
			}
			x++;
		}
		if(match == 1){
			if(first_index== 0){
				first_index = 1;
				*first_match_ind = i;
			}
			count++;
		}
		match = 0;
		i++;

	}
		
	return count;
}

/* initialize the Rabin-karp hash computation by calculating 
 * and returning the RK hash over a charbuf of m characters, 
 * i.e. The return value should be 
 * 256^(m-1)*charbuf[0] + 256^(m-2)*charbuf[1] + ... + charbuf[m-1],
 * where 256^(m-1) means 256 raised to the power m-1.
 * Note that all operations *, +, - are modulo arithematic, so you 
 * should use the provided functions mmul, madd, msub.
 * (We use "long long" to represent an RK hash)
 */

long long
pow2(long long num, int pow){
	if(pow == 0){
		return 1;
	}
	if(pow == 1){
		return num;
	}
	long long num2 = num;
	for(int i = 1; i < pow; i++){
		num2 = mmul(num2,num);
	}

	return num2;
}
long long
rkhash_init(const char *charbuf, int m, long long *h)
{
	long long d = 256;
	long long d2 = 1;
	long long p = 0;
	for(int i = 0; i < m; i++){
		d2 = pow2(d, m-1-i);
		long long num = mmul(d2,charbuf[i]);
		p = madd(p,num);
	}
	d2 = pow2(d, m);	
	*h = d2;
	return p;

	/* Your code here */
}


/* Given the rabin-karp hash value (curr_hash) over substring Y[i],Y[i+1],...,Y[i+m-1]
 * calculate the hash value over Y[i+1],Y[i+2],...,Y[i+m] = curr_hash * 256 - leftmost * h + rightmost
 * where h is 256 raised to the power m (and given as an argument).  
 * Note that *,-,+ refers to modular arithematic so you should use mmul, msub, madd.
 */
long long 
rkhash_next(long long curr_hash, long long h, char leftmost, char rightmost)
{
	long long num = mmul(256, curr_hash);
	long long num2 = mmul(leftmost, h);
	num = msub(num,num2);
	num = madd(num, rightmost);
	return num;
	/* Your code here */
}

/* rk_substring_match returns the number of positions in the document "doc" where
 * the "pattern" has been found, using the Rabin-karp substring matching algorithm.
 * Both pattern and doc are null-terminated C strings. The function also stores
 * the first position where pattern is found in the int variable pointed to by first_match_ind
 *
 * Note: You should implement the Rabin-Karp algorithm by completing the 
 * rkhash_init and rkhash_next functions and then use them here.
*/

int
check(char *pattern1, char *pattern2, int m){
	for(int i = 0; i < m; i ++){
		if(pattern1[i] != pattern2[i]){
			return 0;
		}
	}

	return 1;
}

int
rk_substring_match(const char *pattern, const char *doc, int *first_match_ind)
{

	int n = strlen(doc);
	int m = strlen(pattern);
	long long h = 0;
	long long h2 = 0;
	long long pat1 = rkhash_init(pattern, m, &h);
	long long pat2 = rkhash_init(doc, m, &h2);
	int count = 0;
	int hasindex = 0;
	for (int i = 0; i <= n - m; i++) 
    { 
    	
    
        if ( pat1 == pat2 ) 
        { 
        	int match = 1;
        	for (int j = 0; j < m; j++) 
            { 
                if(doc[i+j]!= pattern[j]){
                	match = 0;
                	break;

                }
            } 
            if(match){
	        	if(!hasindex){
	        		hasindex = 1;
	        		*first_match_ind = i;
	        	}
	        	count = count +1;  
	        }
        	  

        } 
  
        pat2 = rkhash_next(pat2,h,  doc[i], doc[i+m]); 
 
    } 
    return count;
}


/* rk_create_doc_bloom returns a pointer to a newly created bloom_filter. 
 * The new bloom filter is populated with all n-m+1 rabin-karp hashes for 
 * all the substrings of length m in "doc".
 * Hint: use the provided bloom_init() and your implementation of bloom_add() here.
 */
bloom_filter *
rk_create_doc_bloom(int m, const char *doc, int bloom_size)
{
	/* Your code here */
	

	int n = strlen(doc);
	bloom_filter *bf = bloom_init(bloom_size);
	long long h = 0;
	long long elm = rkhash_init(doc, m, &h);
	for(int i = 0; i <=n-m; i++){
		bloom_add( bf, elm);
		elm = rkhash_next(elm, h, doc[i], doc[i+m]);
	}


	return bf;
	




}

/* rk_substring_match_using_bloom returns the total number of positions where "pattern" 
 * is found in "doc".  It performs the matching by first checking against the 
 * pre-populated bloom filter "bf" (which has been created by rk_create_doc_bloom on "doc")
 * If the pattern is not found in "bf", then the function immediately returns 0.
 * Otherwise, the function invokes rk_substring_match() function to find "pattern" in "doc".
*/
int
rk_substring_match_using_bloom(const char *pattern, const char *doc, bloom_filter *bf, int *first_match_ind)
{
    /* Your code here */
    int m = strlen(pattern);
	long long h = 0;
    long long check = rkhash_init(pattern, m, &h);
    if(bloom_query(bf, check)){

    	return rk_substring_match(pattern, doc,first_match_ind);

    }
    else{

    	return 0;
    }
}
