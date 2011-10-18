# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>

/*
  main_parse.c --

  Helps with recognizing the types in a string before
  inserting into a linked list.
  Handles all cases. Recognizes strings, booleans,
  ints, floats, symbols, and others.

  Run like this:
  ==============================================
  gcc -o main_parse main_parse.c
  cat test-token.txt | ./main_parse
  ==============================================

  I have a lot of print statements here that you could
  remove. But pls. keep them if you can for debugging.

  So far, this code works for all drracket quirks!
 */

// debugging macros
# define number(num_args) \
  printf("The number of args received is: %d\n\n", num_args)

# define printString(string) \
  printf("[StringToTokenize]=%s\n", string)

# define printChar(char) \
  fprintf(stderr, ";%c;", char)

// end of debugging macros

void print_args(int argc, char **argv);
void readAndPrintTokens(char *expression);


int main(int argc, char *argv[]) {
  char* expression = (char *) malloc(sizeof(char) *256);
  
  while (fgets(expression, 256, stdin)) {
    readAndPrintTokens(expression);
  }

  free(expression);

  return 0;
}

void print_args(int argc, char **argv) {
  int i; 

  for (i=1; i< argc; i++) {
    printf("The %dth argument is: %s\n", i, argv[i]);
  }
}

void readAndPrintTokens(char *expression) {
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
  int isFloat = 0;
  int notDigit = 0;
  int isCommentInDig = 0;
 

  // now while loop that does main work

  while (expression[i] != '\n') {
    printf("\n");
    if (isspace(expression[i])) {
      i++; continue;
    }
    printChar(expression[i]);
    printf("\ti=%d\n", i);
    // handle numbers here

    if (expression[i] == ';') {
      break; // comment so get out
    }
    if (isdigit(expression[i])) {
      printf("[digit]\n");
      printf("digit starts with %c", expression[i]);
      numStart = i;
      // printChar(expression[i]);
      // so now you can check for '.'
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
	 floatNum = atof(strncpy(scratchFloat,
	 			&expression[numStart],
				numEnd - numStart + 1));
	 printf("and this is my float value: %f", floatNum);
	 // ====== USE ME ======== I'M A FLOAT
	 // USE ME IN FLOATNUM
	numFloats++;
       } else if (!isFloat && !notDigit) {
	 intNum = atoi(strncpy(scratchInt,
	 		      &expression[numStart],
	 		      numEnd - numStart + 1));
	printf("And this is my int value: %d", intNum);	
	// ========== USE ========== I'M AN INT
	// USE ME IN INTNUM
	numInts++;
       } 

       if (notDigit) {
	 // if it's not a digit, then it's probably
	 // a symbol
	 printf("[symbol] not [int]\n");
	 // ======= USE ME ======= I'M A SYMBOL
	 // I START AT NUMSTART AND END AT NUM END
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
	break;
    case ')':
      printf("[closeParens]\n");
      numCloses++;
      // ======= USE ME ======== CLOSE PARENS
      break;
    case '"':
      printf("[string]\n");
      stringStart = i+1;
      printf("string starts with %c", expression[i+1]);
      while (expression[++i] != '"');

      printf(" and ends with %c", expression[i-1]);
      numStrings++;

      strncpy(scratchString, 
	      &expression[stringStart],
	      i - stringStart);
      scratchString[i-stringStart] = '\0'; // terminate the string

      printf("\nYo, string=%s\n", scratchString);
      // ======== USE ME ========== STRING
      // I'm in scratchString

      // reset scratch string after use
      // clearStringContents(scratchString);
      break;
    case '#':
      if (expression[i+1] == 'f' || expression[i+1] == 't') {
        printf("[boolean]\n");
	i++;
	// ============ USE ME ======== I'M A BOOLEAN
	numBools++;
      }
      break;
      // don't break here, because it might just be a symbol
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
}




