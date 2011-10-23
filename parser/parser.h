#include "tokenizer.h"

enum ERROR{
  SYNTAX_ERROR_UNTOKENIZABLE,SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS,SYNTAX_ERROR_UNTERMINATED_INPUT
};
List* parse(List* tokens, int* depth);
List* append(List* list1, List* list2);
