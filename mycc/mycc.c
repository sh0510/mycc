#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

//Token kind
typedef enum{
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
} TokenKind;

typedef struct Token Token;

//Token type
struct Token{
	TokenKind kind;
	Token *next;
	int val;
	char *str;
	int len; // Length of Token
};

Token *token;


//
char *user_input;

//
void error_at(char *loc, char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	// print pos th number of " " by '%*s" format specifier.
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

//
void error(char *fmt, ...){
	va_list ap;
	va_start(ap,fmt);
	vfprintf(stderr,fmt,ap);
	fprintf(stderr,"\n");
	exit(1);
}



//
bool consume(char *op){
	if(
		token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len)
	){
		return false;
	}
	token = token->next;
	
	return true;
}

//
void expect(char *op){
	if(
		token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len)
	){
		//error("Not '%c'", op);
		error_at(token->str, "Not '%c'", op);
	}
	token = token->next;
}

//
int expect_number(){
	if(token->kind != TK_NUM){
		//error("Not a number");
		error_at(token->str, "Not a number");
	}

	int val = token->val;
	token = token->next;
	return val;
}

//
bool at_eof(){
	return token->kind == TK_EOF;
}

//
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;

	return tok;
}

//
Token *tokenize(char*p){
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while(*p){
		if(isspace(*p)){
			p++;
			continue;
		}

		//if(*p=='+' | *p=='-' | *p=='*' | *p=='/' | *p=='(' | *p==')' ){
		if(strchr("+-*/()",*p)){
			//cur = new_token(TK_RESERVED, cur, p++);
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}

		if(isdigit(*p)){
			//single operator
			cur = new_token(TK_NUM, cur, p, 0);
			char *p_tmp = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p-p_tmp;
			continue;
		}
		
		//error("Cannot tokenize");
		error_at(token->str, "Cannot tokenize");
	}

	new_token(TK_EOF, cur, p, 0);

	return head.next;
}


//
typedef enum{
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_NUM, // Integer
} NodeKind;

typedef struct Node Node;

//
struct Node{
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
};

//
Node *new_node(NodeKind kind){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	return node;
}

//
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs){
	Node *node = new_node(kind);
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

//
Node *new_num(int val){
	Node *node = new_node(ND_NUM);
	node->val = val;
	return node;
}

//
Node *expr();
Node *mul();
Node *primary();
Node *unary();


//
//Node *new_node_num(int val){
//	Node *node = calloc(1, sizeof(Node));
//	node->kind = ND_NUM;
//	node->val = val;
//	return node;
//}

// parimary = num | "(" exprt ")"
Node *primary(){
	if(consume("(")){
		Node *node = expr();
		expect(")");
		return node;
	}

	return new_num(expect_number());
}

// mul = unary ("*" unary | "/" unary)*
Node *mul(){
	Node *node = unary();
	for(;;){
		if(consume("*")){
			node = new_binary(ND_MUL, node, unary());
		}
		else if(consume("/")){
			node = new_binary(ND_DIV, node, unary());
		}
		else{
			return node;
		}
	}
}

// exprt = mul ("+" mul | "-" mul)*
Node *expr(){
	Node *node = mul();
	for(;;){
		if(consume("+")){
			node = new_binary(ND_ADD, node, mul());
		}
		else if(consume("-")){
			node = new_binary(ND_SUB, node, mul());
		}
		else{
			return node;
		}
	}
}


//
void gen(Node *node){
	if(node->kind == ND_NUM){
		printf("	push %d\n", node->val);
		return;
	}

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
	}
	printf("	push rax\n");
}

//
Node *unary(){
	if(consume("+")){
		//return primary();
		return unary();
	}
	if(consume("-")){
		//return new_node(ND_SUB, new_node_num(0), primary());
		return new_binary(ND_SUB, new_num(0), unary());
	}
	return primary();
}


//
int main(int argc, char **argv){
	if(argc!=2){
		fprintf(stderr, "Wrong argc\n");
		return 1;
	}

	user_input = argv[1];
  token = tokenize(user_input);
	Node *node = expr();

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	gen(node);

	printf("	pop rax\n");
	printf("	ret\n");

	return 0;
}	
