# include <stdio.h>
# include <stdlib.h>
# include "stack.h"

int main() {
  Stack *mainStack;
  fprintf(stderr, "bout to init stack");
  initStack(mainStack);
  fprintf(stderr, "done initing stack");
  Value *oneValue, *twoValue,
    *threeValue, *fourValue,
    *fiveValue, *sixValue,
    *sevenValue; 

  oneValue->type = booleanType;
  oneValue->boolValue = 1;
  pushStack(mainStack, oneValue);

  twoValue->type = integerType;
  twoValue->intValue = 5;
  pushStack(mainStack, twoValue);

  threeValue->type = floatType;
  threeValue->dblValue = 2.32;
  pushStack(mainStack, threeValue);

  fourValue->type = symbolType;
  fourValue->symbolValue = "randomSym";
  pushStack(mainStack, fourValue);
  
  fiveValue->type = openType;
  fiveValue->open = '(';
  pushStack(mainStack, fiveValue);
  
  sixValue->type = closeType;
  sixValue->close = ')';
  pushStack(mainStack, sixValue);

  sevenValue->type = stringType;
  sevenValue->stringValue = "man;; this is is some string";
  pushStack(mainStack, sevenValue);  
}
