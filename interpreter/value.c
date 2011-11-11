#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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
	printf("Cell type\n");
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

void printValue(Value *curValue) {
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
	//if (withQuotes)
	  printf("(");
	break;
      case closeType:
	//if (withQuotes)
	  printf(")");
	break;
      case cellType:
	printList(curValue);
	break;
      case closureType:
	printf("#<procedure");
	if (curValue->closureValue && curValue->closureValue->identifier!=NULL){
	  printf(":%s>",curValue->closureValue->identifier);
	}else{
	  printf(">");
	}
	break;
      case primitiveType:
	printf("#<procedure>");
	break;
      case nullType:
	printf("()");
	break;
      case envType:
	printf("Environment");
	break;
      default:
	break;
      }
  } 
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
      key += (id[index++]) * index;
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
    
    if ((table->entries)[key].cdr){
      freeValue((table->entries)[key].cdr);
    }
    (table->entries)[key].car = keyVal;
    (table->entries)[key].cdr = deepCopy(value);
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
int deleteItem(HashTable* table, char* id){
  ConsCell* entry = lookupEntry(table, id);
  if (entry){
    if (entry->car && entry->car->type == symbolType){
      free(entry->car->symbolValue); 
      free(entry->car);
      freeValue(entry->cdr);
     }
     entry->car = NULL;
     //entry->cdr = NULL;
     (table->size)--;
     
    return 1;
  }
  return 0;
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

void cleanupEnvironment(Environment *env) {
  if (env) {
    while (env) {
      if (env->bindings->type == tableType) {
	cleanupTable(env->bindings->tableValue);
	free(env->bindings);
      }
      env = env->parent;
    }
  }
  free(env);
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
	freeValue(table->entries[i].cdr);
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


// This function frees its cons cells. It can only be used on basic types from tokenizer.
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

void freeValue(Value *value){
  if (!value){
    return;
  }
  assert(value->type!=envType);
  if (value->type == stringType){
    free(value->stringValue);
    free(value);
    
  }else if (value->type == symbolType){
    free(value->symbolValue);
    free(value);
  }else if (value->type == cellType){
    assert(value->cons!=NULL);
    assert(value->cons->car!=NULL);
    freeValue(value->cons->car);
    freeValue(value->cons->cdr);
    free(value->cons);
    free(value);

  }else if (value->type == primitiveType){
    free(value);
  }else if (value->type == closureType){
    destroyClosure(value->closureValue);
    free(value);
  }else if (value->type == tableType){
    destroyTable(value->tableValue);
    free(value);
  }else{
    free(value);
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
  This returns the car of a value (that is a list)
 */
Value *car(Value *value, Environment *env) {
  Value *current = value;
  int count = listLength(value);
  if (count > 1) {
    printf("car:expects 1 argument, given %d.\n", count);
    return NULL;
  }
  else if (count < 1){
    printf("car: expects argument of type <pair>.\n");
    return NULL;
  }
  
  if (!(current && (current->type == cellType))){
    printf("car: expects argument of type <pair>, given 0\n");
    return NULL;
  }else if (getFirst(current) && getFirst(current)->type == nullType){
      printf("car: expects argument of type <pair>; given ()\n");
      return NULL;
  }
  while (getFirst(current) && getFirst(current)->type!=openType){ 
    current = getFirst(current);
  }
  if (!getFirst(current)){
    printf("car: expects argument of type <pair>: given ");
    printValue(getFirst(value));
    printf("\n");
    return NULL;
  }

  Value *content = current->cons->cdr;
  if (listLength(content) < 1) {
    printf("car: you must car a list with one or more items\n");
    return NULL;
  } else {
    return getFirst(content);
  }
  return NULL;
}
/*
  This returns the cdr of a value (that is a list)
 */
Value *cdr(Value *value, Environment *env) {
  Value *content;
  Value *openParen;
  Value *newValue;
 
  printf("print cdr: ");
  printValue(value);
  printf("\n");
  Value *current = value;
  int count = listLength(value);
  if (count > 1) {
    printf("cdr:expects 1 argument, given %d.\n", count);
    return NULL;
  }
  else if (count < 1){
    printf("cdr: expects argument of type <pair>.\n");
    return NULL;
  }
  
  if (!(current && (current->type == cellType))){
    printf("cdr: expects argument of type <pair>, given 0\n");
    return NULL;
  }else if (getFirst(current) && getFirst(current)->type == nullType){
      printf("cdr: expects argument of type <pair>; given ()\n");
      return NULL;
  }
  while (getFirst(current) && getFirst(current)->type!=openType){ 
    current = getFirst(current);
  }
  if (!getFirst(current)){
    printf("cdr: expects argument of type <pair>: given ");
    printValue(getFirst(value));
    printf("\n");
    return NULL;
  }

  content = current->cons->cdr;
  
  if (listLength(content) == 1) {
     newValue = (Value *) malloc(sizeof(Value));
     newValue->type = nullType;
     insertItem(env->bindings->tableValue,"#cdr",newValue);
     free(newValue);
     return lookup(env->bindings->tableValue, "#cdr");
  } else {
    openParen = (Value *) malloc(sizeof(Value));
    openParen->type = openType;
    openParen->open = '(';
    newValue = (Value* )malloc(sizeof(Value));
    newValue->type = cellType;
    newValue->cons = (ConsCell *)malloc(sizeof(ConsCell));
    newValue->cons->car = openParen;
    newValue->cons->cdr = getTail(content);
    insertItem(env->bindings->tableValue, "#cdr", newValue);
    free(openParen);
    free(newValue->cons);
    free(newValue);
    newValue = lookup(env->bindings->tableValue, "#cdr");
    return newValue;

  }
 
}



/*
  (cons 1 '(2 3)) --> (1 2 3)
  (cons 1 3) --> (1  . 3)
*/
Value *cons(Value *value, Environment *env) {
  Value *openParen, *closeParen;
  Value *newValue;
  Value *newCar,  *newCdr;
  Value *carCdr;
  Value *endCdr;
  Value *carCopy, *cdrCopy;

  int count = listLength(value);

  if (!(count == 2)) {
    printf("cons: expects 2 arguments, given %d: ", count);
    printValue(value);
    printf("\n");
    return NULL;
  }
  // first handle this case: (cons 1 (1 2)) -> (1 1 2)
  // secondly: (cons 1 2) -> (1 . 2)
  newCar = getFirst(value);
  newCdr = getFirst(getTail(value));
  
  assert(newCdr != NULL);

  if (newCdr->type == cellType) {
    carCopy = deepCopy(newCar);
    cdrCopy = deepCopy(newCdr);

    free(cdrCopy->cons->car);
    Value *freeMe = cdrCopy;
    cdrCopy = cdrCopy->cons->cdr;
    free(freeMe->cons);
    free(freeMe);

    carCdr = (Value *)malloc (sizeof(Value));
    carCdr->type = cellType;
    carCdr->cons = (ConsCell *)malloc(sizeof(ConsCell));
    carCdr->cons->car = carCopy;
    carCdr->cons->cdr = cdrCopy;
        
  } else if (typeCheck(newCdr) == 1 || typeCheck(newCdr) == 2) {
    carCopy = deepCopy(newCar);
    cdrCopy = deepCopy(newCdr);    

    carCdr = (Value *)malloc (sizeof(Value));
    carCdr->type = cellType;
    carCdr->cons = (ConsCell *)malloc(sizeof(ConsCell));
    carCdr->cons->car = carCopy;
    carCdr->cons->cdr = cdrCopy;
    
  } 
  
  openParen = (Value *) malloc(sizeof(Value));
  openParen->type = openType;
  openParen->open = '(';

  newValue = (Value* )malloc(sizeof(Value));
  newValue->type = cellType;
  newValue->cons = (ConsCell *)malloc(sizeof(ConsCell));
  newValue->cons->car = openParen;
  newValue->cons->cdr = carCdr;
  printValue(newValue); 
  // perhaps the deep copy doesn't work for the new cons
  printValue(deepCopy(newValue));
  insertItem(env->bindings->tableValue,"#cons",newValue);
  // we probably have to free the remaining components of newValue
  free(newValue);
  
  return lookup(env->bindings->tableValue, "#cons");
}

/*
  This function accepts a Value that is the head of the parse tree, and prints out the list.
*/
void printList(Value* value){
  if (value && value->type == cellType){
    Value *curValue = value;
    while (curValue->cons->cdr){
      if (!(curValue->type == cellType) && !curValue->cons->car) {
	return;
      }
      if (!(curValue->type == cellType)) {
	printf(" . ");
	printValue(curValue);
	printf(")");
	return;
      }
      if (!(curValue->cons->cdr->type == cellType)){
	printValue(curValue->cons->car);
	printf(" . ");
	printValue(curValue->cons->cdr);
	printf(")");
	return;
      }
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
	  //if (withQuotes)
	    printf("(");
	  break;
	case closeType:
	  //if (withQuotes)
	    printf(")");
	  break;
	case cellType:
	  printList(curValue->cons->car);
	  break;
	case nullType:
	  printf("()");
	  break;
	default:
	  break;
	}
      
      if (curValue->cons->cdr && 
	  curValue->cons->cdr->cons->car->type !=closeType &&
	  curValue->cons->car &&
	  curValue->cons->car->type!=openType){
	printf(" ");
	} 
       	curValue = curValue->cons->cdr;
     } 

    if (curValue->type == cellType) {
       printValue(curValue->cons->car);
   }  
    
  }
  else printf("This is not a value\n");
}

// return the car of a cons cell.
Value *getFirst(Value *value) {
  if (!value)
    return NULL;
  else if (value->type == cellType) {
    return value->cons->car;
  }else 
    return NULL;
}

// return the cdr of a cons cell.
Value *getTail(Value *value) {
  if (!value) 
    return NULL;
  else if (value->type == cellType) {
    return value->cons->cdr;
  }else 
    return NULL;
}

int listLength(Value *value) {
  return properListLength(value);
}

int properListLength(Value *value) {
  if (!(value && value->type==cellType)){ 
    return 0;
  }else if (value->cons->car->type == openType) {
    return properListLength(value->cons->cdr);
  }else if (value->cons->car->type == closeType){
    return 0;
  }
  else {
    return 1 + properListLength(value->cons->cdr);
  }
}

Value *deepCopyList(Value *value){
  Value *head = value;
  Value *newValue = NULL;
  List *newList = initializeList();
  while (head && head->type == cellType) {   
    newValue = (Value *) malloc(sizeof(Value));
    if (!getFirst(head)){
      break;
    }
    switch (head->cons->car->type) 
      {
      case booleanType:
        newValue->type = booleanType;
        newValue->boolValue = head->cons->car->boolValue;
	break;
      case integerType:
	newValue->type = integerType;
	newValue->intValue = head->cons->car->intValue;
	break;
      case floatType:
	newValue->type = floatType;
	newValue->dblValue = head->cons->car->dblValue;
	break;
      case stringType:
	newValue->type = stringType;
	newValue->stringValue = (char *)malloc(sizeof(char)*(strlen(head->cons->car->stringValue)+1));
	strcpy(newValue->stringValue, head->cons->car->stringValue);
	break;
      case symbolType:
	newValue->type = symbolType;
	newValue->symbolValue = (char *)malloc(sizeof(char)*(strlen(head->cons->car->symbolValue)+1));
	strcpy(newValue->symbolValue,head->cons->car->symbolValue);
	break;
      case openType:
	newValue->type = openType;
	newValue->open = head->cons->car->open;
	break;
      case closeType:
	newValue->type = closeType;
	newValue->close = head->cons->car->close;
	break;
      case cellType:
	free(newValue);
	newValue = deepCopyList(head->cons->car);
	break;
      case nullType:
	newValue->type = nullType;
	break;
      case primitiveType:
      case closureType:
	free(newValue);
	newValue = deepCopyFun(head->cons->car);
	break;
      default:
	break;
      }
    push(newList, newValue);
    head = head->cons->cdr;
  }
  
  reverse(newList);
  head = newList->head;
  free(newList);
  return head;
}


// deep copy as a linked list.
List* deepCopyLinkedList(List* list){
  if (list){
 
    List *returnList = initializeList();
    Value *head = deepCopyList(list->head);
    returnList->head = head;
    return returnList;
  }
  return NULL;
}

//  deep copy the things in parse tree
Value* deepCopyFun(Value *function){
  if (function){
    Value *value = (Value *)malloc(sizeof(Value));
    if (function->type == primitiveType){
      value->type = primitiveType;
      value->primitiveValue = function->primitiveValue;
      return value;
    }else if (function->type == closureType){
      value->type=closureType;
      value->closureValue = (Closure *)malloc(sizeof(Closure));
      value->closureValue->body = deepCopy(function->closureValue->body);
      value->closureValue->args = deepCopyLinkedList(function->closureValue->args);
      value->closureValue->parent = function->closureValue->parent;
      if (function->closureValue->identifier){
	value->closureValue->identifier = (char *)malloc(sizeof(char)*(strlen(function->closureValue->identifier)+1));
	strcpy(value->closureValue->identifier,function->closureValue->identifier);
	value->closureValue->identifier[strlen(function->closureValue->identifier)] = '\0';	
      }else{
	value->closureValue->identifier = NULL;
      } 
      return value;
    }else{
      free(value);
    } 
  }
  return NULL;
}
Value* deepCopy(Value *value){

  if (value){   
    Value *newValue = (Value *) malloc(sizeof(Value));
    switch (value->type) 
      {
      case booleanType:
        newValue->type = booleanType;
        newValue->boolValue = value->boolValue;
	break;
      case integerType:
	newValue->type = integerType;
	newValue->intValue = value->intValue;
	break;
      case floatType:
	newValue->type = floatType;
	newValue->dblValue = value->dblValue;
	break;
      case stringType:
	newValue->type = stringType;
	newValue->stringValue = (char *)malloc(sizeof(char)*(strlen(value->stringValue)+1));
	strcpy(newValue->stringValue, value->stringValue);
	break;
      case symbolType:
	newValue->type = symbolType;
	newValue->symbolValue = (char *)malloc(sizeof(char)*(strlen(value->symbolValue)+1));
	strcpy(newValue->symbolValue,value->symbolValue);
	break;
      case openType:
	newValue->type = openType;
	newValue->open = value->open;
	break;
      case closeType:
	newValue->type = closeType;
	newValue->close = value->close;
	break;
      case cellType:
	free(newValue);
	newValue = deepCopyList(value);
	break;
      case nullType:
	newValue->type = nullType;
	break;
      case primitiveType:
      case closureType:
	free(newValue);
	newValue = deepCopyFun(value);
	break;
      case envType:
	free(newValue);
	newValue = deepCopyEnv(value);
	break;
      case tableType:
	free(newValue);
	newValue = deepCopyTable(value);
	break;
      default:
	return NULL;
	break;
      }
    return newValue;
  }
  return NULL;
}

Value *deepCopyEnv(Value * value){
  assert(value!=NULL);
  assert(value->type==envType);
  Value *returnValue = (Value *)malloc(sizeof(Value));
  returnValue->type = envType;
  returnValue->envValue = (Environment *)malloc(sizeof(Environment));
  returnValue->envValue->parent = value->envValue->parent;
  returnValue->envValue->bindings = deepCopyTable(value->envValue->bindings);
  return returnValue;
}

Value *deepCopyTable(Value * value){
  assert(value!=NULL);
  assert(value->type==tableType);
  int i;
  Value *returnValue = (Value *)malloc(sizeof(Value));
  returnValue->type = tableType;
  returnValue->tableValue = initializeTable(value->tableValue->capacity);
  for (i=0;i<value->tableValue->capacity;i++){
    if ((value->tableValue->entries)[i].car){
      if (((value->tableValue->entries)[i].car)->type == symbolType){
	insertItem(returnValue->tableValue, ((value->tableValue->entries)[i].car)->symbolValue, (value->tableValue->entries)[i].cdr);
      }	
    }
  }
  return returnValue;
}
Closure *initializeClosure(Environment* env){
  Closure *closure = (Closure *)malloc(sizeof(Closure));
  closure->parent = env;
  closure->identifier = NULL;
  closure->args = initializeList();
  //closure->body = (Value *)malloc(sizeof(Value));
  //closure->body->type = cellType;
  return closure;
}


// cleanup frees the contents of the closure, but does not free closure itself.
void cleanupClosure(Closure *closure){
  if (closure && closure->args && closure->body){
    cleanup(closure->args->head);
    if (closure->identifier){
      free(closure->identifier);
    }
    free(closure->args); 
    freeValue(closure->body);
  }
}

// destroy frees the contents of the closure and closure itself.
void destroyClosure(Closure *closure){
  if (closure){
    cleanupClosure(closure);
    free(closure);
  }
}

// name the closure for comparision purpose.
void nameClosure(Closure *closure, char *id){
  if (closure){
    if (closure->identifier!=NULL){
      free(closure->identifier);
    }
    closure->identifier = (char *)malloc(sizeof(char)*(strlen(id)+1));
    strcpy(closure->identifier, id);
    closure->identifier[strlen(id)]='\0';
  }

}

// remove the last thing in the list, typically the close parenthesis.
// If the list is empty or only has one element, nothing will happen.
void removeLast(Value* value){
  if (!value || value->type!=cellType){
    return;
  }
  Value *previous = NULL;
  Value *current = value;
  Value *next = getTail(value);
  while (next){
    previous = current;
    current = next;
    next = getTail(next);
  }
  if (!previous){
    return;
  }else{
    assert(current->type == cellType);
    free(current->cons->car);
    free(current->cons);
    free(current);
    previous->cons->cdr = NULL;
  } 
}
void destroyEnvironment(Environment *env){
  if (env){
    assert(env->bindings!=NULL);
    assert(env->bindings->type==tableType);
    destroyTable(env->bindings->tableValue);
    free(env->bindings);
    free(env);
  }
}

Value* findLast(Value *value) {
  if (!value) return NULL;
  else return findLastRec(value);
}

Value *findLastRec(Value* value){
  if (!(value->cons->cdr)) {
    return value;
  } 
  else
    return findLast(value->cons->cdr);
}

// can free top frame and also destory itself.
void destroyTopFrame(Environment *env){
  assert(env!=NULL);
  Environment *top = env;
  while (top->parent){
    top = top->parent;
  }  // make sure it is the top level environment.
  Value *subEnvCounter = lookup(top->bindings->tableValue, "#envID");
  assert(subEnvCounter!=NULL);
  assert(subEnvCounter->type==integerType);
  int i;
  ConsCell *entry;
  char *id;
  printf("the counter of top level is ");
  printValue(subEnvCounter);
  printf("\n");
  for (i = subEnvCounter->intValue ;i>=0;i--){
    id = intToString(i);
    printf("the id is %s\n",id);
    assert(top->bindings->tableValue!=NULL);
    entry = lookupEntry(top->bindings->tableValue, id);
    if (entry){
      destroyFrame(entry->cdr->envValue);
    
      free(entry->cdr);
      entry->cdr = NULL;
    }
    free(id);
  }
  
}

void destroyFrame(Environment *env){
  assert(env!=NULL);
  Value *subEnvCounter = lookup(env->bindings->tableValue, "#envID");
  assert(subEnvCounter!=NULL);
  assert(subEnvCounter->type==integerType);
  int i;
  ConsCell *entry;
  char *id;
  printf("the counter of sublevel is ");
  printValue(subEnvCounter);
  printf("\n");
  for (i = subEnvCounter->intValue;i>=0;i--){
    id = intToString(i);
    printf("the id is %s\n",id);
    assert(env->bindings->tableValue!=NULL);
    entry = lookupEntry(env->bindings->tableValue, id);
    
    if (entry){
      printf("Hello world\n");
      destroyFrame(entry->cdr->envValue);

      free(entry->cdr);
      entry->cdr = NULL;
    }
    free(id);
  }
  destroyEnvironment(env);
}

char* intToString(int number){
  char digit[] = {'0','1','2','3','4','5','6','7','8','9'}; 
  int reminder;
  int quotient = number;
  int counter = 0;
  if (number==0){
    char *id = (char *)malloc(sizeof(char)*3);
    id[0]='#';
    id[1]='0';
    id[2]='\0';
    return id;
  }
  while (quotient!=0){
    reminder = quotient % 10;
    quotient = quotient / 10;
    if (reminder==0 && quotient ==0){
      counter++;
    }
    counter++;
  }

  char *id = (char *)malloc(sizeof(char)*(counter+2));
  id[0]='#';
  id[counter+1] = '\0';
  quotient = number;
  while (quotient!=0){
    reminder = quotient % 10;
    quotient = quotient / 10;
    id[counter] = digit[reminder];
    counter --;
 }
  return id;
}


