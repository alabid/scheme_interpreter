#include "value.h"
#include <stdlib.h>
#include <stdio.h>

int main(){
  HashTable* table = initializeTable(2);
  char x[]="x\0";
  char y[]="y\0";
  char z[]="z\0";
  char t[]="t\0";
  Value *value1, *value2, *value3, *value4;
  value1 = (Value*)malloc(sizeof(Value));
  value1->type = integerType;
  value1->intValue = 10; 
  value2 = (Value*)malloc(sizeof(Value));
  value2->type = integerType;
  value2->intValue = 9; 
  value3 = (Value*)malloc(sizeof(Value));
  value3->type = integerType;
  value3->intValue = 8; 
  value4 = (Value*)malloc(sizeof(Value));
  value4->type = integerType;
  value4->intValue = 7; 
  insertItem(table,x,value1);
  insertItem(table,y,value2);
  insertItem(table,z,value3);
  insertItem(table,t,value4);
  printToken(lookup(table,x));
  printf("\n");
  printTable(table);
  printf("before deleting\n");
  printToken(deleteItem(table,t));
  printf("after deleting\n\n\n");
  printTable(table);
  cleanupTable(table);
  free(value1);
  free(value2);
  free(value3);
  free(value4);
  free(table);
  return 0;
}
