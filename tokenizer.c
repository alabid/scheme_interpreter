#include <stdlib.h>
#include <stdio.h>
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
	  printf("True:boolean\n");
	}
	else{
	  printf("False:boolean\n");
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

List* tokenize(char* expression){
  int i=0;
  List *tokens = (List *)malloc(sizeof(List));
  initialize(tokens);
  Value *newValue;
  while(i<MAX && expression[i]!='\0'){
    switch(expression[i])
      {
      case ' ':
	i++;
	break;
      case '(':
	newValue = (Value *)malloc(sizeof(Value));
	newValue->type = openType;
	newValue->open = '(';
	insertCell(tokens, newValue);
	break;
      case ')':
	newValue = (Value *)malloc(sizeof(Value));
	newValue->type = closeType;
	newValue->close = ')';
	insertCell(tokens, newValue);
	break;
      case '#':
	i++;
	if(expression[i]=='t'){
	  newValue = (Value *)malloc(sizeof(Value));
	  newValue->type = booleanType;
	  newValue->boolValue = 1;
	}else if(expression[i]=='f'){
	  newValue = (Value *)malloc(sizeof(Value));
	  newValue->type = booleanType;
	  newValue->boolValue = 0;
	}
	// Ignored the case #\a (a is any character.)
	else{
	  //error catching needed.
	}
	break;
      case ';':
	return tokens;
	break;
      case '"':
	char *tempString = (char *)malloc(sizeof(char)*MAX);
	int j = i;
        tempString[i-j]='"';
	i++;
	while(expression[i]!='"'){
	  if(expression[i]=='\0'){
	    // error catching needed.
	    return tokens;
	  }
	  tempString[i-j]=expression[i];
	  i++; 
	}
	tempString[i-j]='"';
	tempString[i-j+1]='\0';
	newValue = (Value*)malloc(sizeof(Value));
	newValue->type=stringType;
	newValue->stringValue=tempString;
	i++;
	break;
	
      case '\'':
	char *tempSymbol = (char *)malloc(sizeof(char)*MAX);
        int j = i;
        i++;
        while(expression[i]!=' '){
	  if(expression[i]=='\0'){
	    // error catching needed.
	    return tokens;
	  }
          tempSymbol[i-j]=expression[i];
          i++;
        }
        tempSymbol[i-j+1]='\0';
        newValue = (Value*)malloc(sizeof(Value));
        newValue->type=symbolType;
        newValue->symbolValue=tempSymbol;
        break;

	
      default:
	char *tmp = (char *)malloc(sizeof(char)*MAX);
	int number = 0;
	int j = i;
	int isFloat = 0;
	int isInt = 1;
	newValue = (Value *)malloc(sizeof(Value));
	while(expression[i]!=' ' && expression[i]!=';' && expression[i]!='(' && expression[i]!='"'){
	  *(tmp+i-j) = expression[i];
	  i++;
	}
	*(tmp+i)='\0';
	j = 0;
	while (tmp[j]!='\0'){
	  if (tmp[j]=='.'){
	    isFloat = 1;
	    isInt = 0;
	  }else if (! ('0'<tmp[j] && tmp[j]<'9')){
	    isInt = 0;
	    isFloat = 0;
	    break;
	  }
	}
	if (isFloat){
	  newValue->type = floatType;
	  newValue->dblType =;   
	}else if (isInt){

	}else{
	  //procedure here.
	}

	}
	break;
	
	// Code needed to check what tmp is. E.g. whether tmp is an int, a float, or a procedure(a function).
      }
  }
  return tokens;
}
