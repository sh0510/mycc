#include "mycc.h"

//Functions that express token nodes or handle a token.

static Node *new_node(NodeKind kind){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	return node;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs){
	Node *node = new_node(kind);
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

static Node *new_unary(NodeKind kind, Node *expr){
	Node *node = new_node(kind);
	node->lhs = expr;
	return node;
}
//*node = calloc(1, sizeof(Node));
//	node->kind = ND_NUM;
//	node->val = val;
//	return node;
//}


static Node *stmt(Token **rest, Token *token);
static Node *exprstmt(Token **rest, Token *token);
static Node *expr(Token **rest, Token *token);
static Node *equality(Token **rest, Token *token); // -> "==", "!="
static Node *relational(Token **rest, Token *token); // -> "<", "<=", ">", ">="
static Node *add(Token **rest, Token *token);
static Node *mul(Token **rest, Token *token);
static Node *unary(Token **rest, Token *token);
static Node *primary(Token **rest, Token *token);

// stmt = exprstmt
static Node *stmt(Token **rest, Token *token){
	return exprstmt(rest, token);
}

// ???????????
// exprstmt = expr ";"
static Node *exprstmt(Token **rest, Token *token){
	Node *node = new_unary(ND_EXPRSTMT, expr(&token, token));
	*rest = expect(token, ";");
	return node;
}

static Node *new_num(int val){
	Node *node = new_node(ND_NUM);
	node->val = val;
	return node;
}

//Node *new_node_num(int val){
//	Node n **rest, Token *token){
	//c.f. return new_binary(ND_SUB, new_num(0), unary(rest, token->next));
	//Node *node = new_unary(ND_EXPRSTMT, expr(&token, token));
	//Node *node = expr(&token, token);
	//node = new_binary(ND_EXPRSTMT, new_node(node->kind), unary(rest, token->next));

//}

// expr = equality
static Node *expr(Token **rest, Token *token){
	return equality(rest, token);
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality(Token **rest, Token *token){
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
static Node *relational(Token **rest, Token *token){
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
static Node *add(Token **rest , Token *token){
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
static Node *mul(Token **rest , Token *token){
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
static Node *unary(Token **rest, Token *token){
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
static Node *primary(Token **rest, Token *token){
	if(equal(token, "(")){
		Node *node = expr(&token, token->next);
		*rest = expect(token, ")");
		return node;
	}
	return new_num(expect_number(rest, token));
}


/////////////////////////////////////////////////////////////

// program = stmt*
Node *parse(Token *token){
	Node head = {};
	Node *node = &head;
	for(;token->kind != TK_EOF;){
		node->next = stmt(&token, token);
		node = node->next;
	}

	return head.next;
}
