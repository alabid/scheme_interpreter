#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
Memory to free:
follow execution starting from main.
Have a table at the side where you keep track of memory allocated on the
heap and then try to deal with each of them.
One by one.
*/
// I have made quote, if, define, literal work well with our changes.
// I also implement freeValue() function which can help determine which parts of memory need to be freed.

// This function evaluates the parse tree given by expr within the given environment.
Value* eval(Value *expr, Environment *env){
  Value* operator;
  Value* args;
  //printf("Here is expression:  ");
  //printValue(expr);
  //printf("\n");

  if (!expr){
    return NULL;
  }

  switch (expr->type) 
    {
    case symbolType:
      //printf("unknown identifier");
      args = envLookup(expr->symbolValue, env);
      if (args){
	return args;
      }else{
	printf("syntax error: unknown identifier");
	return NULL;
      }
      break;
    case cellType:
      if (expr->cons->car->type == nullType) {
	return expr->cons->car;
      }
      //printf("Here is expression:  ");
      //printValue(getFirst(expr));
      //printf("\n");
      
      if (getFirst(expr) != NULL && getFirst(expr)->type == openType) {
	operator = car(expr);
	args = getTail(getTail(expr));
	
	if (!operator){
	  printf("syntax error, missing components here");
	  return NULL;
	}
	
	 if (operator->type == symbolType){
	   if (args  == NULL){
	    return eval(operator,env);
	   }else if (strcmp(operator->symbolValue,"define")==0){
	     return evalDefine(args, env);
	   }else if (strcmp(operator->symbolValue,"lambda")==0){
	     /*eval lambda goes here*/
	     return evalLambda(args, env);
	   }else if (strcmp(operator->symbolValue,"if")== 0){
	    
	     return evalIf(args, env);
	     /*eval if goes here*/
	   }else if (strcmp(operator->symbolValue,"quote")==0){
	     /*eval quote goes here*/
	     return evalQuote(args);
	   }else if (strcmp(operator->symbolValue,"let")==0){
	     /*eval let goes here*/
	     return evalLet(args, env);
	   }
	}else if (typeCheck(operator)==1){
	  printf("A literal ");
	  printValue(operator);
	  printf(" cannot be a procedure.\n");
	  return NULL; 
	}else{
	  Value *evaledOperator = eval(operator, env);
	  Value *evaledArgs = evalEach(args, env);
	  return apply(evaledOperator, evaledArgs);
	}
       } else if (typeCheck(getFirst(expr))==1){
	//printValue(expr);
	//printf("\n");
	return evalEach(expr,env);
      }else if (getFirst(expr) && getFirst(expr)->type ==cellType && getFirst(getTail(expr)) && getFirst(getTail(expr))->type==closeType){
	  return eval(getFirst(expr),env);
       }else if (getFirst(expr) && getFirst(expr)->type == symbolType){
	 operator = getFirst(expr);
	 Value *returnValue = envLookup(operator->symbolValue, env);
	 if (returnValue){
	   
	   return returnValue;
	 }else{
	   if (strcmp(operator->symbolValue,"define")==0){
	     printf("define: bad syntax in ");
	     printValue(expr);
	     printf("\n");
	   }else if (strcmp(operator->symbolValue,"lambda")==0){
	     printf("lambda: bad syntax in ");
	     printValue(expr);
	     printf("\n");
	   }else if (strcmp(operator->symbolValue,"if")==0){
	     printf("if: bad syntax in ");
	     printValue(expr);
	     printf("\n");
	   }else if (strcmp(operator->symbolValue,"quote")==0){
	     printf("quote: bad syntax in ");
	     printValue(expr);
	     printf("\n");
	   }else if (strcmp(operator->symbolValue,"let")==0){
	     printf("let: bad syntax in ");
	     printValue(expr);
	     printf("\n");
	   }else{
	    printf("Unknown identifier %s.\n",getFirst(expr)->symbolValue);
	   }
	  
	   return NULL;
	 }
       }
       
    case closeType:
      //printValue(expr);
      //printf("\n");
      return NULL;
      break;
    default:
      //printValue(expr);
      //printf("\n");
      if (getTail(expr)){
	assert(getFirst(getTail(expr))!=NULL);
	assert(getFirst(getTail(expr))->type==closeType);
	Value *toRemove = getTail(expr);
	free(toRemove->cons->car);
	free(toRemove->cons);
	free(toRemove);
	expr->cons->cdr = NULL;
	assert(1==2);
      }
      return expr;      
    }
}

/*
  Quote function works well.
*/
Value* evalQuote(Value* args){
  if (getFirst(args) && getFirst(args)->type==closeType){
    printf("quote: bad syntax (wrong number of parts) in: (quote)");
    return NULL;
    // since there is one argument list and one close parenthese, the listLength should return zero.
  }else if (listLength(args) != 1){
    printf("quote: bad syntax (wrong number of parts) in: (quote ");
    printValue(args);
    printf("\n");
    return NULL;
  }
  else{
    assert(getFirst(getTail(args))!=NULL);
    assert(getFirst(getTail(args))->type==closeType);
    Value *toRemove = getTail(args);
    free(toRemove->cons->car);
    free(toRemove->cons);
    free(toRemove);
    args->cons->cdr = NULL;
    return args;
  }
}


// We have not tested this function yet for part a.
Value* apply(Value* function, Value* actualArgs){
  if (!actualArgs){
    return function;
  }else if (!function){
    return actualArgs;
  }else if (function->type == primitiveType){
    return function->primitiveValue(actualArgs);
  }else if (function->type == closureType){
    Value *formalArgs = function->closureValue->args;
    eval(formalArgs, function->closureValue->parent);
    Environment *frame = createFrame(function->closureValue->parent);
    return eval(function->closureValue->body, frame);
  }else{
    printf("Unknown identifier!");
    return NULL;
  }
}

// look at the identifier.
Value* envLookup(char* id, Environment* env){
  Value* returnValue = NULL;
  while (env){
  
    assert(env->bindings->type == tableType);
    returnValue = lookup(env->bindings->tableValue, id);
    if (returnValue){
      break;
    }else{
      env = env->parent;
    }
  }
  return returnValue;
}

// group types into three main groups.
int typeCheck(Value* value){
  if (value){
    switch (value->type)
      {
      case booleanType:
      case integerType:
      case floatType:
      case stringType:
	return 1;
	break;
      case cellType:
      case closureType:
	return 2;
	break;
      case symbolType:
	return 3;
	break;
      default:
	return 0;
    }
  }
  else{
    return -1;
  }
}

// Check special cases.
int variableCheck(Value* value){
  if (value){
    if (value->type == symbolType){
      if (strcmp(value->symbolValue,".")==0 ||
	  strcmp(value->symbolValue,"+")==0 ||
	  strcmp(value->symbolValue,"-")==0 ||
	  strcmp(value->symbolValue,"*")==0 ||
	  strcmp(value->symbolValue,"/")==0 ||
	  strcmp(value->symbolValue,"<")==0 ||
	  strcmp(value->symbolValue,"=")==0 ||
	  strcmp(value->symbolValue,">")==0 ||
	  strcmp(value->symbolValue,"car")==0 ||
	  strcmp(value->symbolValue,"cdr")==0 ||
	  strcmp(value->symbolValue,"list")==0)
	{
	  return 0;
	}else{
	  return 1;
	}
    }else{
      return -1;
    }
  }
  else{
    return -1;
  }
}	  

      


// This function evaluates define by creating bindings. It always returns NULL.
Value* evalDefine(Value* args, Environment* env){
  //printf("I'm in the evalDefine\n");
  if (args == NULL||args->cons->cdr== NULL){
    printf("syntax error: missing components here\n");
    return NULL;
  }
  assert(args->type == cellType);
  
  //check if there are more than 2 values after define
  if (listLength(args) > 2){
    printf("syntax error: multiple expressions after identifier\n");
    return NULL;
  }
  //check if the variable is valid
  if (variableCheck(getFirst(args)) < 1){
    if (variableCheck(getFirst(args)) < 0){
      printf("bad syntax\n");
      return NULL;
    }
    else{
      printf("cannot change constant variable\n");
      return NULL;
    }
  }else{
    // now it has correct number of arguments and the identifier is valid.
    // eval the second argument and put (1st, 2nd) as key-value pair in the hash table. 
    
    assert(env!=NULL);
    assert(env->bindings->type == tableType);
    
    assert(args->type==cellType);
    assert(getFirst(args)!=NULL);
    assert(getFirst(args)->type == symbolType);
    
    if (getFirst(getTail(args)) && getFirst(getTail(args))->type == symbolType){
      Value *value = eval(envLookup(getFirst(getTail(args))->symbolValue, env), env);
      if (value){
	insertItem(env->bindings->tableValue, (getFirst(args))->symbolValue, value);
      }else{
	printf("syntax error: unknown identifier\n");
	return NULL;
      }
    }else{
      
      assert(getFirst(args)!=NULL);
      assert(getFirst(args)->type==symbolType);
      assert(env->bindings->tableValue!=NULL);
      assert(getFirst(args)->symbolValue);
      //printf("printing out value");
      //printValue(eval(getTail(args),env));
      //printf("\n");
      insertItem(env->bindings->tableValue, getFirst(args)->symbolValue, eval(getFirst(getTail(args)), env));
      
    }
    return NULL;
  }  
}

// We have not finished this function yet.
Value* evalEach(Value* args, Environment* env){
  Value *temp, *toClean, *tail, *previous = args;
  Value *head = args;
  
  while (args){ 
    assert(args->type==cellType);    
    //temp = args->cons->car;
    if (getFirst(args) && (getFirst(args))->type==cellType){
      tail = getTail(args);
      temp =  deepCopy(eval(getFirst(args), env));
      
      assert(getFirst(getFirst(args))!=NULL);
      assert(getFirst(getFirst(args))->type==openType);
      //if (temp){
	//assert(args->type==cellType);
      cleanup(getTail(getFirst(args)));
      free(getFirst(args)->cons);
      freeValue(args->cons->car);
      args->cons->car = temp;
      args = tail;
      //}
      
    }else{
    
      tail = getTail(args);
      //printf("start to print:  ");
      //printValue(eval(args->cons->car, env));
      //printf("\n");
      temp = deepCopy(eval(args->cons->car, env));
    
      if (temp==NULL){ 
	//printf("NOPE");
	cleanup(tail);
	args->cons->cdr = NULL;
	freeValue(args->cons->car);
	free(args->cons);
	free(args);
	if (args!=previous){
	  previous->cons->cdr = NULL;
	}else{
	  return NULL;
	}
	break;
      }else{
	//printValue(temp);
	//printf("\n");
	
	freeValue(args->cons->car);
	
	args->cons->car = temp;
	
      //printValue(temp);
      //printf("\n");
	previous = args;
	args = tail;
      }
    }
    
  } 
 
  return head;
}
// not finished.
Value* evalLet(Value* args, Environment* env){
  int count = listLength(args);
  if (count < 2){
    printf("sytax error: bad syntax");
    return NULL;
  }
  if (getFirst(args)-> type != cellType){
    printf("sytax error:not a sequence of indentifier");
    return NULL;
  }else{
    if (getFirst(getFirst(args))-> type != cellType){
      printf("sytax error: not an indentifier for bindings");
      return NULL;
    } 
    if (getFirst(getFirst(getFirst(args))) -> type !=symbolType){
      printf("bad syntax not an indentifier");
      return NULL;
    }else{
      Environment* newenv = createFrame(env);
      Value* listofBinds = getFirst(args);
      while(listofBinds){
	  Value* toBind = eval(getTail(getFirst(listofBinds)), newenv);
	  if (typeCheck(toBind) == 1 ||toBind ->type == closureType){
	    insertItem(newenv->bindings->tableValue, (getFirst(getFirst(listofBinds)))->symbolValue, toBind);
	  }else if(toBind->type == symbolType){
	    Value *value = eval(envLookup(toBind->symbolValue, newenv), env);
	    if (value){
	      insertItem(newenv->bindings->tableValue, toBind->symbolValue, value);
	    }else{
	      printf("syntax error: unknown identifier\n");
	      free(toBind);
	      free(value);
	      return NULL;
	    }
	  }else{
	    printf("sytax error");
	  }
	  listofBinds = getTail(listofBinds);
      }
      Value* listofExpressions = getTail(args);
      while(listofExpressions){
	Value* toReturn = eval(getFirst(listofExpressions), newenv);
	if(typeCheck(toReturn) != 3){
	  if(getTail(listofExpressions)){
	    listofExpressions = getTail(listofExpressions);
	  }else{
	    return toReturn;
	  }
	}else{
	  
	  ; 
	
      }
	;  
}
// This function evaluates if statement.
Value* evalIf(Value* args, Environment* env){
  int count = listLength(args);
  
  if (count < 2) {
    printf("syntax error: too few arguments in if statement\n");
    return NULL;
  } if ( count > 3) {
      printf("syntax error: too many arguments in if statement\n");
      return NULL;
  }
  Value *evalTest = eval(getFirst(args), env);
  
 if (evalTest && evalTest->type == booleanType && !(evalTest->boolValue)) {
    if (count == 3) {
      return eval(getFirst(getTail(getTail(args))), env);
   }
   else 
     return  NULL; // DRracket doesn't return a '(), it returns nothing (NULL)
 }
 else {
    return eval(getFirst(getTail(args)), env); // return eval(alternate)
 }
}

// not finished yet.
Value* evalLambda(Value* args, Environment* env){
  return NULL;
}

// 
Value *makePrimitiveValue(Value* (*f)(Value *)){
  Value *resultValue = (Value *)malloc(sizeof(Value));
  resultValue->type = primitiveType;
  resultValue->primitiveValue = f;
  return resultValue;
}
// creates a top-level frame and binds the "built-in" functions.
Environment *createTopFrame(){
  Environment *frame = createFrame(NULL);
  // bind("+", makePrimitiveValue(add), frame);
  // bind("-", makePrimitiveValue(subtract), frame);
  // bind("*", makePrimitiveValue(multiply), frame);
  // bind("/", makePrimitiveValue(divide), frame);
  // bind("exp", makePrimitiveValue(exponentiate), frame);
  // bind("load", makePrimitiveValue(loadFunction), frame);
  // bind("car", makePrimitiveValue(car), frame);
  // bind("cdr", makePrimitiveValue(cdr), frame);
  return frame;
}

// creates an environment.
Environment *createFrame(Environment *parent){
  Environment *frame = (Environment *)malloc(sizeof(Environment));
  frame->parent = parent;
  HashTable *table = initializeTable(32);
  Value *value = (Value *)malloc(sizeof(Value));
  value->type = tableType;
  value->tableValue = table;
  frame->bindings = value;
  return frame;
}

// not finished yet.
Value *exponentiate(Value *args){
  return NULL;
}

// not tested yet.
Value *add(Value *args){
  int intSum = 0;
  double dblSum = 0;
  int isFloat = 0;
  Value  *result;
  while (args && args->type == cellType){
    if (getFirst(args)->type == integerType){
      if (!isFloat){
	intSum += getFirst(args)->intValue;
      }else{
	dblSum += getFirst(args)->dblValue;
      }
    }else if (getFirst(args)->type == floatType){
      if (!isFloat){
	isFloat = 1;
	dblSum +=intSum;
      }
      dblSum += getFirst(args)->dblValue;	
    }else if (getFirst(args)->type == closeType){
      break;
    }else{
      printf("+: expects type <number> as arguments");
      return NULL;
    }
    args = getTail(args);
  }
  Value *value = (Value*) malloc(sizeof(Value));
  
  cleanup(getTail(args));
  result = getFirst(args);
  if (isFloat){
    result->type = floatType;
    result->dblValue = dblSum; 
  }else{
    result->type = integerType;
    result->intValue = intSum; 
  }
  args->cons->cdr = NULL;
  return args;
}

// not finished yet.
Value *loadFunction(Value *args){
  // read in the lines of a file one at a time
  // eval each line line by line
  char *expression = (char *)malloc(256 * sizeof(char));
  int count = listLength(args);

  if (count > 1) {
    printf("load: load one file at a time\n");
    free(expression);
    cleanup(args);
    return NULL;
  } else if (count < 1) {
    printf("load: you must enter the name of a file to load\n");
    free(expression);
    cleanup(args);
    return NULL;
  }
  FILE *fp;
  if (car(args)->type == stringType) {
    fp = fopen(car(args)->stringValue, "r");

    while (fgets(expression, 256, fp)) {
      fputs(expression, stdin);
    } 
    free(expression);
    fclose(fp);
    cleanup(args);
  }  else {
    free(expression);
    cleanup(args);
    return NULL;
  }
  free(expression);
  cleanup(args);
  return NULL;
}

void bind(char *identifier, Value *function, Environment *env){
  ;
}



