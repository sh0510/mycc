#include "mycc.h"

//Functions that break input into tokens.

static char *user_input;

static void verror_at(char *loc, char *fmt, va_list ap){
	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	// print pos th number of " " by '%*s" format specifier.
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

static Token *new_token(TokenKind kind, Token *cur, char *str, int len){
	Token *token = calloc(1, sizeof(Token));
	token->kind = kind;
	token->str = str;
	token->len = len;
	cur->next = token;

	return token;
}

static bool startswith(char *p, char *q){
	return memcmp(p, q, strlen(q)) == 0;
}


///////////////////////////////////////////////////////////

void error(char *fmt, ...){
	va_list ap;
	va_start(ap,fmt);
	vfprintf(stderr,fmt,ap);
	fprintf(stderr,"\n");
	exit(1);
}

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

//Is it possible to make the expect function group into a single abstract function like codegen() and parse()?
//It would be better to have less coupling between parse.c and tokenize.c.
Token *expect(Token *token, char *op){
	if(equal(token, op)==0){
		//error("Not '%c'", op);
		//error_at(token->str, "Not '%c'", op);
		//error_at(token->str, "expected \"%s\"", op);
		error_token(token, "expected \"%s\"", op);
	}
	//return token = token->next;
	return token->next;
}

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

bool expect_eof(Token *token){
	return token->kind == TK_EOF;
}

Token *tokenize(char *p){
	user_input = p;

	Token head;
	head.next = NULL;
	Token *cur = &head;

	while(*p){
		if(isspace(*p)){
			p++;
		}
		//multi characters operator
		else if(
			startswith(p, "==") ||
			startswith(p, "!=") ||
			startswith(p, "<=") ||
			startswith(p, ">=")
		){
			cur->next = new_token(TK_RESERVED, cur, p, 2);
			cur = cur->next;
			p+=2;
		}
		//ispunct()???
		//if(*p=='+' | *p=='-' | *p=='*' | *p=='/' | *p=='(' | *p==')' ){
		else if(strchr("+-*/()<>;",*p)){
			//cur = new_token(TK_RESERVED, cur, p++);
			//cur = new_token(TK_RESERVED, cur, p++, 1);
			cur->next = new_token(TK_RESERVED, cur, p, 1);
			cur = cur->next;
			p+=1;
		}
		else if(isdigit(*p)){
			//single operator
			cur->next = new_token(TK_NUM, cur, p, 0);
			cur = cur->next;
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p-q;
		}
		else{
			//error("Cannot tokenize");
			error_at(p, "Cannot tokenize");
		}
		//unreachable()
	}
	cur->next = new_token(TK_EOF, cur, p, 0);
	cur = cur->next;

	return head.next;
}

