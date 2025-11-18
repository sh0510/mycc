#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

//tokenize
//Token kind
typedef enum{
	TK_RESERVED,
	TK_IDENT,
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

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_token(Token *token, char *fmt, ...);

bool equal(Token *token, char *op);
Token *expect(Token *token, char *op);
int expect_number(Token **rest, Token *token);
bool expect_eof(Token *token);
Token *tokenize(char *p);


//parse
typedef enum{
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_EQ, // ==
	ND_NE, // !=
	ND_LT, // <
	ND_LE, // <=
	ND_ASSIGN, // =
	ND_EXPRSTMT, // Expression Statement
	ND_LVAR, // Variable
	ND_NUM, // Integer
} NodeKind;

typedef struct Node Node;
struct Node{
	NodeKind kind;
	Node *next;
	Node *lhs; // Left-hand side
	Node *rhs; // Right-hand side
	char name; // ND_VAR
	int val; // ND_NUM
};

extern Node *parse(Token *token);

//codegen
extern void codegen(Node *node);


//debug
void print_tokens(Token *token);
void print_nodes(Node *node);