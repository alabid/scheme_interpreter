#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This function prints the linked list.
void printToken(Value* curValue){
   
  switch (curValue->type)
    {
    case booleanType:
      if(curValue->boolValue){
	printf("#t:boolean\n");
      }
      else{
	printf("#f:boolean\n");
      }
      break;
    case cellType:
      printf("Cell type");
      break;
    case integerType:
      printf("%d:integer\n",curValue->intValue);
      break;
    case floatType:
      printf("%lf:float\n",curValue->dblValue);
      break;
    case stringType:
      printf("%s:string\n",curValue->stringValue);
      break;
    case symbolType:
      printf("%s:symbol\n",curValue->symbolValue);
      break;
    case openType:
	  printf("(:open\n");
	  break;
    case closeType:
      printf("):close\n");
      break;
    default:
      break;
    }
}

HashTable* initializeTable(int capacity){
  HashTable* table = (HashTable*) malloc(sizeof(HashTable));
  table->entries = (ConsCell *) malloc(sizeof(ConsCell)*capacity);
  table->capacity = capacity;
  table->size = 0;
  return table;
}

int hash(HashTable* table, char* id){
  if (table){
    int key = 0,index=0, startingPoint;
    while (id[index]!='\0'){
      key += id[index++];
    }
    startingPoint = key;
    Value* current = ((table->entries)[key%(table->capacity)]).car;
    while (current){
      if (key>=(startingPoint+table->capacity)){
	return -1;
      }else if (current->type == symbolType && (strcmp(current->symbolValue, id)==0)){
	  return key%(table->capacity);
      }
      key++;
    }
    return key%(table->capacity);  
  }
  return -1;
}

int insertItem(HashTable* table, char* id, Value* value){
  if (table){
    if ((table->size) > ((table->capacity)*3/4)){
      autoDouble(table);
    }
    int key = hash(table, id);
    if (key == -1){
      return 0;
    }
    int length = 0;
    while (id[length]!='\0'){
      length++;
    }
    length++;
    char* copiedID = (char*)malloc(sizeof(char)*length);
    strncpy(copiedID,id,length);
    Value* keyVal = (Value*)malloc(sizeof(Value));
    keyVal->type = symbolType;
    keyVal->symbolValue = copiedID;
    
    (table->entries)[key].car = keyVal;
    (table->entries)[key].cdr = value;
    (table->size)++;
    return 1;
  }
  else{
    return 0;
  }
}

int autoDouble(HashTable* table){
  if (table){
    ConsCell* newEntries;
    int oldCapacity = (table->capacity);
    int i;
    int newID;
    newEntries = (ConsCell*) malloc(sizeof(ConsCell)*oldCapacity*2);
    table->capacity = oldCapacity*2;
    for (i=0;i<oldCapacity;i++){
      if ((table->entries)[i].car){
	if ((table->entries)[i].car->type==symbolType){
	  newID = hash(table, (table->entries)[i].car->symbolValue);
	  newEntries[newID].car = (table->entries)[i].car;
	  newEntries[newID].cdr = (table->entries)[i].cdr;
	}
      }
    }
    free(table->entries);
    table->entries = newEntries;
    return 1;
  }else{
    return 0;
  }
}

Value* deleteItem(HashTable* table, char* id){
  ConsCell* entry = lookupEntry(table, id);
  if (entry){
    if (entry->car && entry->car->type==symbolType){
      free(entry->car->symbolValue);
      free(entry->car);
    }
    entry->car = NULL;
    (table->size)--;
    return entry->cdr;
  }
  return NULL;
}

ConsCell* lookupEntry(HashTable* table, char* id){
  if (table){
    int key = hash(table, id);
    if (key==-1 || (!(table->entries)[key].car)){
      return NULL;
    }else{
      return (&((table->entries)[key]));
    }   
  }
  return NULL;
}

Value* lookup(HashTable* table, char* id){
  ConsCell* entry = lookupEntry(table, id);
  if (entry){
    return entry->cdr;
  }else{
    return NULL;
  }
}

// cleanupTable only cleans the cons cells and keys.
void cleanupTable(HashTable* table){
  if (table){
    int i;
    for (i=0;i<(table->capacity);i++){
      if (table->entries[i].car){
	if (table->entries[i].car->type==symbolType){
	  free(table->entries[i].car->symbolValue);
	}
	free(table->entries[i].car);
      }
    }
    table->size = 0;
    free(table->entries);
  }
}

void printTable(HashTable* table){
  if (table){
    int i;
    
    for (i=0;i<table->capacity;i++){
      if ((table->entries)[i].car){
	printToken((table->entries)[i].car);
	printToken((table->entries)[i].cdr);	
      }
      
    }
  }
}
  /*
  This function accepts a Value that is the head of the list, and prints out the list.
*/
