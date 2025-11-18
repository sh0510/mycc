#include "mycc.h"

int main(int argc, char **argv){
	if(argc!=2){
		fprintf(stderr, "Wrong argc\n");
		return 1;
	}

	Token *token = tokenize(argv[1]);
#ifdef DEBUG_PRINT_TOKENS
	print_tokens(token);
#endif
	Node *node = parse(token);
	codegen(node);

	return 0;
}	
