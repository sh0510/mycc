#include "mycc.h"

//Functions that generates assembly code.

static void gen_lvar(Node *node){
	if(node->kind != ND_LVAR){
		error("not variable");
	}
	else{
		// stack for variable a-z are pre-allocated.
		// When assigning variable, arrange stack address (rbp-i*8)
		int offset = (node->name - 'a' + 1)*8;
		printf("	mov rax, rbp\n");
		printf("	sub rax, %d\n", offset);
		printf("	push rax\n");
	}
}

static void gen(Node *node){
	//handling leaf nodes and branch nodes separately 
	//handling leaf nodes that don't have lhs/rhs.
	switch(node->kind){
		case ND_NUM:
			//We can choice which to use for store value with stack or register.(I believe.)
			printf("	push %d\n", node->val);
			//printf("	mov rax, %d\n", node->val);
			return;
		case ND_LVAR:
			gen_lvar(node);
			printf("	pop rax\n");
			printf("	mov rax, [rax]\n");
			printf("	push rax\n");
			return;
		case ND_ASSIGN:
			gen_lvar(node->lhs);
			gen(node->rhs);
			printf("	pop rdi\n");
			printf("	pop rax\n");
			printf("	mov [rax], rdi\n");
			printf("	push rdi\n");
			return;
	}

	//handling branch nodes that have lhs, rhs.
	gen(node->lhs);
	gen(node->rhs);
	printf("	pop rdi\n");
	printf("	pop rax\n");
	switch(node->kind){
		case ND_ADD:
			printf("	add rax, rdi\n");
			break;
		case ND_SUB:
			printf("	sub rax, rdi\n");
			break;
		case ND_MUL:
			printf("	imul rax, rdi\n");
			break;
		case ND_DIV:
			printf("	cqo\n");
			printf("	idiv rdi\n");
			break;
		case ND_EQ:
			printf("	cmp rax, rdi\n");
			printf("	sete al\n");
			//load al to rax with 0 extension. Clearing higher 56 bits. Lower 16 bits are al.
			printf("	movzb rax, al\n");
			break;
		case ND_NE:
			printf("	cmp rax, rdi\n");
			// alternatively, ,,,
			printf("	setne al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_LT:
			printf("	cmp rax, rdi\n");
			printf("	setl al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_LE:
			printf("	cmp rax, rdi\n");
			printf("	setle al\n");
			printf("	movzb rax, al\n");
			break;
	}
	
	printf("	push rax\n");
	return;
}

static void gen_exprtstmt(Node *node){
	if(node->kind == ND_EXPRSTMT){
		gen(node->lhs);
		return;
	}
	error("invalid statement");
}

///////////////////////////////////////////////////////////////////


void codegen(Node *node){
    printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	//allocate stack
	printf("	push rbp\n");
	printf("	mov rbp, rsp\n");
	printf("	sub rsp, %d\n", ('z'-'a'+1)*8);

	for(Node *n=node; n; n=n->next){
		gen_exprtstmt(n);
		//gen(n);
		//Why this pop is needed here? And, don't we need the "ret" call?
		printf("	pop rax\n");
	}
	//gen(node);
    //printf("	pop rax\n");

	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");

    return;
}