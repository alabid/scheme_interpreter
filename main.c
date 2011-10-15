#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"

int main(int argc, char *argv[]){
  List *list = (List *) malloc(sizeof(List));
  initialize(list);
  Value *value;
  int i;
  for (i=0;i<20;i++){
    value = (Value *)malloc(sizeof(List));
    value->type = integerType;
    value->intValue = i;
    insertCell(list,value);
  }
  print(list);
  destroy(list);
  return 0;
}
