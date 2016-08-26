#include <stdio.h>

static char local_input[2048];

int main (int argc, char** argv) {
	puts("the lisp");
	puts("ctrl+c to exit\n");
	
	while (1) {
		fputs("fau> ", stdout);
		fgets(local_input, 2048, stdin);
		printf("-> %s", local_input);
	}
	
	return 0;
}