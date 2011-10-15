
#include <stdlib.h>
#include "tokenizer.h"

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
  while(curValue){
    switch curValue->cons->car->type{
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
	printf("%lf:double\n",curValue->cons->car->dbValue);
	break;
      case stringType:
	printf("%s:string\n",curValue->cons->car->stringValue);
	break;
      case symbolType:
	printf("%s:symbol\n",curValue->cons->car->symbolValue);
	break;
      case open:
	printf("(:open\n");
	break;
      case close:
	printf("):close\n");
	break;
      default:
	break;
      }
    curValue = curValue->cons->cdr;
  }
}

