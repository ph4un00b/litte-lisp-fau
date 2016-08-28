#include <stdio.h>
#include <stdlib.h>
#include "mpc/mpc.h"

#include <editline/readline.h>
// #include <editline/history.h>

// static char local_input[2048];

long eval_operation(long x, char* operator, long y) {
	if (strcmp(operator, "+") == 0) { return x + y; }
	if (strcmp(operator, "-") == 0) { return x - y; }
	if (strcmp(operator, "*") == 0) { return x * y; }
	if (strcmp(operator, "/") == 0) { return x / y; }
	return 0;
}

// o
long eval(mpc_ast_t* tree) {
	if (strstr(tree->tag, "number")) {
		return atoi(tree->contents);
	}
	
	// after child[0] '('
	char* operator = tree->children[1]->contents;
	
	long x = eval(tree->children[2]);
	
	// next children
	int index = 3;
	while (strstr(tree->children[index]->tag, "expr")) {
		x = eval_operation(	x, 
							operator, 
							eval(tree->children[index]));
		index++;
	}
	
	return x;
}

// o
int number_of_nodes(mpc_ast_t* tree) {
	if (tree->children_num == 0) { return 1; }
	if (tree->children_num >= 1) {
		int node_counter = 1;
		
		for (int i = 0; i < tree->children_num; i++) {
			node_counter = 	node_counter + 
							number_of_nodes(tree->children[i]);
		}
		
		return node_counter;
	}
	
	return 0;
}

int main (int argc, char** argv) {
	puts("ctrl+c to exit\n");
	
	/* Create Some Parsers */
	mpc_parser_t* Number   = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expr     = mpc_new("expr");
	mpc_parser_t* Lispy    = mpc_new("lispy");

	/* Define them with the following Language */
	mpca_lang(MPCA_LANG_DEFAULT,
  	"                                                     \
    	number   : /-?[0-9]+([.][0-9]+)?/ ;                             \
	    operator : '+' | '-' | '*' | '/' | '%' ;                  \
    	expr     : <number> | '(' <operator> <expr>+ ')' ;  \
	    lispy    : /^/ <operator> <expr>+ /$/ ;             \
	",
  	Number, Operator, Expr, Lispy);
	
	while (1) {
// 		fputs("fau> ", stdout);
		char* input = readline("fau> ");
		
		add_history(input);
		
// 		fgets(local_input, 2048, stdin);
		printf("-> %s\n", input);
		
		/* Attempt to Parse the user Input */
		mpc_result_t r;
		if (mpc_parse("<stdin>", input, Lispy, &r)) {
  			/* On Success Print the AST */
  			mpc_ast_print(r.output);
  			
  			mpc_ast_t* a = r.output;
  			
  			printf("Tag: %s\n", a->tag);
			printf("Contents: %s\n", a->contents);
			printf("Number of children: %i\n", a->children_num);
			
			/* Get First Child */
// 			mpc_ast_t* c0 = a->children[0];
			
			// printf("First Child Tag: %s\n", 
// 				c0->tag);
// 			printf("First Child Contents: %s\n", 
// 				c0->contents);
// 			printf("First Child Number of children: %i\n", 
// 				c0->children_num);

			long result = eval(r.output);
			printf("Result: %li\n", result);
  			
  			mpc_ast_delete(r.output);
		} else {
  			/* Otherwise Print the Error */
  			mpc_err_print(r.error);
  			mpc_err_delete(r.error);
		}
		
		free(input);
	}
	
	/* Undefine and Delete our Parsers */
	mpc_cleanup(4, Number, Operator, Expr, Lispy);
	return 0;
}

