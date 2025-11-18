#include "mycc.h"

// トークンリストを標準エラー出力にダンプする
void print_tokens(Token *token) {
  fprintf(stderr, "--- Tokens ---\n");
  int i = 0;
  for (Token *t = token; t->kind != TK_EOF; t = t->next) {
    fprintf(stderr, "  [%d] ", i++);
    
    switch (t->kind) {
      case TK_NUM:
        fprintf(stderr, "TK_NUM (val: %d)\n", t->val);
        break;
      case TK_IDENT:
        fprintf(stderr, "TK_IDENT (str: %c)\n", *t->str);
        break;
      case TK_RESERVED:
        // t->str は1文字の演算子を指している
        fprintf(stderr, "TK_RESERVED (str: '%.1s')\n", t->str);
        break;
      default:
        fprintf(stderr, "Unknown Token Kind\n");
    }
  }
  fprintf(stderr, "  [%d] TK_EOF\n", i);
  fprintf(stderr, "----------------\n");
}

static void print_node_recursive(Node *node, char *prefix, bool is_last) {
  if (!node) {
    return;
  }

  // display node connecting lines and node information
  fprintf(stderr, "%s", prefix);
  //fprintf(stderr, is_last ? "└── " : "├── ");
  fprintf(stderr, is_last ? "|__ " : "|__ ");

  switch (node->kind) {
    case ND_ADD:      fprintf(stderr, "ND_ADD (+)\n"); break;
    case ND_SUB:      fprintf(stderr, "ND_SUB (-)\n"); break;
    case ND_MUL:      fprintf(stderr, "ND_MUL (*)\n"); break;
    case ND_DIV:      fprintf(stderr, "ND_DIV (/)\n"); break;
    case ND_EQ:       fprintf(stderr, "ND_EQ (==)\n"); break;
    case ND_NE:       fprintf(stderr, "ND_NE (!=)\n"); break;
    case ND_LT:       fprintf(stderr, "ND_LT (<)\n"); break;
    case ND_LE:       fprintf(stderr, "ND_LE (<=)\n"); break;
    case ND_ASSIGN:   fprintf(stderr, "ND_ASSIGN (=)\n"); break;
    case ND_EXPRSTMT: fprintf(stderr, "ND_EXPRSTMT (;)\n"); break;
    case ND_LVAR:     fprintf(stderr, "ND_LVAR (name: %c)\n", node->name); break;
    case ND_NUM:      fprintf(stderr, "ND_NUM (val: %d)\n", node->val); break;
    default:          fprintf(stderr, "Unknown Node Kind (%d)\n", node->kind);
  }

  // create prefix for child node
  char new_prefix[512];
  snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_last ? "    " : "│   ");

  if (node->lhs) {
    // if rhs exists, lhs isn't last child
    print_node_recursive(node->lhs, new_prefix, node->rhs == NULL);
  }
  if (node->rhs) {
    // rhs is always last child
    print_node_recursive(node->rhs, new_prefix, true);
  }
}

// Dump AST to stderr
void print_nodes(Node *node) {
  fprintf(stderr, "--- AST Nodes ---\n");
  int i = 0;
  for (Node *n = node; n; n = n->next) {
    fprintf(stderr, "Statement %d:\n", i++);
    print_node_recursive(n, "", true);
  }
  fprintf(stderr, "-----------------\n");
}