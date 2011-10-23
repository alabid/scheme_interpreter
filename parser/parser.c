#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


List* parse(List* tokens, int* depth){
  List* stack = initializeList();
  List* tempList = initializeList();
 
  if (!tokens){
    free(stack);
    free(tempList);
    return NULL;
  }
  *depth = 0;
  while (tokens->head){
    if (tokens->head->cons->car->type == openType){
	(*depth)++;
	push(stack, pop(tokens));
    }else if (tokens->head->cons->car->type == closeType){
      (*depth)--;
      // push the close paren into the temp list.
      tempList->head = pop(tokens);
      while (stack->head && stack->head->cons->car->type!=openType){
	push(tempList, pop(stack));
      }
      if (!stack->head){
	*depth = -1;
	//free(tempList);
	//free(stack);
	return NULL;
      } 
      if (*depth == 0){
	tokens->head = NULL;
	push(tempList, pop(stack));

	return tempList;
      }
      
      // push the open paren into the temp list.
      push(tempList, pop(stack));
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

List* append(List* list1, List* list2){
  if (!list1){
    return list2;
  }
  while (list1->head){
  push(list2, pop(list1));
  }
  return list2;
}

void printValue(Value* value){
if (value && value->type == cellType){
    Value *curValue = value;
    Value* second;
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
	  second = curValue->cons->car->cons->car;
	  // printf("\ncurrent type is %d\n",curValue->type);
	  //printf("\ncurrent->cons->car type is %d\n",second->type);
	  /*while (second){
	    printf("\nsecond->cons->car type is %d\n",second->cons->car->type);
	    if (second->cons->car->type == openType){
	      printf("(");
	    }else if (second->cons->car->type ==closeType){
	      printf("(");
	    }else if (second->cons->car->type ==symbolType){
	      printf("%s",second->cons->car->symbolValue);
	    }
	    second = second->cons->cdr;
	    }*/
	  //printf("hello world");
	  printValue(curValue->cons->car);
	  break;
	default:
	  break;
	}
      printf(" ");
      curValue = curValue->cons->cdr;
    }
    printf("\n");
 }
}


