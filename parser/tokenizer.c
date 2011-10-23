/* By Jie Lin, Kan Wang, Daniel Alabi, Yichen Shen.
   CS 251, Fall 2011
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"
// This function initializes a linked list. It will assign head as NULL.
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
void printToken(Value* value){
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


// This function deletes a cons cell from the linked list.
int deleteCell(List *list, Value *value){
  int type = value->type;
  Value* current = list->head;
  Value* storedValue;
  Value* previous;
  int found = 0;
  if (type==cellType){
    return 0; //invalid input
  }
  
  while (current){
    if (current->cons->car->type==type){
      storedValue = current->cons->car;
      switch (type){
	case booleanType:
	  if (storedValue->boolValue == value->boolValue)
	    {
	      found =1;
	    }
	  break;
	case integerType:
	  if (storedValue->intValue == value->intValue)
	    {
	      found =1;
	    }
	  break;
	case floatType:
	  if (storedValue->dblValue == value->dblValue)
	    {
	      found =1;
	    }
	  break;
	case stringType:
	  if (storedValue->stringValue == value->stringValue)
	    {
	      found =1;
	    }
	  break;
	case symbolType:
	  if (storedValue->symbolValue == value->symbolValue)
	    {
	      found =1;
	    }
	  break;
	case openType:
	  if (storedValue->open == value->open)
	    {
	      found =1;
	    }
	  break;
	case closeType:
	  if (storedValue->close == value->close)
	    {
	      found = 1;
	    }
	  break;
	}
      if (found){
	previous->cons->cdr = current->cons->cdr;
	free(current->cons->car);
	free(current->cons);
	free(current);
	return 1;
      }
    }
    previous = current;
    current = current->cons->cdr;
  }
  return 0;
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
	//cleanup(head->cons->car);
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



List *tokenize(char *expression) {
  
  int i; // counter for expression

  int numStart, numEnd, stringStart,
  symbolStart, symbolEnd;// record the start and ends of the floats

  numStart = numEnd = symbolStart = symbolEnd = stringStart = 0;

  i = 0;
  
  float floatNum;
  int intNum;
  char scratchFloat[256]; // I assume there isn't any float that has more than 256 digits.
  char *scratchString; 
  char scratchInt[256]; // same thing for int
  char *scratchSymbol;
  int isFloat = 0;
  int notDigit = 0;
  char *boolString;
  int boolStart = 0;
  int boolEnd = 0;

  Value *newValue;
  List *list = initializeList();
  // now while loop that does main work

  while (expression[i] != '\n') {
    if (isspace(expression[i])) {
      i++; 
      continue;
    }

    if (expression[i] == ';') {
      break; // comment so get out
    }
    if (isdigit(expression[i]) || expression[i]=='.'
	|| expression[i]=='-' || expression[i]=='+') {
      numStart = i;
      if (expression[i] == '.') {
	isFloat=1; 
      }
      i++; 
      
      // handle numbers here
      while ((!isspace(expression[i])) 
	     && expression[i] != ')'
	     && expression[i] != '('
	     && expression[i] != '"'
	     && expression[i] != ';') {


	if (expression[i] == '.') {
	  if (isFloat){
	    notDigit =1;
	  }
	  isFloat=1; 
	  i++; 
	  continue;
	}
	
	if (!isdigit(expression[i])) {
	  // then we know it's not a digit but a symbol
	  notDigit = 1;
	}
	i++;
      }
      
      i--;
      numEnd = i;
      if (numStart == numEnd && (!isdigit(expression[numStart]))){
	notDigit = 1;
      }
      if (isFloat && !notDigit) {
	
	strncpy(scratchFloat,
		&expression[numStart],
		numEnd - numStart + 1);
	scratchFloat[numEnd-numStart +1] = '\0';
	floatNum = atof(scratchFloat);
	// ====== USE ME ======== I'M A FLOAT
	// USE ME IN FLOATNUM
	
	newValue = (Value *)malloc(sizeof(Value));
	newValue->type = floatType;
	newValue->dblValue = floatNum;
	insertCell(list, newValue);
      } else if (!isFloat && !notDigit) {
	strncpy(scratchInt,
		&expression[numStart],
		numEnd - numStart + 1);
	scratchInt[numEnd-numStart + 1] = '\0';
	intNum = atoi(scratchInt);
	
	// ========== USE ========== I'M AN INT
	// USE ME IN INTNUM
	newValue = (Value *)malloc(sizeof(Value));
	newValue->type = integerType;
	newValue->intValue = intNum;
	insertCell(list, newValue);
	
      } 
      
      if (notDigit) {
	// if it's not a digit, then it's probably a symbol
	// ======= USE ME ======= I'M A SYMBOL
	int length = numEnd - numStart + 1;
	scratchSymbol = (char *)malloc(sizeof(char)  * (length+1));
	
	strncpy(scratchSymbol,
		&expression[numStart],
		length);
	
	scratchSymbol[length] = '\0';
	newValue = (Value *)malloc(sizeof(Value));
	newValue->type = symbolType;
	newValue->symbolValue = scratchSymbol;
	insertCell(list, newValue);
      }
      
      isFloat = notDigit = 0;
      i++;
      continue;
    }
    
    // == GOING INTO THE SWITCH STATEMENT ==
    switch(expression[i]) { // these are for chars and strings
    case '(':
      // === USE ME === OPEN PARENS
      newValue = (Value *)malloc(sizeof(Value));
      newValue->type = openType;
      newValue->open = '(';
      insertCell(list, newValue);
      break;
    case ')':
      // ======= USE ME ======== CLOSE PARENS
      newValue = (Value *)malloc(sizeof(Value));
      newValue->type = closeType;
      newValue->close = ')';
      insertCell(list, newValue);
      break;
    case '"':
      stringStart = i;
      while (expression[++i] != '"'){
	if (expression[i]=='\n'){
	  reverse(list);
	  return list;
	}
      }
    
      
      scratchString = (char *)malloc(sizeof(char) * (i-stringStart+2));
      strncpy(scratchString, 
	      &expression[stringStart],
	      i - stringStart+1);
      scratchString[i-stringStart +1] = '\0'; // terminate the string
      
      // ======== USE ME ========== STRING
      // I'm in scratchString
       newValue = (Value *)malloc(sizeof(Value));
       newValue->type = stringType;
       newValue->stringValue = scratchString;
       insertCell(list, newValue);
    
      break;
    case '#':
      boolStart= i;
      while (!isspace(expression[i]) && 
	     expression[i] != ')' &&
	     expression[i] != '(' &&
	     expression[i] != ';' &&
	     expression[i] != '"' &&
	     expression[i] != '\n'){
	i++;
      }
      i--;
      boolEnd = i;
      if (boolEnd<boolStart){
	i++;
      }
      
      newValue = (Value *)malloc(sizeof(Value));
     
      boolString = (char *)malloc(sizeof(char)*(boolEnd-boolStart+2));
      strncpy(boolString, 
	      &expression[boolStart],
	      boolEnd-boolStart+1);
      boolString[boolEnd-boolStart+1] ='\0'; 
      if ((strcmp(boolString,"#f")==0) || (strcmp(boolString,"#F")==0) || (strcmp(boolString,"#false")==0)){
	newValue->type = booleanType;
	newValue->boolValue = 0;
	insertCell(list, newValue);
	free(boolString);
      }else if ((strcmp(boolString,"#t")==0) ||(strcmp(boolString,"#T")==0) || (strcmp(boolString,"#true")==0)) {
	newValue->type = booleanType;
	newValue->boolValue = 1;
	insertCell(list, newValue);
	free(boolString);
      }
      else{
	newValue->type = symbolType;
	newValue->symbolValue = boolString;
	insertCell(list, newValue);
      }
      break;
      
    default:
      // every other thing is a symbol
      symbolStart = i;

      while (expression[i] != '(' &&
	     expression[i] != ')' &&
	     expression[i] != ';' &&
	     expression[i] != '"' &&
	     !isspace(expression[i]) &&
	     expression[i] != '\n'
	     ) {
	i++;
      }
      i--; // take me to the end of the symbol
      symbolEnd = i;
      if (symbolEnd<symbolStart){
	i++;
      }
     
      scratchSymbol = (char *) malloc(sizeof(char) *
				      (symbolEnd - symbolStart + 2));
      strncpy(scratchSymbol,
	      &expression[symbolStart],
	      (symbolEnd-symbolStart+1));
      
      scratchSymbol[symbolEnd-symbolStart+1] = '\0';
      newValue = (Value *)malloc(sizeof(Value));
      newValue->type = symbolType;
      newValue->symbolValue = scratchSymbol;
      insertCell(list, newValue);
      // USE ME ================== I'M A SYMBOL
	break;
    }

    i++;

  }
  if (!list->head){
    return NULL;
  }
  reverse(list);
  return list;   
}




