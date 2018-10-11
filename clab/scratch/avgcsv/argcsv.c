#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

//need number of columns
//need number of rows
//need individual values in each row
//need some place to store those values;
//array of lines
//# of columns
//# of rows

int strlen2(char* buf){
	int count = 1;
	int index = 0;
	while(buf[index]!= '\0'){
		if(buf[index]==';' ){
			count++;
		}
		index++;
	}
	return count;
}

int
countlines(FILE *fp){
	int count = 0;
	char buf[1000];

	while(!feof(fp)){
		printf("%d\n",count );
		if(!fgets(buf,1000, fp)){
			break;
		}

		count++;
	}
	return count;
}


void
storelines(FILE *fp, char **stringarray){
	int count = 0;
	char buf[1000];

	while(!feof(fp)){
		printf("%d\n",count );
		if(!fgets(buf,1000, fp)){
			break;
		}

		stringarray[count];
		count++;
	}

}



//char**
//storefile(FILE *fp, int* linenum, char **stringlist){
//	*linenum = countlines(fp, stringlist);
//	return stringlist;
	
//}

float* 
atoi2(char *s, int len, float *store){
	float result = 0;
	float decresult = 0;
	int i = 0;
	int arrayindex = 0;
	int postpoint = 0;
	while(s[i]>='0' && s[i]>= '9' || s[i] == ';' || s[i]== '.'){
		if((s[i])==';'){
			store[arrayindex] = result;
			arrayindex++;
			result = 0;
			postpoint = 0;
			i++;
			continue;
		}
		if((s[i])=='.'){
			postpoint = 1;
			i++;
			continue;
		}
		if(postpoint==1){
			decresult = (result + (s[i]-'0')/(pow(10.0,postpoint)));
			postpoint++;
			i++;
		}
		result = result*10 + (s[i] - '0');
		i++;
	}

	return store;

}

void
strcopy(char *dst, char *src, int len){
	int i = 0;
	while( src[0]!= '\0'){
		dst[i] = src[i];
		i++;
	}
}

void
tonumarray(char** x, float** intarray, int len, int column){
	float *curr = (float *)x;
	int length = 0;
	float store[column];
	for(int i = 0; i < len; i ++){
		intarray[i] = atoi2(x[i], column, store);
	}

}

void
printsumarray(float **array, int len, int column){
	float sum = 0;
	for(int j = 0; 0< column; j++){
		for(int i = 0; i < len; i ++){
			sum = sum + array[i][j];
		}

		printf("%.2f\n", sum/len);

}



}

int main(int argc, char **argv)
{

	FILE *fp = fopen(argv[1], "r");
	int len = countlines(fp);
	printf("%d\n", len);
	char *stringarray[len];
	storelines(fp, stringarray);
	printf("%s\n", stringarray[0] );
	int column = strlen2(stringarray[1]);
	printf("%d column\n", column);
	float **numarray;
	tonumarray(stringarray,numarray, len, column );
	printsumarray(numarray, len, column);
	fclose(fp);

}


