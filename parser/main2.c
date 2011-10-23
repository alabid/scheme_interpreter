#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


int main(int argc, char *argv[]) {
   List *tokens, *parseTree, *leftoverTokens = NULL;
   leftoverTokens = initializeList();
   int depth = 0;
   char *expression = (char *)malloc(256 * sizeof(char));

   while (fgets(expression, 256, stdin)) {
      tokens = append(leftoverTokens, tokenize(expression));  /* actually, my tokenize does both steps:
                                                               * tokenize(expression, leftoverTokens) */
      if (!tokens) {
	 printf("syntax error\n");   /* This line is untokenizable. */
	 destroy(leftoverTokens);   
	 destroy(tokens);
	 destroy(parseTree);
         return SYNTAX_ERROR_UNTOKENIZABLE;
      }
      parseTree = parse(tokens,&depth);    /* My parse() takes a list of tokens and an integer pointer, and 
                                            * returns a parse tree (a list) and changes depth to be the 
                                            * number of unclosed parens encountered while parsing tokens. */
      if (depth < 0) {
	 printf("syntax error\n");   /* Too many close parentheses. */
         return SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS;
      } else if (depth > 0) {
	 /* There are more open parens than close parens, so these tokens are saved as leftovers. */
	 leftoverTokens = tokens;
	 depth = 1;
      } else {
	if (parseTree)
	  printToken(parseTree->head);
	destroy(leftoverTokens);
	leftoverTokens = NULL;
      }
   }
   printf("hello world");
   if (leftoverTokens) {
     printf("syntax error\n");   /* Too few close parens at end of input. */
     destroy(leftoverTokens);   
     destroy(tokens);
     destroy(parseTree);
     return SYNTAX_ERROR_UNTERMINATED_INPUT;
   }
   /* clean up memory */ 
   destroy(tokens);
   destroy(parseTree);
   
   
   free(expression);
   return -1;
}
