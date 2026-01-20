#include "common.h"

int factorial(int in){
	if (in==1){return 1;}
	return in * factorial(in-1);
}

int main(int argc, char* argv[])
{	
	
	int arg_i = atoi(argv[1]);
	if (argc == 1 || arg_i<=0){ // if no arg, negative
		printf("Huh?\n");
		return 0;
	}

	// if decimals, string, or char, return "Huh?
	int i = 0;
	while (argv[1][i] != '\0'){
		if (i >=3){ // catches all string bigger than 2 chars long
			printf("Huh?\n");
			return 0;
		}
		if (argv[1][i] - '0' > 9 || argv[1][i] - '0' < 0){ // if not 1 to 9 in ascii, then its a decimal, neg, or other string
			printf("Huh?\n");
			return 0;
		}
	}

	// if bigger than 12 return Overflow
	if (arg_i > 12){ 
		printf("Overflow\n");
		return 0;
	}

	// finally get factorial
	int res = factorial(arg_i);
	printf("%d\n", res);
	return 0;
}
