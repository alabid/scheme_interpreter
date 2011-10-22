# include <stdio.h>
# include <stdlib.h>
# include "stack.h"

int main() {
  Stack *mainStack = (Stack *) malloc(sizeof(Stack));

  initStack(mainStack);
 
  Value *oneValue, *twoValue,
    *threeValue, *fourValue,
    *fiveValue, *sixValue,
    *sevenValue; 
  
  oneValue = (Value *) malloc(sizeof(Value));
  oneValue->type = booleanType;
  oneValue->boolValue = 1;
  pushStack(mainStack, oneValue);

  twoValue = (Value *) malloc(sizeof(Value));
  twoValue->type = integerType;
  twoValue->intValue = 5;
  pushStack(mainStack, twoValue);

  threeValue = (Value *) malloc(sizeof(Value));
  threeValue->type = floatType;
  threeValue->dblValue = 2.32;
  pushStack(mainStack, threeValue);
  
  fourValue = (Value *) malloc(sizeof(Value));
  fourValue->type = symbolType;
  fourValue->symbolValue = "randomSym";
  pushStack(mainStack, fourValue);
  
  fiveValue = (Value *) malloc(sizeof(Value));
  fiveValue->type = openType;
  fiveValue->open = '(';
  pushStack(mainStack, fiveValue);

  sixValue = (Value *) malloc(sizeof(Value));
  sixValue->type = closeType;
  sixValue->close = ')';
  pushStack(mainStack, sixValue);


  sevenValue = (Value *) malloc(sizeof(Value));
  sevenValue->type = stringType;
  sevenValue->stringValue = "man;; this is is some string";
  pushStack(mainStack, sevenValue);  

  printStack(mainStack);
  popStack(mainStack);
  printStack(mainStack);
}
