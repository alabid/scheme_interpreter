# include <stdio.h>
# include <stdlib.h>
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

