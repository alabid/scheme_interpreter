#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"

int main(int argc, char *argv[]){
  List *list = (List *) malloc(sizeof(List));
  initialize(list);
  Value *value;
  int i;
  char letters[]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
  for (i=0;i<20;i++){
    value = (Value *)malloc(sizeof(Value));
    value->type = symbolType;
    char *s = (char *)malloc(sizeof(char)*256);
    *s = letters[i];
	*(s+1)='\0';
    value->symbolValue =s; 
    insertCell(list,value);
  }
  print(list);
  reverse(list);
  print(list);
  destroy(list);
  return 0;
}
