#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char *argv[]) {

  List *tokens, *parseTree, *leftoverTokens = NULL;
  leftoverTokens = initializeList();
  int depth = 0;
  char *expression = (char *)malloc(256 * sizeof(char));
  /*
  fgets(expression, 256, stdin);
  tokens = append(leftoverTokens, tokenize(expression));
  printf("===================print before parse\n");
  printValue(tokens->head);
  parseTree = parse(tokens,&depth);
  leftoverTokens = tokens;
  printf("===================print after parse\n");
  printValue(tokens->head);
  fgets(expression, 256, stdin);
  tokens = tokenize(expression);
  tokens = append(leftoverTokens, tokens);
  printf("===================print before parse2\n");
  printValue(tokens->head);
  parseTree = parse(tokens,&depth);
  printf("===================print after parse 2\n");
  printValue(tokens->head);
  printf("===================\n");
  // printToken(tokens->head);
  
  tokens = append(leftoverTokens, tokens);
  leftoverTokens = tokens;
  printValue(tokens->head);
  printf("================\n");

  printValue(tokens->head);
  fgets(expression, 256, stdin);
  tokens = tokenize(expression);
  tokens = append(leftoverTokens, tokens);
  printf("=================\n");
  parse(tokens,&depth);
  //printToken(parse(tokens,&depth)->head);
  
  printValue(tokens->head);
    
  //printToken(parse(tokens,&depth)->head);
  /*
  
  while (fgets(expression, 256, stdin)) {
     printf("hello\n"); 
     tokens = append(leftoverTokens, tokenize(expression));  // actually, my tokenize does both steps:
  
     // tokenize(expression, leftoverTokens) 
      if (!tokens) {
	printf("syntax error1\n");   // This line is untokenizable. 
	destroy(leftoverTokens);   
	destroy(tokens);
	destroy(parseTree);
	return SYNTAX_ERROR_UNTOKENIZABLE;
      }
      parseTree = parse(tokens,&depth);    // My parse() takes a list of tokens and an integer pointer, and 
      
      if (depth < 0) {
	printf("syntax error2\n");   // Too many close parentheses. 
	return SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS;
      } else if (depth > 0) {
	// There are more open parens than close parens, so these tokens are saved as leftovers.
	leftoverTokens = tokens;
	
      } else {
	if (parseTree){
	  // printValue(parseTree->head);
	}else{
	   printf("syntax error2.5\n"); 
	}
	
	//cleanup(leftoverTokens->head);
	//leftoverTokens = NULL;
      }
  }
  if (leftoverTokens) {
    printf("syntax error3\n");   // Too few close parens at end of input. 
    destroy(leftoverTokens);   
    //destroy(tokens);
    
    //destroy(parseTree);
    return SYNTAX_ERROR_UNTERMINATED_INPUT;
  }
  // clean up memory 
  destroy(tokens);
  destroy(parseTree);
  
   
  free(expression);
  */
  return -1;
}
