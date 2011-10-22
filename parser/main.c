# include <stdio.h>
# include <stdlib.h>
# include "stack.h"

int main(int argc, char **argv) {
  Stack *mainStack = (Stack *) malloc(sizeof(Stack));
  
  // for the list that's going into the stack
  List listInStack;
  List *plistInStack = &listInStack; // pointer to somewhere on 
  // stack
  initialize(plistInStack);

  initStack(mainStack);
 
  Value oneValue, twoValue,
    threeValue, fourValue,
    fiveValue, sixValue,
    sevenValue; 
  
  oneValue.type = booleanType;
  oneValue.boolValue = 1;
  printStack(mainStack);
  pushStack(mainStack, &oneValue);
  printStack(mainStack);
  // insert #t into the list
  insertCell(plistInStack, &oneValue);
  // end of insert
  
  twoValue.type = integerType;
  twoValue.intValue = 5;
  pushStack(mainStack, &twoValue);
  popStack(mainStack);
  printStack(mainStack);
  // insert #t into the list
  insertCell(plistInStack, &twoValue);
  // end of insert
  
  threeValue.type = floatType;
  threeValue.dblValue = 2.32;
  pushStack(mainStack, &threeValue);
  printStack(mainStack);
  // insert #t into the list
  insertCell(plistInStack, &threeValue);
  // end of insert
  
  fourValue.type = symbolType;
  fourValue.symbolValue = "randomSym";
  pushStack(mainStack, &fourValue);
  printStack(mainStack);
  popStack(mainStack);
  popStack(mainStack);
  // insert #t into the list
  insertCell(plistInStack, &fourValue);
  // end of insert
  
  fiveValue.type = openType;
  fiveValue.open = '(';
  pushStack(mainStack, &fiveValue);
  printStack(mainStack);
  // insert #t into the list
  insertCell(plistInStack, &fiveValue);
  // end of insert


  // now at this point, list = [#t, 5, 2.32, "randomSym",
  // '(']; oops. prints in reverse. Just can't think like
  // a stack does.
  // reverse(list);
  sixValue.type = closeType;
  sixValue.close = ')';
  pushStack(mainStack, &sixValue);
  printStack(mainStack);

  sevenValue.type = stringType;
  sevenValue.stringValue = "\"man;; this is is some string\"";
  pushStack(mainStack, &sevenValue);  


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


  pushListInStack(mainStack, plistInStack);
  
  printStack(mainStack);

  return 0;
}
