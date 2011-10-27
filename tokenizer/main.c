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
    // printf("\n===========START=============\n");
    list = tokenize(expression);
    if (list){
      printToken(list->head);
    }
  }
    // destroy and clean up
    destroy(list);
    // printf("\n=============END===========\n");
  }
  free(expression);
  return 0;
}
