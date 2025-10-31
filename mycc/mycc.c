#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

//Token kind
typedef enum{
	TK_RESERVED,
	TK_PUNCT,
	TK_NUM,
	TK_EOF,
} TokenKind;

typedef struct Token Token;
//Token type
struct Token{
	TokenKind kind;
	Token *next;
	int val; // Used as value if kind==TK_NUM
	char *str; // Token location
	int len; // Length of Token
};

char *user_input;


//
void error(char *fmt, ...){
	va_list ap;
	va_start(ap,fmt);
	vfprintf(stderr,fmt,ap);
	fprintf(stderr,"\n");
	exit(1);
}
//
void verror_at(char *loc, char *fmt, va_list ap){
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
void error_at(char *loc, char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	verror_at(loc, fmt, ap);
}
void error_token(Token *token, char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	//verror_at(token->str, fmt, ap);
	error_at(token->str, fmt, ap);
}




//
bool equal(Token *token, char *op){
	if(
		token->kind != TK_RESERVED ||
		//Alternatively, op[token->len]!='\0'
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len)
	){
		return false;
	}
	return true;
}

//
Token *expect(Token *token, char *op){
	if(equal(token,op)==0){
		//error("Not '%c'", op);
		//error_at(token->str, "Not '%c'", op);
		//error_at(token->str, "expected \"%s\"", op);
		error_token(token, "expected \"%s\"", op);
	}
	return token = token->next;
}
//
int expect_number(Token **rest, Token *token){
	if(token->kind != TK_NUM){
		//error("Not a number");
		//error_at(token->str, "Not a number");
		error_token(token, "Not a number");
	}

	int val = token->val;
	*rest = token->next;
	return val;
}
//
bool expect_eof(Token *token){
	return token->kind == TK_EOF;
}

//
Token *new_token(TokenKind kind, Token *cur, char *str, int len){
	Token *token = calloc(1, sizeof(Token));
	token->kind = kind;
	token->str = str;
	token->len = len;
	cur->next = token;

	return token;
}

//
bool startswith(char *p, char *q){
	return memcmp(p, q, strlen(q)) == 0;
}

//
Token *tokenize(){
	char *p = user_input;
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while(*p){
		if(isspace(*p)){
			p++;
			continue;
		}

		//multi characters operator
		if(
			startswith(p, "==") ||
			startswith(p, "!=") ||
			startswith(p, "<=") ||
			startswith(p, ">=")
		){
			cur = new_token(TK_RESERVED, cur, p, 2);
			p+=2;
			continue;
		}
		//ispunct()???
		//if(*p=='+' | *p=='-' | *p=='*' | *p=='/' | *p=='(' | *p==')' ){
		else if(strchr("+-*/()<>",*p)){
			//cur = new_token(TK_RESERVED, cur, p++);
			//cur = new_token(TK_RESERVED, cur, p++, 1);
			cur = new_token(TK_RESERVED, cur, p, 1);
			p+=1;
		}
		else if(isdigit(*p)){
			//single operator
			cur = new_token(TK_NUM, cur, p, 0);
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p-q;
		}
		else{
			//error("Cannot tokenize");
			error_at(p, "Cannot tokenize");
		}
	}
	cur = new_token(TK_EOF, cur, p, 0);

	return head.next;
}


//
typedef enum{
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_EQ, // ==
	ND_NE, // !=
	ND_LT, // <
	ND_LE, // <=
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
Node *expr(Token **rest, Token *token);
Node *mul(Token **rest, Token *token);
Node *primary(Token **rest, Token *token);
Node *unary(Token **rest, Token *token);
Node *equality(Token **rest, Token *token); // -> "==", "!="
Node *relational(Token **rest, Token *token); // -> "<", "<=", ">", ">="
Node *add(Token **rest, Token *token);

//
//Node *new_node_num(int val){
//	Node *node = calloc(1, sizeof(Node));
//	node->kind = ND_NUM;
//	node->val = val;
//	return node;
//}

// expr = equality
Node *expr(Token **rest, Token *token){
	return equality(rest, token);
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality(Token **rest, Token *token){
	Node *node = relational(&token, token);
	for(;;){ // )*
		if(equal(token, "==")){ // "==" relational
			node = new_binary(ND_EQ, node, relational(&token, token->next));
		}
		else if(equal(token, "!=")){ // "!=" relational
			node = new_binary(ND_NE, node, relational(&token, token->next));
		}
		else{
			*rest = token;
			return node;
		}
	}	
}

//relational = add ("" add | "<=" add | ">" add | ">=" add)*
Node *relational(Token **rest, Token *token){
	Node *node = add(&token, token);
	for(;;){
		if(equal(token, "<")){
			node = new_binary(ND_LT, node, add(&token, token->next));
		}
		else if(equal(token, "<=")){
			node = new_binary(ND_LE, node, add(&token, token->next));
		}
		else if(equal(token, ">")){
			node = new_binary(ND_LT, add(&token, token->next), node);
		}
		else if(equal(token, ">=")){
			node = new_binary(ND_LE, add(&token, token->next), node);
		}
		else{
			*rest = token;
			return node;
		}
	}
}

//add = mul ("+" mul | "-" mul)*
Node *add(Token **rest , Token *token){
	Node *node = mul(&token, token);
	for(;;){
		if(equal(token, "+")){
			node = new_binary(ND_ADD, node, mul(&token, token->next));
		}
		else if(equal(token, "-")){
			node = new_binary(ND_SUB, node, mul(&token, token->next));
		}
		else{
			*rest = token;
			return node;
		}
	}
}

// mul = unary ("*" unary | "/" unary)*
Node *mul(Token **rest , Token *token){
	Node *node = unary(&token, token);
	for(;;){
		if(equal(token, "*")){
			node = new_binary(ND_MUL, node, unary(&token, token->next));
		}
		else if(equal(token, "/")){
			node = new_binary(ND_DIV, node, unary(&token, token->next));
		}
		else{
			*rest = token;
			return node;
		}
	}
}

// //unary = ("+" | "-")? primary
//unary = ("+" | "-")? unary | primary
Node *unary(Token **rest, Token *token){
	if(equal(token, "+")){
		//return primary(&token, token);
		// eliminate sigle +
		return unary(rest, token->next);
	}
	//not "else if"???
	if(equal(token, "-")){
		//Alternatively, use neg CPU instruction c.f. case ND_NEG of https://github.com/rui314/chibicc/commit/bf9ab52860c1cbbeeca40df515468f42300ff429#diff-a0cb465674c1b01a07d361f25a0ef2b0214b7dfe9412b7777f89add956da10ec
		//return new_node(ND_SUB, new_node_num(0), primary(&token, token));
		return new_binary(ND_SUB, new_num(0), unary(rest, token->next));
	}
	return primary(rest, token);
}

// // parimary = num | "(" exprt ")"
// primary = "(" expr ")" | num
Node *primary(Token **rest, Token *token){
	if(equal(token, "(")){
		Node *node = expr(&token, token->next);
		*rest = expect(token, ")");
		return node;
	}
	return new_num(expect_number(rest, token));
}


//
void gen(Node *node){
	switch(node->kind){
		case ND_NUM:
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

//
int main(int argc, char **argv){
	if(argc!=2){
		fprintf(stderr, "Wrong argc\n");
		return 1;
	}

	user_input = argv[1];
	Token *token = tokenize();
	Node *node = expr(&token, token);

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	gen(node);

	printf("	pop rax\n");
	printf("	ret\n");

	return 0;
}	
