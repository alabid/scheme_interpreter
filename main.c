#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"

int main(int argc, char *argv[]){
  List *list = (List *) malloc(sizeof(List));
  initialize(list);
  Value *value;
  int i;
  char letters[]={abcdefghijklmnopqrstuvwxyz};
  for (i=0;i<20;i++){
    value = (Value *)malloc(sizeof(Value));
    value->type = symbol;
    char *s = (char *)malloc(sizeof(char)*256);
    *s=letters[i];
    value->charValue =s; 
    insertCell(list,value);
  }
  print(list);
  reverse(list);
  print(list);
  destroy(list);
  return 0;
}
