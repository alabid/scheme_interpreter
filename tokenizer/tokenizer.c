#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"



// This function initializes a linked list. It will assign head as NULL.
void initialize(List *list){
  list->head = NULL;
}

int insertCell(List *list, Value *value){
  Value *newValue = (Value *)malloc(sizeof(Value));
  ConsCell *newCell = (ConsCell *)malloc(sizeof(ConsCell));
  newValue->type = cellType;
  newValue->cons = newCell;
  newCell->car = value;
  newCell->cdr = list->head;
  list->head = newValue;
  return 1;
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

void printToken(List* list){
  Value *curValue = list->head;
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
void cleanup(List* list){
  Value *second;
  while (list->head){
    second = (list->head->cons)->cdr;
    if (list->head->cons->car->type == stringType){
      free(list->head->cons->car->stringValue);
    }else if (list->head->cons->car->type == symbolType){
      free(list->head->cons->car->symbolValue);
    }
    free(list->head->cons->car);
    free(list->head->cons);
    free(list->head);
    list->head = second;
  }
}

// This function frees its cons cells and also frees the list.
void destroy(List* list){
  cleanup(list);
  free(list);
}

/* This function creates a new struct __Value and
   copys the payload stored in the Value value. 
   If value's payload contains a pointer, it only performs shallow copy.*/
Value* copyValue(Value *value){
  Value *newValue = (Value *)malloc(sizeof(Value));
  newValue->type = value->type;
  switch (value->type)
    {
    case booleanType:
      newValue->boolValue = value->boolValue;
      break;
    case integerType:
      newValue->intValue = value->intValue;
      break;
    case floatType:
      newValue->dblValue = value->dblValue;
      break;
    case stringType:
      
      newValue->stringValue = value->stringValue;
      break;
    case symbolType:
 
      newValue->symbolValue = value->symbolValue;
      break;
    case openType:
      newValue->open = value->open;
      break;
    case closeType:
      newValue->close = value->close;
      break;
    case cellType:
      newValue->cons = value->cons;
      break;
    default:
      break;
    }
  return newValue;  
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
  List *list = (List *)malloc(sizeof(List));
  // now while loop that does main work

  while (expression[i] != '\n') {
    if (isspace(expression[i])) {
      i++; continue;
    }

    if (expression[i] == ';') {
      break; // comment so get out
    }
    if (isdigit(expression[i])||expression[i]=='.'
	||expression[i]=='-' || expression[i]=='+') {
      numStart = i;
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
	  isFloat=1; i++; continue;
	}
	
	if (!isdigit(expression[i])) {
	  // then we know it's not a digit but a symbol
	  notDigit = 1;
	}
	i++;
      }
      
      i--;
      numEnd = i;
      if (numStart == numEnd && !isdigit(expression[numStart])){
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
      // reset scratch string after use
      // clearStringContents(scratchString);
      break;
    case '#':
      boolStart= i;
      while (expression[i] != ' ' && 
	     expression[i] != ')' &&
	     expression[i] != '(' &&
	     expression[i] != ';' &&
	     expression[i] != '"' ){
	i++;
      }
      i--;
      boolEnd = i;
      if (boolEnd<=boolStart){
	i++;
	continue;
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
	     !isspace(expression[i])) {
	i++;
      }
      i--; // take me to the end of the symbol
      symbolEnd = i;
      
      scratchSymbol = (char *) malloc(sizeof(char) *
				      (symbolEnd - symbolStart + 2));
      strncpy(scratchSymbol,
	      &expression[symbolStart],
	      (symbolEnd-symbolStart+1));
      
      scratchSymbol[symbolEnd+1] = '\0';
      newValue = (Value *)malloc(sizeof(Value));
      newValue->type = symbolType;
      newValue->symbolValue = scratchSymbol;
      insertCell(list, newValue);
      // USE ME ================== I'M A SYMBOL
	break;
    }

    i++;

  }

  reverse(list);
  return list;
}




