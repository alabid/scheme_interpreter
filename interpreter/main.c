#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"

int main(int argc, char *argv[]) {

  List *tokens, *parseTree, *leftoverTokens = NULL;
  leftoverTokens = initializeList();
  Environment *topEnv = createTopFrame();
  int depth = 0;
  char *expression = (char *)malloc(256 * sizeof(char));
<<<<<<< HEAD
  Value* temp;
  while (fgets(expression, 256, stdin)) {
=======

  
while (fgets(expression, 256, stdin)) {
>>>>>>> 65acc69650eb927aee49620049fbf7c4f7f25415
    
     tokens = append(leftoverTokens, tokenize(expression)); 
     
     if (!tokens) {
       leftoverTokens->head = NULL;
       continue;
       }
    
     parseTree = parse(tokens,&depth);   
     
     if (depth < 0) {
       printf("syntax error. Too many close parentheses.\n");   // Too many close parentheses. 
       cleanup(tokens->head);
       //free(parseTree);
       free(leftoverTokens);
       free(tokens);
       return SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS;
     } else if (depth > 0) {
       // There are more open parens than close parens, so these tokens are saved as leftovers. We partially generate a parse tree now.
       leftoverTokens->head = tokens->head;
       
      } else {
       if (parseTree && parseTree->head){
	 //printf("going to print parse tree: ");
	 //printValue(parseTree->head);
	 //printf("\n");
	 //printf("going to print parse tree again: ");
	 //printValue(deepCopyList(parseTree->head));
	 //printf("\n");
	 temp = eval(parseTree->head,topEnv);
	 if (temp){
	   printValue(temp);
	   printf("\n");
	 }
	 
	 //leftoverTokens->head = tokens->head;
	 free(parseTree);
	 // cleanup(leftoverTokens->head);
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
  printf("cleaning memory\n");
  destroy(leftoverTokens);
  free(tokens);
  free(expression);
  
  return -1;
}
