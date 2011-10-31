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
      // ignore the close parent.
      pop(tokens); 
      tempList->head = NULL;
  
      while (stack->head && stack->head->cons->car->type!=openType){
	push(tempList, pop(stack));
      }
      if (!stack->head){
	*depth = -1;
	free(tempList);
	free(stack);	
	return NULL;
      } 

      // if the depth is zero and there is no token left, we finish parsing. So we can return the list.
      if (*depth == 0 && !(tokens->head)){
	pop(stack); // ignore the open parenthese.
	push(stack, tempList->head);
	tokens->head = stack->head;
	free(stack);

	return tempList;
      }
      
      // ignore the open paren.
      pop(stack);
      push(stack, tempList->head);
    }
    else{
      push(stack, pop(tokens));
    }
  }
  
  tokens->head = stack->head;
  free(tempList);
  free(stack);
  
  return tokens;
}

/*
  This function accepts the first list in the reverse order, and the second list in order. 
  It modifies the lists in place and returns list2 as the final list.
*/
List* append(List* list1, List* list2){
  if (!list1){
    return list2;
  }
  if (!list2){
    list2 = initializeList();
  }
  while (list1->head){
    push(list2, pop(list1));
  }
  return list2;
}

/*
  This returns the car of a value (that is a list)
 */
Value *car(Value *value) {
  if (value->type == cellType){ 
    return value->cons->car;
  }else{
    return NULL;
  }
}
/*
  This returns the cdr of a value (that is a list)
*/
Value *cdr(Value *value) {
  if (value->type == cellType){
    return value->cons->cdr;
  }else{ 
    return NULL;
  }
}

/*
  This function accepts a Value that is the head of the list, and prints out the list.
*/
void printValue(Value* value){
  if (value && value->type == cellType){
    printf("(");
    Value *curValue = value;
    while (curValue){
      switch (curValue->cons->car->type)
	{
	case booleanType:
	  if(curValue->cons->car->boolValue){
	    printf("#t");
	  }
	  else{
	    printf("#f");
	  }
	  break;
	case integerType:
	  printf("%d",curValue->cons->car->intValue);
	  break;
	case floatType:
	  printf("%lf",curValue->cons->car->dblValue);
	  break;
	case stringType:
	  printf("%s",curValue->cons->car->stringValue);
	  break;
	case symbolType:
	  printf("%s",curValue->cons->car->symbolValue);
	  break;
	case openType:
	  printf("(");
	  break;
	case closeType:
	  printf(")");
	  break;
	case cellType:
	  printValue(curValue->cons->car);
	  break;
	case closureType:
	case primitiveType:
	  printf("#<procedure>");
	  break;
	default:
	  break;
	}
      if (curValue->cons->cdr){
	printf(" ");
      } 
      curValue = curValue->cons->cdr;
    } 
    printf(")");
  }
}


