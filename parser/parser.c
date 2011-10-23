#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


List* parse(List* tokens, int* depth){
  // List* parseTree = initializeList();
  List* stack = initializeList();
  List* tempList = initializeList();
  Value* newValue;
  *depth = 0;
  if (!tokens){
    free(stack);
    free(tempList);
    return NULL;
  }
  while (tokens->head){
    if (tokens->head->cons->car->type!=closeType){
      push(stack, tokens->head->cons->car);
      if (tokens->head->cons->car->type==openType){
	(*depth)++;
      }
    }
    else{
      (*depth)--;
      // push the close paren into the temp list.
      tempList->head = tokens->head->cons->car;
      //push(stack, tokens->head->cons->car);
      while (stack->head && stack->head->cons->car->type!=openType){
	push(tempList, pop(stack));
      }
      if (!(stack->head)){
	*depth = -1;
	free(tempList);
	tokens->head = stack->head;
	free(stack);
	return tokens;
      }
      // push the open paren into the temp list.
      push(tempList, pop(stack));
      newValue = (Value *)malloc(sizeof(Value));
      newValue->type = cellType;
      newValue->cons = (ConsCell *)malloc(sizeof(ConsCell));
      newValue->cons->car = tempList->head;
      newValue->cons->cdr = tokens->head->cons->cdr;
      push(stack, newValue);
    }
    tokens->head = tokens->head->cons->cdr;  
  }
  
  tokens->head = stack->head;
  free(tempList);
  free(stack);
  return tokens;
}

List* append(List* list1, List* list2){
  while (list1->head){
    push(list2, pop(list1));
  }
  free(list1);
  return list2;
}
