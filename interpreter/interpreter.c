#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


// This function evaluates the parse tree given by expr within the given environment.
Value* eval(Value *expr, Environment *env){
  Value* operator;
  Value* args;
  if (!expr){
    return NULL;
  }
  switch (expr->type) 
    {
    case symbolType:
      return envLookup(expr->symbolValue, env);
      break;
    case cellType:
      operator = car(expr);
      args = cdr(expr);
      if (operator->type == symbolType){
	if (strcmp(operator->symbolValue,"define")==0){
	  return evalDefine(args, env);
	}else if (strcmp(operator->symbolValue,"lambda")==0){
	  /*eval lambda goes here*/
	  return evalLambda(args, env);
	}else if (strcmp(operator->symbolValue,"if")==0){
	  return evalIf(args, env);
	  /*eval if goes here*/
	}else if (strcmp(operator->symbolValue,"quote")==0){
	  /*eval quote goes here*/
	  return args->cons->car;
	}else if (strcmp(operator->symbolValue,"let")==0){
	  /*eval let goes here*/
	  return evalLet(args, env);
	}else if (envLookup(operator->symbolValue, env)){
	  return envLookup(operator->symbolValue, env);
	}else{
	  Value *evaledOperator = eval(operator, env);
	  Value *evaledArgs = evalEach(args, env);
	  return apply(evaledOperator, evaledArgs);
	}
      }
    default:
      return expr;
      
    }
}


// We have not tested this function yet for part a.
Value* apply(Value* function, Value* actualArgs){
  if (function->type == primitiveType){
    return function->primitiveValue(actualArgs);
  }else if (function->type == closureType){
    Value *formalArgs = function->closureValue->args;
    eval(formalArgs, function->closureValue->parent);
    Environment *frame = createFrame(function->closureValue->parent);
    return eval(function->closureValue->body, frame);
  }else{
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
  if (args == NULL||args->cons->cdr== NULL){
    printf("syntax error: missing components here\n");
    return NULL;
  }
  
  //check if there are more than 2 values after define
  if (cdr(cdr(args)) != NULL){
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
    assert(args->type == cellType);
    assert(env!=NULL);
    assert(env->bindings->type == tableType);
    assert(args->cons->car->type == symbolType);
    if (cdr(args) && (cdr(args))->type == symbolType){
      Value *value = eval(envLookup(cdr(args)->symbolValue, env), env);
      if (value){
	insertItem(env->bindings->tableValue, car(args)->symbolValue, value);
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
  Value *temp;
  while (args){
    temp = args->cons->car;
    args->cons->car = eval(args->cons->car, env);
    // free memory?
    // cleanup(temp);
    args = cdr(args);
  }
  return args;
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


