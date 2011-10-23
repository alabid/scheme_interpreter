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

List *deepCopy(List *listToCopy);

/*
pushes an item onto the top of the stack
*/
int pushStack(Stack *stack, Value *value);

int pushListInStack(Stack *stack, List *list);

void printStack(Stack *stack);

Value *car (List *list);
/*
  append returns a new list containing list1 and list2 
  where list2 is at the tail of list1
 */
List *append(List *list1, List *list2);
