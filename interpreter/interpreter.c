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
      ; // simple hack to fix GCC problem

      Value *operator = car(expr);
      Value *args = cdr(expr);
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
  if (value->type == booleanType || integerType || floatType || stringType || closureType){
    return 1;
  }
  else{
    if(value->type == symbolType){ 
      return 2;
    }
    else{
      return 0;
  }
}
}



Value* evalDefine(Value* args, Environment* env){
  if (args == NULL||args->cons->cdr== NULL){
    printf ("syntax error: missing components here");
    return NULL;
}
  else{
    if (lookup(env->bindings->tableValue, args->cons->car)){
      deleteItem(env->bindings->tableValue, args->cons->car);
    }
    if (typeCheck(args->cons->cdr) == 1){
      insertItem(env->bindings->tableValue, args->cons->car, args->cons->cdr);
      return NULL;
    }
    else{
      if ((typeCheck(args->cons->cdr == 2))){ 
	if (lookup(env->bindings, args->cons->cdr)){
	  insertItem(env->bindings->tableValue, args->cons->car, lookup(env->bindings->tableValue, args->cons->cdr));
	  return NULL;
	   }
	else{
	  printf("syntax error: unknown identifier");
	  return NULL;
      }
      else{
	printf("syntax error;the component is undefinable");
	return NULL;
      }	
     }
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


