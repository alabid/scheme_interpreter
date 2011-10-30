#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Value* eval(Value *expr, Environment * env){
  switch (expr->type) 
    {
    case symbolType:
      return envLookup(expr->symbolValue, env);
      break;
    case cellType:
      Value *operator = expr->cons->car;
      Value *args = expr->cons->cdr;
      if (operator->type == symbolType){
	if (strcmp(operator->symbolType,"define")==0){
	  return evalDefine(args, env);
	}else if (strcmp(operator->symbolType,"lambda")==0){
	  /*eval lambda goes here*/
	  return evalLambda(args, env);
	}else if (strcmp(operator->symbolType,"if")==0){
	  /*eval if goes here*/
	}else if (strcmp(operator->symbolType,"let")==0){
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

Value* evalDefine(Value* args, Environment* env){

}

Value* evalEach(Value* args, Environment* env){

}

Value* evalLet(Value* args, Environment* env){

}

Value* evalIf(Value* args, Environment* env){

}

Value* evalLambda(Value* args, Environment* env){

}

Value *makePrimitiveValue(Value* (*f)(Value *)){

}

Environment *createTopFrame(){
  Environment *frame = createFrame(NULL);
  bind("+", makePrimitiveValue(add), frame);
  bind("exp", makePrimitiveValue(exponentiate), frame);
  bind("load", makePrimitiveValue(loadFunction), frame);
  return frame;
}

Environment *creatFrame(Environment *parent){
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


