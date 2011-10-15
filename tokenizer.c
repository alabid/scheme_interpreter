
#include <stdlib.h>
#include <stdio.h>
#include "tokenizer.h"

// This function initializes a linked list. It will assign head as NULL.
void initialize(List *list){
  list->head = NULL;
}

int insertCell(List *list, Value *value){
  Value *newValue = (Value *)malloc(sizeof(Value));
  ConsCell *newCell = (ConsCell *)malloc(sizeof(ConsCell));
  newValue->type = cellType;
  newValue->cons = newCell;
  newCell->car = value;
  newCell->cdr = list->head;
  list->head = newValue;
  return 1;
}



int reverse(List *list){
  List *newList = (List *)malloc(sizeof(List));
  List *oldList;
  Value *curValue = list->head;
  initialize(newList);
  while(curValue){
    if(insertCell(newList, curValue->cons->car)){
      curValue = curValue->cons->cdr;
    }
    else{
      return 0;
    }
  }
  oldList = list;
  list = newList;
  cleanup(oldList);
  return 1;
}

void print(List* list){
  Value *curValue = list->head;
  while (curValue){
    switch (curValue->cons->car->type){
      case booleanType:
	if(curValue->cons->car->boolValue){
	  printf("True:boolean\n");
	}
	else{
	  printf("False:boolean\n");
	}
	break;
      case integerType:
	printf("%d:integer\n",curValue->cons->car->intValue);
	break;
      case floatType:
	printf("%lf:float\n",curValue->cons->car->dblValue);
	break;
      case stringType:
	printf("%s:string\n",curValue->cons->car->stringValue);
	break;
      case symbolType:
	printf("%s:symbol\n",curValue->cons->car->symbolValue);
	break;
      case openType:
	printf("(:open\n");
	break;
      case closeType:
	printf("):close\n");
	break;
      default:
	break;
      }
    curValue = curValue->cons->cdr;
  }
}

// This function deletes a cons cell from the linked list.
int deleteCell(List *list, Value *value){
  int type = value->type;
  Value* current = list->head;
  Value* storedValue;
  Value* previous;
  int found = 0;
  if (type==cellType){
    return 0; //invalid input
  }
  
  while (current){
    if (current->cons->car->type==type){
      storedValue = current->cons->car;
      switch (type){
	case booleanType:
	  if (storedValue->boolValue == value->boolValue)
	    {
	      found =1;
	    }
	  break;
	case integerType:
	  if (storedValue->intValue == value->intValue)
	    {
	      found =1;
	    }
	  break;
	case floatType:
	  if (storedValue->dblValue == value->dblValue)
	    {
	      found =1;
	    }
	  break;
	case stringType:
	  if (storedValue->stringValue == value->stringValue)
	    {
	      found =1;
	    }
	  break;
	case symbolType:
	  if (storedValue->symbolValue == value->symbolValue)
	    {
	      found =1;
	    }
	  break;
	case openType:
	  if (storedValue->open == value->open)
	    {
	      found =1;
	    }
	  break;
	case closeType:
	  if (storedValue->close == value->close)
	    {
	      found = 1;
	    }
	  break;
	}
      if (found){
	previous->cons->cdr = current->cons->cdr;
	free(current->cons->car);
	free(current);
	return 1;
      }
    }
    previous = current;
    current = current->cons->cdr;
  }
  return 0;
}

// This function frees its cons cells.
void cleanup(List* list){
  Value *current;
  Value *second;
  while (list->head){
    second = (list->head->cons)->cdr;
    free((list->head->cons)->car);
    free(list->head->cons);
    free(list->head);
    list->head = second;
  }
}

// This function frees its cons cells and also frees the list.
void destroy(List* list){
  cleanup(list);
  free(list);
}
