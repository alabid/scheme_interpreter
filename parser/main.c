# include <stdio.h>
# include <stdlib.h>
# include "stack.h"

int main(int argc, char **argv) {
  Stack *mainStack = (Stack *) malloc(sizeof(Stack));
  
  // for the list that's going into the stack

  List *plistInStack = (List *)malloc(sizeof(List)); // pointer to somewhere on 
  // I don't want to allocate stuff on the stack here
  List *plistStackCopy;
  List *toTestAppend;
  List *resultAppend;
  // stack
  initialize(plistInStack);
 
  initStack(mainStack);
 
  Value *oneValue, *twoValue,
    *threeValue, *fourValue,
    *fiveValue, *sixValue,
    *sevenValue; 
  oneValue = (Value *)malloc(sizeof(Value));
  oneValue->type = booleanType;
  oneValue->boolValue = 1;
  printStack(mainStack);
  pushStack(mainStack, oneValue);
  printStack(mainStack);
  // insert #t into the list
  insertCell(plistInStack, oneValue);
  // end of insert
  twoValue = (Value *)malloc(sizeof(Value));
  twoValue->type = integerType;
  twoValue->intValue = 5;
  pushStack(mainStack, twoValue);
  popStack(mainStack);
  printStack(mainStack);
  // insert #t into the list
  insertCell(plistInStack, twoValue);
  // end of insert
  threeValue = (Value *)malloc(sizeof(Value));
  threeValue->type = floatType;
  threeValue->dblValue = 2.32;
  pushStack(mainStack, threeValue);
  printStack(mainStack);
  // insert #t into the list
  insertCell(plistInStack, threeValue);
  // end of insert
  
  fourValue = (Value *)malloc(sizeof(Value));
  fourValue->type = symbolType;
  fourValue->symbolValue = "randomSym";
  pushStack(mainStack, fourValue);
  printStack(mainStack);
  popStack(mainStack);
  popStack(mainStack);
  // insert #t into the list
  insertCell(plistInStack, fourValue);
  // end of insert
  fiveValue = (Value *)malloc(sizeof(Value));
  fiveValue->type = openType;
  fiveValue->open = '(';
  pushStack(mainStack, fiveValue);
  printStack(mainStack);
  // insert #t into the list
  insertCell(plistInStack, fiveValue);
  // end of insert


  // now at this point, list = [#t, 5, 2.32, "randomSym",
  // '(']; oops. prints in reverse. Just can't think like
  // a stack does.
  // reverse(list);
  sixValue = (Value *)malloc(sizeof(Value));
  sixValue->type = closeType;
  sixValue->close = ')';
  pushStack(mainStack, sixValue);
  printStack(mainStack);
 
  sevenValue = (Value *)malloc(sizeof(Value));
  sevenValue->type = stringType;
  sevenValue->stringValue = "\"man;; this is is some string\"";
  pushStack(mainStack, sevenValue);  


  printStack(mainStack);
  popStack(mainStack);
  printStack(mainStack);
  popStack(mainStack);
  printStack(mainStack);
  popStack(mainStack);
  printStack(mainStack);
  popStack(mainStack);
  printStack(mainStack);
  popStack(mainStack);
  printStack(mainStack);
  popStack(mainStack);
  printStack(mainStack);
  popStack(mainStack);  
  printStack(mainStack);
  popStack(mainStack);
  printStack(mainStack);
 
  plistStackCopy = deepCopy(plistInStack);
  toTestAppend = deepCopy(plistInStack);
  resultAppend = append(plistStackCopy, toTestAppend);

  pushListInStack(mainStack, resultAppend);
  
  printStack(mainStack);

  /*
    Story so far:
    I've been working on append. Append should return a new list.
    But it doesn't right now because it depends on a function,
    deepCopy that should return a copy of one list. Remember
    that a copy should point to different parts of memory but should
    have the same values as the one from which it should be copying from.

    whenever I try to reverse the newlist or print it, it gives me a
    Segmentation fault. Could someone held me.
   */

  return 0;
}

