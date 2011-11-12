/* By Jie Lin, Kan Wang, Daniel Alabi, Yichen Shen.
   CS 251, Fall 2011
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"
// This function initializes a linked list. It will assign head as NULL.


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

  while (expression[i]) {
    if (expression[i] == EOF) {
      printf("reached the end of line\n");
      break;
    } // so if there is any EOF then take me back
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
      while (isspace(expression[i+1])){
	i++;
      }
      newValue = (Value *)malloc(sizeof(Value));
      if (expression[i+1] == ')') {
	i++;
	newValue->type = nullType;
	insertCell(list, newValue);
      } else {
	newValue->type = openType;
	newValue->open = '(';
	insertCell(list, newValue);
      }
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




