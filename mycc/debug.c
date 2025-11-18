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