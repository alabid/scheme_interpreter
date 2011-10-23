# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "stack.h"

/*
  stack.c --
  Implements a stack.
*/
void initStack(Stack *stack) {
  List *items = (List *) malloc(sizeof(List));
  stack->items = items;
  initialize(stack->items);
  stack->size = 0;
}

Value * popStack(Stack *stack) {
  if (!stack || stack->size <= 0)
    return NULL;
  
  Value *popped = pop(stack->items);
  stack->size -= 1;

  return popped;
}

int pushListInStack(Stack *stack, List *newList) {
  Value *listValue = (Value *)malloc(sizeof(Value));
  ConsCell *listConsCell = (ConsCell *)malloc(sizeof(ConsCell));

  listValue->type = cellType;
  listValue->cons = listConsCell;
  
  listConsCell->car = newList->head;
  listConsCell->cdr = stack->items->head;
  stack->items->head = listValue;

  stack->size += 1;
  
  return 1;
}
/*
  I can only copy by one level.
 */
List *deepCopy(List *listToCopy) {
  Value *head = listToCopy->head;
  Value *newValue = NULL;
  List *newList = (List *)malloc(sizeof(List));
  

  while (head) {   
    newValue = (Value *) malloc(sizeof(Value));
    switch (head->cons->car->type) {
      case booleanType:
        newValue->type = booleanType;
        newValue->boolValue = head->cons->car->boolValue;
	break;
      case integerType:
	newValue->type = integerType;
	newValue->intValue = head->cons->car->intValue;
	break;
      case floatType:
	newValue->type = floatType;
	newValue->dblValue = head->cons->car->dblValue;
	break;
      case stringType:
	newValue->type = stringType;
	newValue->stringValue = (char *)malloc(sizeof(char)*
					       strlen(head->stringValue));
	strcpy(newValue->stringValue,
	     head->cons->car->stringValue);
	break;
      case symbolType:
	newValue->type = symbolType;
	newValue->symbolValue = (char *)malloc(sizeof(char)*
				    strlen(head->cons->car->symbolValue));
	strcpy(newValue->symbolValue,
	     head->cons->car->symbolValue);
	break;
      case openType:
	newValue->type = openType;
	newValue->open = head->cons->car->open;
	break;
      case closeType:
	newValue->type = closeType;
	newValue->close = head->cons->car->close;
	break;
      default:
	break;
    }
    insertCell(newList, newValue);
    head = head->cons->cdr;
  }
  // fprintf(stderr, "returning from exile\n");
  
  // printToken(newList);
  // reverse(newList);
  fprintf(stderr, "returning from exile\n");
  return newList;
}

List *append(List *list1, List *list2) {
  Value *tempValue = list1->head;
  // we should return a new list
  if (list1 == NULL) return list2; // my deepCopy
  // function isn't working right now
  else if (list2 == NULL) return list1;

  else {
    do {
      tempValue = tempValue->cons->cdr;
    } while (tempValue->cons->cdr);

    tempValue->cons->cdr = list2->head;
  }
  return list1;
}

int pushStack(Stack *stack, Value *value) {
  int inserted = insertCell(stack->items, value);
  if (inserted)
    stack->size += 1;

  return inserted;
}

void printStack(Stack *stack) {
  fprintf(stderr, "\n=========STACK=========\n");
  fprintf(stderr, "SIZE OF STACK=%d\n", stack->size);
  
  printToken(stack->items);
}

