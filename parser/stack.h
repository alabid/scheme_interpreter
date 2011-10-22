# include "tokenizer.h"
/*
  stack.h -- function defintions for the stack
*/

/*
 *A stack has a list of items that can only be accessed from the top.
 */
typedef struct __Stack {
  struct __LinkedList *items; // linked list
  int size;
}Stack;

void initStack(Stack *stack);

/*
pops an item off the stack and returns it
*/
Value* popStack(Stack *stack);

/*
pushes an item onto the top of the stack
*/
int pushStack(Stack *stack, Value *value);

int pushListInStack(Stack *stack, List *list);

void printStack(Stack *stack);
