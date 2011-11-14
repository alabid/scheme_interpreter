/* By Jie Lin, Kan Wang, Daniel Alabi, Yichen Shen.
   CS 251, Fall 2011
*/

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


/* 
   This function parses a list of tokens. If the tokens is incomplete, the parse tree is incomplete. 
   "depth" determines whether the parse tree is complete.
*/
List* parse(List* tokens, int* depth){
  if (!tokens){
    *depth = 0;
    return NULL;
  }
  
  List* stack = initializeList();
  List* tempList = initializeList();
  *depth = 0;
  while (tokens->head){
   
    if (tokens->head->cons->car->type == openType){
	(*depth)++;
	push(stack, pop(tokens));
    }else if (tokens->head->cons->car->type == closeType){
      (*depth)--;
      // pop the close parent.
      
      tempList->head = NULL;
      push(tempList,pop(tokens));
      
      //printList(tempList->head);
      
      
      while (stack->head && stack->head->cons->car->type!=openType){
	push(tempList, pop(stack));
      }
      if (!stack->head){
	*depth = -1;
	destroy(tempList);
	free(stack);
	return NULL;
      } 
      if (getFirst(tempList->head) && getFirst(tempList->head)->type == closeType){
	free(pop(tempList));
	free(pop(stack));
	Value *value= (Value *)malloc(sizeof(Value));
	value->type = nullType;
	push(tempList, value);
	
      }else{
	push(tempList,pop(stack)); // ignore the open parenthese.
      }
      // 
      // if the depth is zero and there is no token left, we finish parsing. So we can return the list.
      if (*depth == 0){
	while (stack->head){
	  push(tempList, pop(stack));
	}

	free(stack);	

	return tempList;
      }
      push(stack, tempList->head);
    }else{
      push(stack, pop(tokens));
    }
  }
  
  tokens->head = stack->head;
  free(tempList);
  free(stack);
  reverse(tokens);
  return tokens;
}

/*
  This function accepts the first list in order, and the second list in order. 
  It modifies the lists in place and returns list2 as the final list.
*/
List* append(List* list1, List* list2){
  if (!list1){
    return list2;
  }
  if (!list2){
    list2 = initializeList();
  }
  reverse(list1);
  while (list1->head){
    push(list2, pop(list1));
  }
  return list2;
}


