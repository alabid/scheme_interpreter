#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
# include <assert.h>

Value* eval(Value *expr, Environment * env){
  switch (expr->type) 
    {
    case symbolType:
      return envLookup(expr->symbolValue, env);
      break;
    case cellType:
      Value *operator = car(expr);
      Value *args = cdr(expr);
      if (operator->type == symbolType){
	if (strcmp(operator->symbolValue,"define")==0){
	  return evalDefine(args, env);
	}else if (strcmp(operator->symbolValue,"lambda")==0){
	  /*eval lambda goes here*/
	  return evalLambda(args, env);
	}else if (strcmp(operator->symbolValue,"if")==0){
	  // printf("I'm here %d\n", evalTest->cons->car->boolValue);
	  return evalIf(args, env);
	  /*eval if goes here*/
	}else if (strcmp(operator->symbolValue,"quote")==0){
	  /*eval if goes here*/
	  return args;
	}else if (strcmp(operator->symbolValue,"let")==0){
	  /*eval let goes here*/
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
Value* apply(Value* function, Value* actualArgs){
  if (function->type == primitiveType){
    return function->primitiveValue(actualArgs);
  }else{
    assert(function->type == closureType);
    Value *formalArgs = function->closureValue->args;
    Environment *frame = createFrame(function->closureValue->parent);
    return eval(function->closureValue->body, frame);
  }
}

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
}

int variableCheck(Value* value){
  if (value){
    if (value->type = symbolType){
      switch(value->symbolValue)
	{
	case ".":
	case "+":
	case "-":
	case "*":
	case "#":
	case ">":
	case "=":
	case "<":
	case "car":
	case "cdr":
	case "list":
	  return 0;
	default:
	  return 1;
	}
      return -1;
    }
  }
}
	  
	  
      



Value* evalDefine(Value* args, Environment* env){
  if (args == NULL||args->cons->cdr== NULL){
    printf ("syntax error: missing components here");
    return NULL;
}
  if (cdr(cdr(cdr(args))) != NULL){
    printf ("syntax error: multiple expressions after identifier");
    return NULL;
  }
  if(variableCheck(car(args)) < 1){
    if (variableCheck(car(args)) < 0){
      printf ("bad syntax");
      return NULL;
    }
    else{
      printf("cannot change constant variable");
      return NULL;
    }
  }   
  else{
    assert(args-type == cellType);
    assert(env!=NULL);
    assert(env->bindings-type == tableType);
    assert(args->cons->car->type == symbolType);
    if (typeCheck(cdr(args)) != 3 && typeCheck(cdr(args)) >0 ){
      insertItem(env->bindings->tableValue, car(args)->symbolValue, eval(cdr(args)), env));
      return NULL;
    }
    else if(typeCheck(cdr(args)) == 3){ 
      if (envLookup(env->bindings->tableValue, cdr(args)->symbolValue)){  // use envLookUp instead. // modify the value directly.
	insertItem(env->bindings->tableValue, car(args)->symbolValue, envLookup(env->bindings->tableValue, cdr(args)->symbolValue));
	return NULL;
      }
      else{
	printf("syntax error: unknown identifier");
	return NULL;
	    }
    }
    else{
      printf("syntax error;the component is undefinable");
      return NULL;
    }	
  }
}


Value* evalEach(Value* args, Environment* env){

}

Value* evalLet(Value* args, Environment* env){

}

Value* evalIf(Value* args, Environment* env){
  Value *evalTest = eval(car(args), env);
  
 if (evalTest->type == booleanType && !(evalTest->boolValue)) {
    // if evalTest is false, then return eval(alternate)
    // if no alternate, just returns NULL
   return eval(car(cdr(cdr(args))), env);
 }
  else {
    // else return eval(consequence)
    return eval(car(cdr(args)), env); // return eval(alternate)
  }
}

Value* evalLambda(Value* args, Environment* env){

}

Value *makePrimitiveValue(Value* (*f)(Value *)){

}

Environment *createTopFrame(){
  Environment *frame = createFrame(NULL);
  // bind("+", makePrimitiveValue(add), frame);
  // bind("exp", makePrimitiveValue(exponentiate), frame);
  // bind("load", makePrimitiveValue(loadFunction), frame);
  return frame;
}

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

Value *exponentiate(Value *args){

}

Value *add(Value *args){

}

Value *loadFunction(Value *args){

}


