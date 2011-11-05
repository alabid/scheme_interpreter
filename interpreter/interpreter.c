#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


// This function evaluates the parse tree given by expr within the given environment.
Value* eval(Value *expr, Environment *env){
  Value* operator;
  Value* args;
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

      operator = car(expr);
      args = cdr(expr);

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
	   //printf("The length is %d\n",listLength(expr));
	   return evalQuote(args);
	 }else if (strcmp(operator->symbolValue,"let")==0){
	   /*eval let goes here*/
	   return evalLet(args, env);
	 }
      if (typeCheck(operator)==1){
	printf("A literal ");
	printValue(operator);
	printf(" cannot be a procedure.\n");
	return NULL; 
      }else{
	Value *evaledOperator = eval(operator, env);
	Value *evaledArgs = evalEach(args, env);
	return apply(evaledOperator, evaledArgs);
      }
    }else if (typeCheck(expr->cons->car)==1){
	printValue(expr);
	printf("\n");
	return evalEach(expr,env);
    }else if (expr->cons->car && expr->cons->car->type == symbolType){
	operator = car(expr);
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
	    printf("Unknown identifier %s.\n",car(expr)->symbolValue);
	  }
	  
	  return NULL;
	}
      }
    case closeType:
      return NULL;
    default:
      return expr;
      
    }
}

Value* evalQuote(Value* args){
  if (car(args) && (car(args))->type==closeType){
    printf("quote: bad syntax (wrong number of parts) in: (quote)");
    return NULL;
    // since there is one argument list and one close parenthese, the listLength should return zero.
  }else if (listLength(args)!=0){
    //printf("The length is %d\n",listLength(args));
    printf("quote: bad syntax (wrong number of parts) in: (quote ");
    printValue(args);
    printf("\n");
    return NULL;
  }else{
    // assert(car(cdr(args))!=NULL);
    // assert(car(cdr(args))->type==closeType);
    Value *toRemove = cdr(args);
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
  printf("I'm in the evalDefine\n");
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
  if (variableCheck(car(args)) < 1){
    if (variableCheck(car(args)) < 0){
      printf("bad syntax\n");
      return NULL;
    }
    else{
      printf("cannot change constant variable\n");
      return NULL;
    }
  }else{
    assert(env!=NULL);
    assert(env->bindings->type == tableType);
    // assert(car(args)->type == symbolType);
    if (args->cons->cdr && car(cdr(args))->type == symbolType){
      Value *value = eval(envLookup((cdr(args))->symbolValue, env), env);
      if (value){
	insertItem(env->bindings->tableValue, (car(args))->symbolValue, value);
      }else{
	printf("syntax error: unknown identifier");
	return NULL;
      }
    }else{
      insertItem(env->bindings->tableValue, car(args)->symbolValue, eval(cdr(args), env));
    }
	return NULL;
  }  
}

// We have not finished this function yet.
Value* evalEach(Value* args, Environment* env){
  Value *temp, *toClean;
  Value *head = args;

  while (args){ 
    assert(args->type==cellType);    
    //temp = args->cons->car;
    if (car(args) && (car(args))->type==openType){
      temp =  eval(args, env);
      if (temp){
	assert(args->type==cellType);
	args->cons->car = temp;
	args = cdr(args);
      }
      while (car(args) && car(args)->type!=closeType){
      //args->cons->car = temp;
      //printValue(temp);
      //printf("\n");
	toClean = cdr(args);
 	free(car(args));
	assert(args->type==cellType);
	//free(args->cons);
	//free(args);
	args = toClean;
      }if (car(args)){
	free(car(args));
      }
      break;
    }else{
      temp =  eval(args->cons->car, env);
      if (temp){
	args->cons->car = temp;
	//printValue(temp);
	printf("\n");
      }
    // free memory?
    // cleanup(temp);
      args = cdr(args);
    }
  } 
  
  return head;
}
// not finished.
Value* evalLet(Value* args, Environment* env){
  return NULL;
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

  Value *evalTest = eval(car(args), env);
  // Value *tempArgs;
 
 if (evalTest->type == booleanType && !(evalTest->boolValue)) {
    // if evalTest is false, then return eval(alternate)
    // if no alternate, just returns NULL
   if (count == 3) {
    
     return eval(car(cdr(cdr(args))), env);
   }
   else 
     return  NULL; // DRracket doesn't return a '(), it returns nothing (NULL)
 }
 else {
    
   // else return eval(consequence)
   return eval(car(cdr(args)), env); // return eval(alternate)
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
  // bind("+", makePrimitiveValue(subtract), frame);
  // bind("+", makePrimitiveValue(multiply), frame);
  // bind("+", makePrimitiveValue(divide), frame);
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
  while (args){
    if (args->type == integerType){
      if (!isFloat){
	intSum += args->intValue;
      }else{
	dblSum += args->dblValue;
      }
    }else{ 
      if (args->type == floatType){
	if (!isFloat){
	  isFloat = 1;
	  dblSum +=intSum;
	}
	dblSum += args->dblValue;	
      }else{
	  printf("+: expects type <number> as arguments");
	  return NULL;
      }
    }
    args = cdr(args);
  }
  Value *value = (Value*) malloc(sizeof(Value));
  if (isFloat){
    value->type = floatType;
    value->dblValue = dblSum; 
  }else{
    value->type = integerType;
    value->intValue = intSum; 
  }
  return value;
}
 
// not finished yet.
Value *loadFunction(Value *args){
  return NULL;
}


void bind(char identifier, Value *function, Environment *env){
  ;
}
