#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"

/*
  main.c --
  Tests our tokenize function.
*/

int main(int argc, char *argv[]) {
  List *list; 
  char* expression = (char *) malloc(sizeof(char) *256);
  
  while (fgets(expression, 256, stdin)) {
    list = tokenize(expression);
    printToken(list);

    // destroy and clean up
    destroy(list);
  }
  free(expression);
  return 0;
}
