/* By Jie Lin, Kan Wang, Daniel Alabi, Yichen Shen.
   CS 251, Fall 2011
*/
#include "tokenizer.h"

// define the types of errors.
enum ERROR{
  SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS,SYNTAX_ERROR_UNTERMINATED_INPUT
};

/* 
   This function parses a list of tokens. If the tokens is incomplete, the parse tree is incomplete. 
   "depth" determines whether the parse tree is complete.
*/
List* parse(List* tokens, int* depth);

/*
  This function accepts the first list in the reverse order, and the second list in order. 
  It modifies the lists in place and returns list2 as the final list.
*/
List* append(List* list1, List* list2);

/*
  This function accepts a Value that is the head of the list, and prints out the list.
*/
void printValue(Value* head);

/*
  returns the car of the List
*/
Value *car (Value *value);

/*
  returns the cdr of the List
*/
Value *cdr (Value *value);
