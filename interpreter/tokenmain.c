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
  HashTable* table = initializeTable(32);
  
  char counter[] ="a\0";
  while (fgets(expression, 256, stdin)) {
    // printf("\n===========START=============\n");
    list = tokenize(expression);
    if (list){
      while (list->head){
	//printTable(table);
	insertItem(table, counter, pop(list));
	//autoDouble(table);
	//pop(list);
	//counter[0] +=1;
	printf("%s\n",counter);
      }
    }
  
    // destroy and clean up
    
    // printf("\n=============END===========\n");
  }
  
  //getKeys(table);
  printf("\n");
  //printValue(getValues(table)->head);
  //cleanupValue(table);
  //free(table);
  //free(list);
  free(expression);
  return 0;
}
