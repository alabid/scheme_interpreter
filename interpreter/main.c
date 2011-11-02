# include <stdio.h>
# include <stdlib.h>
# include "interpreter.h"
// test the interpreter
int main() {
  List *tokens, *parseTree, *leftoverTokens = NULL;
  Environment *topEnv = createTopFrame();
  leftoverTokens = initializeList();
  int depth = 0;
  char *expression = (char *)malloc(256 * sizeof(char));
 
  while (fgets(expression, 256, stdin)) {
    
     tokens = append(leftoverTokens, tokenize(expression)); 
     
     if (!tokens) {
       leftoverTokens->head = NULL;
       continue;
       }
    
     parseTree = parse(tokens,&depth);   
     
     if (depth < 0) {
       printf("syntax error. Too many close parentheses.\n");   // Too many close parentheses. 
       return SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS;
     } else if (depth > 0) {
       // There are more open parens than close parens, so these tokens are saved as leftovers. We partially generate a parse tree now.
       leftoverTokens->head = tokens->head;
       
      } else {
       if (parseTree && parseTree->head){
	 //printValue(parseTree->head);
	 printValue(eval(parseTree->head,topEnv));
	 printf("\n");
	 cleanup(leftoverTokens->head);
       }
     }
  }
  if (leftoverTokens->head) {
    printf("syntax error. Too few close parentheses\n");   // Too few close parens at end of input. 
    destroy(leftoverTokens);   
    free(tokens);
    return SYNTAX_ERROR_UNTERMINATED_INPUT;
  }
  // clean up memory 
  destroy(leftoverTokens);
  free(tokens);
  free(expression);
  
  return 0;
}
