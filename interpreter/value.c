#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This function prints a given token.
void printToken(Value* curValue){
  if (curValue){
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
      case nullType:
	printf("()");
      default:
	break;
      }
  } else printf("I'm nothing!\n");
}

/* 
   This function initializes one hash table with size = capacity
   If capacity is less than or equal to zero, NULL is returned.
*/
HashTable* initializeTable(int capacity){
  if (capacity<=0){
    return NULL;
  }
  HashTable* table = (HashTable*) malloc(sizeof(HashTable));
  table->entries = (ConsCell *) calloc(capacity,sizeof(ConsCell));
  table->capacity = capacity;
  table->size = 0;
  return table;
}

/*
  Hash function that uses open addressing.
  No duplicate keys are allowed.
*/
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
      current = ((table->entries)[key%(table->capacity)]).car;
    }
    return key%(table->capacity);  
  }
  return -1;
}

/*
  Insert key = id, payload = value into the hash table.
  Auto doubling the hash table if the size of the hash table reaches 2/3 of the capacity, 
   where 2/3 is the load factor.
*/
int insertItem(HashTable* table, char* id, Value* value){
  if (!value){
      return 0;
  }
  if (table){
    if ((table->size) >= ((table->capacity)/2)){
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
    strncpy(copiedID,id,length-1);
    copiedID[length-1]='\0';
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

/*
  Auto doubling the hash table if the size of the hash table reaches 2/3 of the capacity, 
  where 2/3 is the load factor.
*/
int autoDouble(HashTable* table){
  if (table){
    int oldCapacity = table->capacity;
    int i;
    HashTable* newTable = initializeTable(table->capacity*2);
    for (i=0;i<oldCapacity;i++){
      if ((table->entries)[i].car){
	if (((table->entries)[i].car)->type == symbolType){
	  insertItem(newTable, ((table->entries)[i].car)->symbolValue, (table->entries)[i].cdr);
	}	
      }
    }
    cleanupTable(table);
    table->entries = newTable->entries;
    free(newTable);
    return 1;
  }else{
    return 0;
  }
}

/*
  Delete an item with key = id.
  This function does not free the payload since we did not malloc memory 
  inside the insertItem function for payload.
*/
Value* deleteItem(HashTable* table, char* id){
  ConsCell* entry = lookupEntry(table, id);
  if (entry){
    if (entry->car && entry->car->type == symbolType){
      free(entry->car->symbolValue); 
      free(entry->car);
     }
     entry->car = NULL;
     (table->size)--;
     
    return entry->cdr;
  }
  return NULL;
}

/*
  Return the entry with key = id
*/
ConsCell* lookupEntry(HashTable* table, char* id){
  if (table){
    int key = hash(table, id);
    if (key==-1 || (!((table->entries)[key].car))){
      return NULL;
    }else{
      return (&((table->entries)[key]));
    }   
  }
  return NULL;
}

/*
  Return the payload with key = id
*/
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

/* 
   Destroy the table by freeing the cons cells, keys and hash table itself.
   This function cannot free any payloads.
*/
void destroyTable(HashTable *table){
  if (table){
    cleanupTable(table);
    free(table);
  }
}

/*
  Print the table for debugging purposes.
*/
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

List* getKeys(HashTable* table){
  List* list = NULL;
  if (table){
    list = initializeList();
    int i;
    for (i=0;i<table->capacity;i++){
      if ((table->entries)[i].car){
	push(list, (table->entries)[i].car);	
      }  
    }
  }
  return list;
}

List* getValues(HashTable* table){
  List* list = NULL;
  if (table){
    list = initializeList();
    int i;
    for (i=0;i<table->capacity;i++){
      if ((table->entries)[i].car && (table->entries)[i].cdr){
	push(list, (table->entries)[i].cdr);
      }	 
    }
  }
  return list; 
}



List* initializeList(){
  List *list = (List*)malloc(sizeof(List));
  list->head = NULL;
  return list;
}

// This function inserts a new cons cell to the linked list.
int insertCell(List *list, Value *value){
  if (value==NULL || list==NULL){
    return 0;
  }
  Value *newValue = (Value *)malloc(sizeof(Value));
  ConsCell *newCell = (ConsCell *)malloc(sizeof(ConsCell));
  newValue->type = cellType;
  newValue->cons = newCell;
  newCell->car = value;
  newCell->cdr = list->head;
  list->head = newValue;
  return 1;
}

int push(List *list, Value *value){
  return insertCell(list, value);
}

// This function pops the head of the linked list.
Value* pop(List *list){
  if (!list){
    return NULL;
  }
  Value *returnValue = NULL;
  Value *prevHead = list->head;
  if(list->head){
    returnValue = prevHead->cons->car;
    list->head = prevHead->cons->cdr;
    free(prevHead->cons);
    free(prevHead);
  }
  return returnValue;
}
   
// This function reverses the linked list.
int reverse(List *list){
  if (!list){
    return 0;
  }
  Value *nextValue;
  Value *tempHead=NULL;
  Value *curValue = list->head;
  while (curValue){
    nextValue = curValue->cons->cdr;
    curValue->cons->cdr = tempHead;
    tempHead = curValue;
    curValue = nextValue;
  }
  list->head = tempHead;
  return 1;
}

// This function prints the linked list.
void printTokens(Value* value){
  if (value && value->type == cellType){
    Value *curValue = value;
    while (curValue){
      switch (curValue->cons->car->type)
	{
	case booleanType:
	  if(curValue->cons->car->boolValue){
	    printf("#t:boolean\n");
	  }
	  else{
	    printf("#f:boolean\n");
	  }
	  break;
	case cellType:
	  printToken(curValue->cons->car);
	  break;
	case integerType:
	  printf("%d:integer\n",curValue->cons->car->intValue);
	  break;
	case floatType:
	  printf("%lf:float\n",curValue->cons->car->dblValue);
	  break;
	case stringType:
	  printf("%s:string\n",curValue->cons->car->stringValue);
	  break;
	case symbolType:
	  printf("%s:symbol\n",curValue->cons->car->symbolValue);
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
      curValue = curValue->cons->cdr;
    }
  }
}


// This function frees its cons cells.
void cleanup(Value* head){
  if (head && (head->type == cellType)){
    Value *second;
    while (head){
      second = (head->cons)->cdr;
      if (head->cons->car->type == stringType){
	free(head->cons->car->stringValue);
	free(head->cons->car);
      }else if (head->cons->car->type == symbolType){
	free(head->cons->car->symbolValue);
	free(head->cons->car);
      }else if (head->cons->car->type == cellType){
	cleanup(head->cons->car);
      }else{
	free(head->cons->car);
      }
      free(head->cons);
      free(head);
      head = second;
    }
  }    
}
// This function frees its cons cells and also frees the list.
void destroy(List* list){
  if (list){
    cleanup(list->head);
    free(list);
  }
}

/*
// We haven't finish this function yet. 
// We will not need this function until we want to destroy all the things after interpreting. 
void cleanupValue(Value* value){
  if (value){
    switch  (value->type)
      {
      case cellType:
	cleanupValue(car(value));
	cleanupValue(cdr(value));
	free(value->cons);
	break;
      case tableType:
	if (value->tableValue){
	  cleanupTable(value->tableValue);
	  free(value->tableValue);
	}
	break;
      case envType:
	if (value->envValue){
	  cleanupValue(value->envValue->bindings);
	  free(value->envValue);
	}
	break;
      case closureType:
	if (value->closureValue){
	  cleanupValue(value->closureValue->body);
	  cleanupValue(value->closureValue->args);
	  if (value->closureValue->parent){
	    if (value->closureValue->parent->bindings){
	      cleanupValue(value->closureValue->parent->bindings);
	    }
	    free(value->closureValue->parent);
	  }
	}
      case stringType:
	free(value->stringValue);
	break;
      case symbolType:
	free(value->symbolValue);

      }
    
    free(value);   
    
  }
}
*/
/*
  This returns the car of a value (that is a list)
 */
Value *car(Value *value) {
  if (!value){
    return NULL;
  }
  if (value->type == cellType){ 
    return value->cons->car;
  }else{
    return NULL;
  }
}
/*
  This returns the cdr of a value (that is a list)
*/
Value *cdr(Value *value) {
  if (!value){
    return NULL;
  }
  if (value->type == cellType){
    return value->cons->cdr;
  }else{ 
    return NULL;
  }
}

// print value.
void printValue(Value* curValue){
  if (curValue){
    switch (curValue->type)
      {
      case booleanType:
	if(curValue->boolValue){
	  printf("#t");
	}
	else{
	  printf("#f");
	}
	break;
      case integerType:
	printf("%d",curValue->intValue);
	break;
      case floatType:
	printf("%lf",curValue->dblValue);
	break;
      case stringType:
	printf("%s",curValue->stringValue);
	break;
      case symbolType:
	printf("%s",curValue->symbolValue);
	break;
      case openType:
	printf("(");
	break;
      case closeType:
	printf(")");
	break;
      case cellType:
	printList(curValue);
	break;
	case closureType:
      case primitiveType:
	printf("#<procedure>\n");
	break;
      case nullType:
	printf("()");
      default:
	break;
      }
  } 
}

/*
  This function accepts a Value that is the head of the parse tree, and prints out the list.
*/
void printList(Value* value){
  if (value && value->type == cellType){
    printf("(");
    Value *curValue = value;
    while (curValue){
      switch (curValue->cons->car->type)
	{
	case booleanType:
	  if(curValue->cons->car->boolValue){
	    printf("#t");
	  }
	  else{
	    printf("#f");
	  }
	  break;
	case integerType:
	  printf("%d",curValue->cons->car->intValue);
	  break;
	case floatType:
	  printf("%lf",curValue->cons->car->dblValue);
	  break;
	case stringType:
	  printf("%s",curValue->cons->car->stringValue);
	  break;
	case symbolType:
	  printf("%s",curValue->cons->car->symbolValue);
	  break;
	case openType:
	  printf("(");
	  break;
	case closeType:
	  printf(")");
	  break;
	case cellType:
	  printList(curValue->cons->car);
	  break;
	default:
	  break;
	}
      if (curValue->cons->cdr){
	printf(" ");
      } 
      curValue = curValue->cons->cdr;
    } 
    printf(")");
  }
  else printf("This is not a value\n");
}

int listLength(Value *value) {
  if (!value) 
    return 0;
  else
    return 1 + listLength(value->cons->cdr);
}
