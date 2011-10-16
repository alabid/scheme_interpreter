# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <ctype.h>
# include "tokenizer.h"


// debugging macros
# define number(num_args) \
  printf("The number of args received is: %d\n\n", num_args)

# define printString(string) \
  printf("[StringToTokenize]=%s\n", string)

# define printChar(char) \
  fprintf(stderr, ";%c;", char)

// end of debugging macros

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

/*
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
	double dblNumber = 0.0;
	int j = i;
	int isFloat = 0;
	int isInt = 1;
	int fractionCounter = 1;
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
	  number = number*10 + tmp[j]-'0';
	  if (isFloat){
	    dblNumber = dblNumber + (tmp[j]-'0')*pow(10,(-fractionCounter));
	    fractionCounter++;
	  }else{
	    dblNumber = dblNumber*10+tmp[j]-'0';
	  }
	  
	}
	if (isFloat){
	  newValue->type = floatType;
	  newValue->dblType = dblNumber;   
	
	}else if (isInt){
	  newValue->type = integerType;
	  newValue->intValue = number;  
	}else{
	  newValue->type = procedureType;
	  newValue->procedureValue = tmp;
	  //procedure here.
	}
	insertCell(tokens, newValue);
	break;
	
	// Code needed to check what tmp is. E.g. whether tmp is an int, a float, or a procedure(a function).
      }
  }
  return tokens;
}
*/
List *tokenize(char *expression) {
  printString(expression);
  
  int i; // counter for expression

  int numOpens,
  numCloses, 
  numStrings,
  numOpers,
  numSymbols,
    numFloats,
    numInts, numBools,numOthers;

  int numStart, numEnd, stringStart,
  symbolStart, symbolEnd;// record the start and ends of the floats

  numOpens = numCloses = numStrings = numSymbols = numInts = numBools
    = numOthers = numOpers = numFloats = 0;

  numStart = numEnd = symbolStart = symbolEnd = stringStart = 0;

  i = 0;
  
  float floatNum;
  int intNum;
  char scratchFloat[256]; // I assume there isn't any float that
  char scratchString[256]; // same thing for string
  // has more than 256 in length
  char scratchInt[256]; // same thing for int
  char *scratchSymbol;
  char *stringStore;
  int isFloat = 0;
  int notDigit = 0;
  int isCommentInDig = 0;
 


  Value *newValue;
  List *list = (List *)malloc(sizeof(List));
  // now while loop that does main work

  while (expression[i] != '\n') {
    printf("\n");
    if (isspace(expression[i])) {
      i++; continue;
    }
    printChar(expression[i]);
    printf("\ti=%d\n", i);


    if (expression[i] == ';') {
      break; // comment so get out
    }
    if (isdigit(expression[i])) {
      printf("[digit]\n");
      printf("digit starts with %c", expression[i]);
      numStart = i;

      // handle numbers here
      while (!isspace(expression[i]) 
	     && expression[i] != ')'
	     && expression[i] != '(') {
	printChar(expression[i]);

	if (expression[i] == ';') {
	  // here we know that there is a comment
	  isCommentInDig = 1;
	  break;
	}
	if (expression[i] == '.') {
	  isFloat = 1; i++; continue;
	} 
	if (!isdigit(expression[i])) {
	  // then we know it's not a digit but a symbol
	  notDigit = 1;
	  printf(" not a digit bcos it has %c", 
		 expression[i]);
	}
	i++;
      }
      
      numEnd = i-1;
      printf(" and ends with %c\n", expression[numEnd]);

     printChar(expression[i]);
       if (isFloat && !notDigit) {
	 
	 strncpy(scratchFloat,
		 &expression[numStart],
		 numEnd - numStart + 1);
         scratchFloat[numEnd-numStart +1] = '\0';
         floatNum = atof(scratchFloat);
	 printf("and this is my float value: %f", floatNum);
	 // ====== USE ME ======== I'M A FLOAT
	 // USE ME IN FLOATNUM
 	 
	 newValue = (Value *)malloc(sizeof(Value));
	 newValue->type = floatType;
	 newValue->dblValue = floatNum;
	 insertCell(list, newValue);
	numFloats++;
       } else if (!isFloat && !notDigit) {
	 strncpy(scratchInt,
	         &expression[numStart],
	 	 numEnd - numStart + 1);
        scratchInt[numEnd-numStart + 1] = '\0';
        intNum = atoi(scratchInt);
	
	printf("And this is my int value: %d", intNum);	
	// ========== USE ========== I'M AN INT
	// USE ME IN INTNUM
	newValue = (Value *)malloc(sizeof(Value));
	newValue->type = integerType;
	newValue->intValue = intNum;
	insertCell(list, newValue);
	
	numInts++;
       } 

       if (notDigit) {
	 // if it's not a digit, then it's probably
	 // a symbol
	 printf("[symbol] not [int]\n");
	 // ======= USE ME ======= I'M A SYMBOL
	 int length = numEnd - numStart + 1;
	 scratchSymbol = (char *)malloc(sizeof(char)  * (length+1));
	 
	 strncpy(scratchSymbol,
		 &expression[numStart],
		 length);
	 // I START AT NUMSTART AND END AT NUM END
	 scratchSymbol[length] = '\0';
	 newValue = (Value *)malloc(sizeof(Value));
	 newValue->type = symbolType;
	 newValue->symbolValue = scratchSymbol;
	 insertCell(list, newValue);
       }

       if (isCommentInDig) {
	isCommentInDig = 0;
	break; 
      }

       isFloat = notDigit = 0;
      continue;
    }
    
    // == GOING INTO THE SWITCH STATEMENT ==
    switch(expression[i]) { // these are for chars and strings
      case '(':
        printf("[openParens]\n");
	numOpens++;
	// === USE ME === OPEN PARENS
	 newValue = (Value *)malloc(sizeof(Value));
	 newValue->type = openType;
	 newValue->open = '(';
	 insertCell(list, newValue);
	break;
    case ')':
      printf("[closeParens]\n");
      numCloses++;
      // ======= USE ME ======== CLOSE PARENS
       newValue = (Value *)malloc(sizeof(Value));
       newValue->type = closeType;
       newValue->close = ')';
       insertCell(list, newValue);
      break;
    case '"':
      printf("[string]\n");
      stringStart = i;
      printf("string starts with %c", expression[i+1]);
      while (expression[++i] != '"');

      printf(" and ends with %c", expression[i]);
      numStrings++;

      strncpy(scratchString, 
	      &expression[stringStart],
	      i - stringStart);
      stringStore = (char *)malloc(sizeof(char) * (i-stringStart+2));
      strncpy(stringStore,
	      &expression[stringStart],
	      i - stringStart+1);
      stringStore[i-stringStart +1] = '\0'; // terminate the string
      scratchString[i-stringStart +1] = '\0'; // terminate the string
      
      printf("\nYo, string=%s\n", scratchString);
      // ======== USE ME ========== STRING
      // I'm in scratchString
       newValue = (Value *)malloc(sizeof(Value));
       newValue->type = stringType;
       newValue->stringValue = stringStore;
       insertCell(list, newValue);
      // reset scratch string after use
      // clearStringContents(scratchString);
      break;
    case '#':
      if (expression[i+1] == 'f' ) {
        printf("[boolean]\n");
	newValue = (Value *)malloc(sizeof(Value));
	newValue->type = booleanType;
	newValue->boolValue = 0;
	insertCell(list, newValue);
	
	i++;
	numBools++;
      }else if (expression[i+1] == 't') {
	printf("[boolean]\n");
	newValue = (Value *)malloc(sizeof(Value));
	newValue->type = booleanType;
	newValue->boolValue = 1;
	insertCell(list, newValue);
	i++;
	numBools++;
      }
      break;
      
    default:
      // every other thing is a symbol
      symbolStart = i;
      printf("[symbol]\n");
      printf("starts with %c\t", expression[i]);

      while (expression[i] != '(' &&
	     expression[i] != ')' &&
	     expression[i] != ']' &&
	     expression[i] != ']' &&
	     expression[i] != ';' &&
	     expression[i] != '#' &&
	     expression[i] != '"' &&
	     !isspace(expression[i])) {
	i++;
      }
      i--; // take me to the end of the symbol
      printf("ends with %c\n", expression[i]);
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
      
      numSymbols++;
      // USE ME ================== I'M A SYMBOL
	break;
    }

    i++;

  }
  printf("\n==========RESULT=========\n");
  printf("Found %d opens, %d closes\n", numOpens, numCloses);
  printf("Found %d ints and %d floats\n", numInts, numFloats);
  printf("Found %d symbol(s)\n", numSymbols);
  printf("Found %d boolean(s)\n", numBools);
  printf("Found %d string(s)\n", numStrings);
  printf("=============================\n\n");
  
  reverse(list);
  return list;
}




