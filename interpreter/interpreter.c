#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
  We have implemented literal, lambda, quote, let, letrec, define, if and load.
  We also implemented several primitive functions, like +, -, *, /.
  We tested the codes above.

  Inside the file, we also have some functions that we will test in the next phase of the
  assignment.
  car, cdr, let* Please do not test these functions this time.
*/

Value* eval(Value *expr, Environment *env){
  Value* operator;
  Value* args;

  if (!expr){
    return NULL;
  }
  switch (expr->type) 
    {
    case symbolType:   
      args = envLookup(expr->symbolValue, env);
      if (args){
	return args;
      }
      else{
	printf("syntax error: unknown identifier\n");
	return NULL;
      }
      break;
    case nullType:
      return expr;
      break;
    case cellType:
      if (expr->cons->car->type == nullType) {
	return expr->cons->car;
      }

      if (getFirst(expr) != NULL && getFirst(expr)->type == openType) {
	operator = getFirst(getTail(expr));
	args = getTail(getTail(expr));

	if (!operator){
	  printf("syntax error, missing components here\n");
	  return NULL;
	}else if (operator->type==nullType){
	  printf("procedure application: expected procedure, given: () (no arguments)\n");
	  return NULL;
	}
	if (operator->type == cellType){
	  while (operator->type == cellType){
	   
	    operator = eval(operator, env);
	    
	    if (!operator){
	      printf("Invalid procedure!\n");
	      return NULL;
	    }
	  }
	  Value *evaledArgs = evalEach(args, env);
	 
	  return apply(operator, evaledArgs, env);
	}
	if (operator->type == symbolType){  

	  if (strcmp(operator->symbolValue,"define")==0){
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
	  }else if (strcmp(operator->symbolValue,"letrec")==0){
	    /*eval letrec goes here*/
	    return evalLetrec(args, env);
	  }else if (strcmp(operator->symbolValue,"and")==0){
	    /*eval let* goes here*/
	    return evalAnd(args, env);
	  }else if (strcmp(operator->symbolValue,"or")==0){
	    /*eval let* goes here*/
	    return evalOr(args, env);
	  }else if (strcmp(operator->symbolValue,"let*")==0){
	    /*eval let* goes here*/
	    return evalLetStar(args, env);
	  }else if (strcmp(operator->symbolValue,"cond")==0){
	    return evalCond(args, env);
	  }else if (strcmp(operator->symbolValue,"set!")==0){
	    return evalSetBang(args, env);
	  }else if(strcmp(operator->symbolValue, "load") == 0){
	    while (env && env->parent){
	      env = env->parent;
	    } // go to top-level environment.
	    return loadFunction(args, env);
	  } else{
	    Value *evaledOperator = eval(operator, env);
	    Value *evaledArgs = evalEach(args, env);
	    
	    if (!evaledOperator){
	      printf("Unknown procedure: ");
	      printValue(operator);
	      printf("\n");
	      return NULL;
	    }
	    
	    Value *applyRet = apply(evaledOperator, evaledArgs, env);
	    return applyRet;
	} 
      }else if (typeCheck(operator)==1){
	printf("A literal ");
	printValue(operator);
	printf(" cannot be a procedure.\n");
	return NULL; 
      }else if (typeCheck(operator)==2){
	
	Value *evaledArgs;
	  if (args &&  getFirst(args) && getFirst(args)->type ==closeType){
	    free(getFirst(args));
	    free(args->cons);
	    free(args);
	    getTail(expr)->cons->cdr = NULL;
	    evaledArgs = NULL;

	  }else{
	    
	    evaledArgs = evalEach(args, env);
	  }
	  return apply(operator, evaledArgs, env);	  
	}
      }else if (typeCheck(getFirst(expr))==1){
	return evalEach(expr,env); // eval literals.
      }else if (getFirst(expr) && getFirst(expr)->type ==cellType && getFirst(getTail(expr)) && getFirst(getTail(expr))->type==closeType){
	return eval(getFirst(expr),env);
      }else if (getFirst(expr) && getFirst(expr)->type == symbolType){


	operator = getFirst(expr);
	Value *returnValue = envLookup(operator->symbolValue, env);
	// we do need this one.
	if (returnValue){
	  return returnValue;
	}else{  // show error message below
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
	    
	    printf("Unknown identifier %s.\n",operator->symbolValue);
	  }
	  
	  return NULL;
	}
      }
      
    case closeType:
      
      return NULL;
      break;
    default:
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
    
    return getFirst(args);
  }
}


// We have not tested this function yet for part a.
Value* apply(Value* function, Value* actualArgs, Environment* env){
  
  if (!function){
    return actualArgs;
  }else if (function->type == primitiveType){
    Value *returnVal = function->primitiveValue(actualArgs, env);
    return returnVal;
  }else if (function->type == closureType){
    List *formalArgs = function->closureValue->args;
  
    
    
    Environment *frame = createFrame(function->closureValue->parent);
    /* Bind formalArgs to actualArgs in frame here. */
    Value *curArg = formalArgs->head;
    Value *curValue = actualArgs;
    while (curArg && curValue){
      assert(getFirst(curArg)->type ==symbolType);

      insertItem(frame->bindings->tableValue, getFirst(curArg)->symbolValue, getFirst(curValue));

      curArg = getTail(curArg);
      curValue = getTail(curValue);
      
    }
    if (curArg || curValue){
      printf("Wrong number of parameters for the procedure.\n");
      destroyEnvironment(frame);
      return NULL;
    }
   
    Value *returnValue = eval(function->closureValue->body, frame);
  
    insertItem(env->bindings->tableValue, "#returnValue",returnValue);
   
    destroyEnvironment(frame);
    returnValue = lookup(env->bindings->tableValue, "#returnValue");
   
    
    return returnValue;
  }else{
    printf("Unknown identifier!");
    return NULL;
  }
}

// look up the identifier.
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

// group types into five main groups.
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
      case primitiveType:
	return 2;
	break;
      case symbolType:
	return 3;
	break;
      case cellType:
	return 4;
	break;
      case closeType:
	return 5;
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
	  strcmp(value->symbolValue,"cons")==0)
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
    Value *value;
    if (getFirst(getTail(args)) && getFirst(getTail(args))->type == symbolType){
      value = eval(envLookup(getFirst(getTail(args))->symbolValue, env), env);
  
      if (value){
	if (value->type == closureType){
	  if (!value->closureValue->identifier){
	    nameClosure(value->closureValue, (getFirst(args))->symbolValue);
	  }
	}
	insertItem(env->bindings->tableValue, (getFirst(args))->symbolValue, value);
      }else{
	printf("syntax error: unknown identifier\n");
	return NULL;
      }
    }else{
      assert(getFirst(args)!=NULL);
      assert(getFirst(args)->type==symbolType);
      assert(env->bindings->tableValue!=NULL);
      value = eval(getFirst(getTail(args)), env);
      if (value){
	if (value->type == closureType){
	  if (!value->closureValue->identifier){
	    nameClosure(value->closureValue, (getFirst(args))->symbolValue);
	  }
	}
	insertItem(env->bindings->tableValue, getFirst(args)->symbolValue, value);
      }else{
	printf("syntax error: Cannot nest another definition inside define.\n");
	return NULL;
      }
    }
    return NULL;
  }  
}

// eval each part of the arguments.
Value* evalEach(Value* args, Environment* env){
  Value *temp;
  Value *head, *openParen;
  List *returnValue = initializeList();
 

  while (args && typeCheck(getFirst(args))!=5){ 
    assert(args->type==cellType);    
    if (getFirst(args) && (getFirst(args))->type==cellType){
      openParen = getFirst(getFirst(args));
      if (openParen && openParen->type == openType){
	temp =  deepCopy(eval(getFirst(args), env));	
	if (!temp){
	  free(returnValue);
	  printf("Error in eval each open.\n");
	  return NULL;
	}
	push(returnValue, temp);
	args = getTail(args);
      }else if (getFirst(getFirst(args)) && getFirst((getFirst(args)))->type==nullType){
	temp =  deepCopy(getFirst(getFirst(args)));
	push(returnValue, temp);
	args = getTail(args);
      }else{
	printf("Error! Cannot evaluate each one.\n");
	free(returnValue);
	return NULL;
      }
    }else if (getFirst(args) && (getFirst(args))->type==nullType){
	temp =  deepCopy(getFirst(args));
	push(returnValue, temp);
	args = getTail(args);
    }else{
      temp = deepCopy(eval(getFirst(args), env));
      if (!temp){
	break;
      }
      
      push(returnValue, temp);
      args =getTail(args);
    }
  }
 
  assert(env!=NULL);
 
  reverse(returnValue);
  insertItem(env->bindings->tableValue, "#returnValue", returnValue->head);
  destroy(returnValue);
  head = lookup(env->bindings->tableValue, "#returnValue"); 

  return head;
}

// eval letrec
Value *evalLet(Value *args, Environment *env){
  Value *toCheck;
  
  int count = listLength(args);
  if (count < 2){
    printf("let: bad syntax in: (let ");
    printValue(args);
    printf("\n");
    return NULL;
  }
  if (getFirst(args)->type== nullType){
    Value * toReturn;
    while (getTail(getTail(getTail(args)))){
      toReturn = eval(getFirst(getTail(args)),env);
      if(toReturn == NULL){
	if (strcmp(getFirst(getTail(getFirst(getTail(args))))->symbolValue, "set!") != 0){
	  printf("Syntax error in letrec\n");
	  return NULL;	
	}
      }						
      args = getTail(args);
    }
    
    Value* toEval = eval(getTail(args), env);
    if (toEval){
      return toEval;
    }else{
      if (strcmp(getFirst(getTail(getFirst(getTail(args))))->symbolValue, "set!") == 0){
	return NULL;
       }else{
	printf("Bad syntax in letrec.\n");
	return NULL;
      }
    }
  }
  if (getFirst(args)-> type != cellType){
    printf("syntax error in let: not a sequence of indentifier\n");
    return NULL;
  }else{
    if (!getFirst(getTail(getFirst(args)))){
      printf("syntax error in let: missing components\n");
      return NULL;
    } else if (getFirst(getTail(getFirst(args)))-> type != cellType){
      printf("syntax error in let: not an indentifier for bindings\n");
      return NULL;
    } 
    toCheck = getTail(getFirst(args));
    
    if (getFirst(getTail(getFirst(toCheck))) -> type !=symbolType){
      printf("bad syntax in let: not an indentifier\n");
      return NULL;
    }else{
      Environment* newEnv = createFrame(env);
      Value* listofBinds = toCheck;
     
      listofBinds = toCheck;
      while (listofBinds){
	
	if (getFirst(listofBinds) && typeCheck(getFirst(listofBinds))==5){
	  break;
	}
	
	Value* toBind = eval(getTail(getTail(getFirst(listofBinds))), env);
	
	if (toBind){
	  if (toBind->type == cellType){
	    if (getTail(toBind)){
	      printf("syntax error in let: too many values for single identifier.\n");
	      destroyEnvironment(newEnv);
	      return NULL;
	    }else{
	      toBind = getFirst(toBind);
	    }
	  }else	if (toBind->type == closureType){
	    if (!toBind->closureValue->identifier){
	      nameClosure(toBind->closureValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue);
	    }
	  }
	  insertItem(newEnv->bindings->tableValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue, toBind);
	  
	}else{ 
	  printf("syntax error in let\n");
	  destroyEnvironment(newEnv);
	  return NULL;
	}
		
	listofBinds = getTail(listofBinds);

      }
      Value* listofExpressions = getTail(args);
      
    
      
      if (!listofExpressions){
	printf(" let: bad syntax.\n ");
	return NULL;
      }
      while(listofExpressions && typeCheck(getFirst(listofExpressions))!=5){
	Value* toReturn = eval(getFirst(listofExpressions), newEnv);
	if(toReturn == NULL){
	  if(!getFirst(getTail(listofExpressions)) || getFirst(getTail(listofExpressions))->type==closeType){
	    if (strcmp(getFirst(getTail(getFirst(listofExpressions)))->symbolValue, "set!") == 0){
	      destroyEnvironment(newEnv);
	      return NULL;
	    }
	  }
	}


	if(typeCheck(toReturn) < 3 && typeCheck(toReturn) > 0 ){
	  if( getFirst(getTail(listofExpressions)) && getFirst(getTail(listofExpressions))->type!=closeType){
	    
	    listofExpressions = getTail(listofExpressions);
	    freeValue(toReturn);
	  }else{
	    if (toReturn && toReturn->type==closureType){
	      toReturn->closureValue->parent = insertEnv(newEnv, env);
	      return toReturn;   // if the last item is lambda, we need to return it and leave let environment there.
	    }else if (toReturn && getFirst(listofExpressions)->type == symbolType && lookup(newEnv->bindings->tableValue, getFirst(listofExpressions)->symbolValue)){
	      toReturn = deepCopy(toReturn); // if the symbol is in the new environment, need to copy it before destroying the new environment.
	    }
	    destroyEnvironment(newEnv); // clean the environment since no pointer points to it.
	    return toReturn;
	  }
	}else{
	  listofExpressions = getTail(listofExpressions);
	}	
      }
      printf("let: bad syntax\n");
      destroyEnvironment(newEnv);
      return NULL;
    }
  }
  return NULL;
}




Value* evalLetStar(Value* args, Environment* env){
  Value *toCheck;
  Environment* newEnv, *firstEnv, *parent, *temp;
  int count = listLength(args);
  if (count < 2){
    printf("let*: bad syntax in: (let* ");
    printValue(args);
    printf("\n");
    return NULL;
  }
  if (getFirst(args)->type== nullType){
    Value * toReturn;
    while (getTail(getTail(getTail(args)))){
      toReturn = eval(getFirst(getTail(args)),env);
      if(toReturn == NULL){
	if (strcmp(getFirst(getTail(getFirst(getTail(args))))->symbolValue, "set!") != 0){
	  printf("Syntax error in let*\n");
	  return NULL;	
	}
      }						
      args = getTail(args);
    }
    
    Value* toEval = eval(getTail(args), env);
    if (toEval){
      return toEval;
    }else{
      if (strcmp(getFirst(getTail(getFirst(getTail(args))))->symbolValue, "set!") == 0){
	return NULL;
       }else{
	printf("Bad syntax in let*.\n");
	return NULL;
      }
    }
  }
  
  if (getFirst(args)-> type != cellType){
    printf("syntax error in let*: not a sequence of indentifier\n");
    return NULL;
  }else{
    if (!getFirst(getTail(getFirst(args)))){
      printf("syntax error in let*: missing components\n");
      return NULL;
    } else if (getFirst(getTail(getFirst(args)))-> type != cellType){
      printf("syntax error in let*: not an indentifier for bindings\n");
      return NULL;
    } 
    toCheck = getTail(getFirst(args));
   
    if (getFirst(getTail(getFirst(toCheck))) -> type !=symbolType){
      printf("bad syntax in let*: not an indentifier\n");
      return NULL;
    }else{
    
      Value* listofBinds = toCheck;
      firstEnv = createFrame(env);
      parent = env;
      newEnv = firstEnv;
      
      while (getFirst(listofBinds) && typeCheck(getFirst(listofBinds))!=5){
	
	Value* toBind = eval(getTail(getTail(getFirst(listofBinds))), parent);
	
	if (toBind){
	  
	  if (toBind->type == cellType){
	    if (getTail(toBind)){
	      printf("syntax error in let*: too many values for single identifier.\n");
	      destroyFrame(firstEnv);
	      return NULL;
	    }else{
	      toBind = getFirst(toBind);
	    }
	  }else if (toBind->type == closureType){
	    if (!toBind->closureValue->identifier){
	      nameClosure(toBind->closureValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue);
	      }
	  }
	  insertItem(newEnv->bindings->tableValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue, toBind);
	  
	}else{
	  printf("syntax error in let*\n");
	  destroyFrame(firstEnv);
	  return NULL;
	}
	
	listofBinds = getTail(listofBinds); // get a list of variables that need to be binded.
	if (getFirst(listofBinds) && typeCheck(getFirst(listofBinds))==5){
	  break;
	}
	  parent = newEnv;
	  newEnv = createFrame(parent);	  
      } // end for while loop.
    
      Value* listofExpressions = getTail(args);
      
      if (!listofExpressions){
	printf("syntax error in let*: bad syntax.\n ");  // if no return value is found, print out syntax error.
	destroyFrame(firstEnv); 
	return NULL;
      }
      
	
      while(listofExpressions && typeCheck(getFirst(listofExpressions))!=5){
	Value* toReturn = eval(getFirst(listofExpressions), newEnv);
	if(toReturn == NULL){

	  if(!getFirst(getTail(listofExpressions)) || getFirst(getTail(listofExpressions))->type==closeType){
	    if (strcmp(getFirst(getTail(getFirst(listofExpressions)))->symbolValue, "set!") == 0){
	      destroyFrame(firstEnv);
	      return NULL;
	    }
	  }
	}

	if(typeCheck(toReturn) < 3 && typeCheck(toReturn) > 0 ){
	  if( getFirst(getTail(listofExpressions)) && getFirst(getTail(listofExpressions))->type!=closeType){
	    
	    listofExpressions = getTail(listofExpressions);
	    freeValue(toReturn);
	  }else{
	    if (toReturn && toReturn->type==closureType){
	      temp = insertEnv(firstEnv, env);
	      destroyTable(temp->bindings->tableValue);
	      temp->bindings->tableValue = firstEnv->bindings->tableValue;
	      free(firstEnv->bindings);
	      free(firstEnv);
	      toReturn->closureValue->parent = newEnv;
	      return toReturn;   // if the last item is lambda, we need to return it and leave let environment there.
	    }else if (toReturn && getFirst(listofExpressions)->type == symbolType && letEnvLookup(newEnv, firstEnv, getFirst(listofExpressions)->symbolValue)){
	      toReturn = deepCopy(toReturn); // if the symbol is in the new environment, need to copy it before destroying the new environment.
	    }
	    destroyFrame(firstEnv); // clean the environment since no pointer points to it.
	    return toReturn;
	  }
	}else{
	  listofExpressions = getTail(listofExpressions);
	}	
      }
      printf("let*: bad syntax\n");
      destroyEnvironment(newEnv);
      return NULL;
    }
  }
  return NULL;
}


Value* evalLetrec(Value* args, Environment* env){
  Value *toCheck,  *toBind;
  int count = listLength(args);
  
  if (count < 2){
    return NULL;
  }
 
  if (getFirst(args)->type== nullType){
    Value * toReturn;
    while (getTail(getTail(getTail(args)))){
      toReturn = eval(getFirst(getTail(args)),env);
      if(toReturn == NULL){
	if (strcmp(getFirst(getTail(getFirst(getTail(args))))->symbolValue, "set!") != 0){
	  printf("Syntax error in letrec\n");
	  return NULL;	
	}
      }						
      args = getTail(args);
    }
    
    Value* toEval = eval(getTail(args), env);
    if (toEval){
      return toEval;
    }else{
      if (strcmp(getFirst(getTail(getFirst(getTail(args))))->symbolValue, "set!") == 0){
	return NULL;
       }else{
	printf("Bad syntax in letrec.\n");
	return NULL;
      }
    }
  }
       
     
   
  if (getFirst(args)-> type != cellType){
    printf("syntax error in letrec: not a sequence of indentifier\n");
    return NULL;
  }else{
    if (!getFirst(getTail(getFirst(args)))){
      printf("syntax error in letrec: missing components\n");
      return NULL;
    } else if (getFirst(getTail(getFirst(args)))-> type != cellType){
      printf("syntax error in letrec: not an indentifier for bindings\n");
      return NULL;
    }
    toCheck = getTail(getFirst(args));
 
   
    
    if (getFirst(getTail(getFirst(toCheck))) -> type !=symbolType){
      printf("bad syntax in letrec: not an indentifier\n");
      return NULL;
    }else{
      Environment* newEnv = createFrame(env);
      Value* listofBinds = toCheck; // remember the start of variable list.
     
      // first round of forming bindings. Ignore symbols first.
      
   
      while (listofBinds){
        toBind = getFirst(getTail(getTail(getFirst(listofBinds))));


        if (typeCheck(toBind) > 0 && typeCheck(toBind)!=3){
          toBind = eval(getFirst(getTail(getTail(getFirst(listofBinds)))), newEnv);
	 
        }else{
          toBind = NULL;
        }
        
        if (toBind){ 
          if (toBind->type == cellType){
            if (getTail(toBind) && typeCheck(getTail(toBind))!=5 ){
              printf("syntax error in letrec: too many values for single identifier.\n");
	      destroyEnvironment(newEnv); 
	      return NULL;
            }else{
              toBind = getFirst(toBind);
	    
            }
          }else	if (toBind->type == closureType){
	    if (!toBind->closureValue->identifier){
	      nameClosure(toBind->closureValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue);
	    }
	    //printf("printing the clousre: ");
	    //  print(toBind->closureValue->args);
	    //printValue(toBind->closureValue->body);
	    //printf("\n");
	  }
	  //printf("in let2: %s => ",getFirst(getTail(getFirst(listofBinds)))->symbolValue);
	  //printValue(toBind);
	  //printf("\n");
          insertItem(newEnv->bindings->tableValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue, toBind);
        }
	listofBinds = getTail(listofBinds);
	//printf("in let3: The following is  ");
	//printValue(listofBinds);
	//printf("\n");	
      }
 
      listofBinds = toCheck;
      
      toBind = getFirst(toBind);
      
      // form bindings by assigning values to the symbols missed in the first round. 
      while(listofBinds){

	if (getFirst(listofBinds) && typeCheck(getFirst(listofBinds))==5){
	 
	  break;
	}
	assert(getFirst(getTail(getFirst(listofBinds)))!=NULL);
        assert(getFirst(getTail(getFirst(listofBinds)))->type == symbolType);
	

        if (envLookup(getFirst(getTail(getFirst(listofBinds)))->symbolValue, newEnv) == NULL){
	  toBind = getFirst(getTail(getTail(getFirst(listofBinds))));
	  
          if (toBind && toBind->type == symbolType){
            if(envLookup(toBind->symbolValue, newEnv) != NULL){
	      Value *curValue = eval(toBind,newEnv);
              insertItem(newEnv->bindings->tableValue,getFirst(getTail(getFirst(listofBinds)))->symbolValue, curValue);
	      
	      //printf("in let4: %s=> ",getFirst(getTail(getFirst(listofBinds)))->symbolValue);
	      //printValue(curValue);
	      //printf("\n");
            }else{
              printf("syntax error: unknown identifier\n");
	      destroyEnvironment(newEnv); 
	      return NULL;
            }
          }
	}
	listofBinds = getTail(listofBinds);
	//printf("in let5: The following is  ");
	//printValue(listofBinds);
	//printf("\n");	

      }
     

      Value* listofExpressions = getTail(args);
 
     
      if (!listofExpressions){
	printf("syntax error in letrec: bad syntax.\n ");  // if no return value is found, print out syntax error.
	destroyEnvironment(newEnv); 
	return NULL;
      }
     
      while(listofExpressions && typeCheck(getFirst(listofExpressions))!=5){
	Value* toReturn = eval(getFirst(listofExpressions), newEnv);
	
	if(toReturn == NULL){

	  if(!getFirst(getTail(listofExpressions)) || getFirst(getTail(listofExpressions))->type==closeType){
	    if (strcmp(getFirst(getTail(getFirst(listofExpressions)))->symbolValue, "set!") == 0){
	      destroyEnvironment(newEnv);
	      return NULL;
	    }
	  }
	}

	
	if(typeCheck(toReturn) < 3 && typeCheck(toReturn) > 0 ){
	  if( getFirst(getTail(listofExpressions)) && getFirst(getTail(listofExpressions))->type!=closeType){
	    
	    listofExpressions = getTail(listofExpressions);
	     freeValue(toReturn);
	  }else{
	    if (toReturn && toReturn->type==closureType){
	      //printf("returning here\n");
	      toReturn->closureValue->parent = insertEnv(newEnv, env);
	      return toReturn;
	    }else if (toReturn && getFirst(listofExpressions)->type == symbolType && lookup(newEnv->bindings->tableValue, getFirst(listofExpressions)->symbolValue)){
	      toReturn = deepCopy(toReturn); // if the symbol is in the new environment, need to copy it before destroying the new environment.
	      //printf("copying the things: ");
	      //printValue(toReturn);
	      //printf("type of toReturn: %d",toReturn->type);
	      //printf("\n");
	    }
	    
	      // if the last item is lambda, we need to return it and leave let environment there.
	    
	    destroyEnvironment(newEnv); // clean the environment since no pointer points to it.
	    return toReturn;
	  }
	}else{
	  listofExpressions = getTail(listofExpressions);
	}	
      }
      printf("letrec: bad syntax\n");
      destroyEnvironment(newEnv);
      return NULL;
    }

  }
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

  Value *evalTest = eval(getFirst(args), env);
  if (!evalTest){
    printf("Unknown identifier in ");
    printValue(getFirst(args));
    printf("\n");
    return NULL;
  }
 
 if (evalTest && evalTest->type == booleanType && !(evalTest->boolValue)) {
    // if evalTest is false, then return eval(alternate)
    // if no alternate, just returns NULL
   if (count == 3) {
    
     return eval(getFirst(getTail(getTail(args))), env);
   }
   else 
     return  NULL; // DRracket doesn't return a '(), it returns nothing (NULL)
 }
 else {
    
   // else return eval(consequence)
   return eval(getFirst(getTail(args)), env); // return eval(alternate)
 }
}

// Return a closure which contains a parse tree, a linked list of parameters.

Value* evalLambda(Value* args, Environment* env){
  if (!env || !args){
    printf("Syntax error for lambda. Missing components here\n");
    return NULL;
  }
  assert(args->type = cellType);
  Value *toCheck = getFirst(args);
 
  if (getFirst(toCheck) && getFirst(toCheck)->type == openType){
    Value *returnValue = (Value *)malloc(sizeof(Value));
    returnValue->type = closureType;
    Closure *closure = initializeClosure(env);
    toCheck = getTail(toCheck);
    
    while (toCheck && getFirst(toCheck) && getFirst(toCheck)->type!=closeType){
      if (variableCheck(getFirst(toCheck))!=1){
	printf("Syntax error for lambda. Invalid parameters.\n");
	destroyClosure(closure);
	return NULL;
      }
      push(closure->args, deepCopy(getFirst(toCheck)));
      toCheck = getTail(toCheck);
    } 
    // we should be able to handle anonymous functions
    
    if (!toCheck || !getFirst(toCheck) || getFirst(toCheck)->type!=closeType){
      printf("Syntax error for lambda. Missing parameters.\n");
      destroyClosure(closure);
      return NULL;
      }
    
    reverse(closure->args);
    toCheck = getTail(args);
    if (!toCheck){
      printf("lambda: bad syntax in: (lambda ");
      printValue(args);
      printf("\n");
      destroyClosure(closure);
      return NULL;
    }
   
    closure->body = deepCopy(getFirst(toCheck));
    returnValue->closureValue = closure;
 
    return returnValue; 
  }else if (getFirst(toCheck) && getFirst(toCheck)->type==nullType){
    Value *returnValue = (Value *)malloc(sizeof(Value));
    returnValue->type = closureType;
    Closure *closure = initializeClosure(env);
    toCheck = getTail(args); // need to worry about this line.

    closure->body = deepCopy(getFirst(toCheck));
    returnValue->closureValue = closure;
  
    return returnValue; 
  }else{
    
    printf("Syntax error for lambda! Missing parameters.\n");
    return NULL;
  }
}

// Make basic c functions to primitive scheme functions.
Value *makePrimitiveValue(Value* (*f)(Value *, Environment *)){
  Value *resultValue = (Value *)malloc(sizeof(Value));
  resultValue->type = primitiveType;
  resultValue->primitiveValue = f;
  return resultValue;
}

// creates a top-level frame and binds the "built-in" functions.
Environment *createTopFrame(){
  Environment *frame = createFrameWithSize(NULL,256);
  bind("+", makePrimitiveValue(add), frame);
  bind("-", makePrimitiveValue(subtract), frame);
  bind("*", makePrimitiveValue(multiply), frame);
  bind("/", makePrimitiveValue(divide), frame);
  // bind("exp", makePrimitiveValue(exponentiate), frame);
  bind("=",makePrimitiveValue(arithmeticEqual),frame);
  bind("equal?",makePrimitiveValue(equality),frame);
  bind("<=",makePrimitiveValue(smallerOrEqualTo),frame);
  bind("<", makePrimitiveValue(smallerThan), frame);
  bind(">=",makePrimitiveValue(greaterOrEqualTo),frame);
  bind(">", makePrimitiveValue(greaterThan), frame);
  bind("car", makePrimitiveValue(car), frame);
  bind("cdr", makePrimitiveValue(cdr), frame);	
  bind("cons", makePrimitiveValue(cons), frame);
  bind("null?", makePrimitiveValue(checkNull), frame);
  Value *null = (Value *)malloc(sizeof(Value));
  null->type = nullType;
  bind("null", null, frame);
  return frame;
}

// bind primitive functions to the top-level environment.
void bind(char identifier[], Value *function, Environment *env){
  if (!env || !function){
    return;
  }
  
  insertItem(env->bindings->tableValue, identifier, function);
  free(function);
}
// free top frame.
/*void freeTopFrame(Environment *env){
  if (env){
    while (env->parent){
      env = env->parent;  // find the top-level environment;
    }
    assert(env->bindings!=NULL);
    assert(env->bindings->type==tableType);
    HashTable *table = env->bindings->tableValue;
    int i;
    for (i = 0; i<(table->capacity); i++){
      if ((table->entries[i]).car != NULL){
	assert(table->entries);
	if ((table->entries[i]).cdr  && ((table->entries[i]).cdr)->type == primitiveType){
	  free((table->entries[i]).cdr);
	  (table->entries[i]).cdr = NULL;
	}
      }
    }
    destroyTable(table);
    free(env->bindings);
    free(env);
  }
}
*/
// creates an environment.
Environment *createFrameWithSize(Environment *parent, int size){
  Environment *frame = (Environment *)malloc(sizeof(Environment));
  frame->parent = parent;
  HashTable *table = initializeTable(size);
  Value *value = (Value *)malloc(sizeof(Value));
  value->type = tableType;
  value->tableValue = table;
  frame->bindings = value;
  Value *id = (Value *)malloc(sizeof(Value));
  id->type = integerType;
  id->intValue = 0;
  insertItem(frame->bindings->tableValue,"#envID",id);
  free(id);
  return frame;
}

Environment *createFrame(Environment *parent){
  return createFrameWithSize(parent, 32);
}



// Arithmetic addition function. Integer and float addition are treated differently.
Value *add(Value *args, Environment *env){
  int intSum = 0;
  double dblSum = 0;
  int isFloat = 0;
  Value  *head;
  head = args;
  if (head){
    while (args && args->type == cellType){
      if (getFirst(args)== NULL){
	if (getTail(args)==NULL){
	  break;
	}else{
	  printf("Invalid input for <#procedure:+>\n");
	  return NULL;
	}
      }else if (getFirst(args)->type == integerType){
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
	printf("+: expects type <number> as arguments\n");
	return NULL;
      }
      args = getTail(args);
    }
    
   
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



// Arithmetic subtraction.
Value *subtract(Value *args, Environment *env){
  int intDiff = 0;
  double dblDiff = 0;
  int isFloat = 0;
  Value  *head;
  if (args && args->type ==cellType && args->cons->car){
    if (args->cons->car->type == integerType){
      intDiff = args->cons->car->intValue; 
    }else if (args->cons->car->type == floatType){
      dblDiff = args->cons->car->dblValue; 
      isFloat = 1;
    }else{
      printf("-: expects type <number> as arguments\n");
      return NULL;
    }
    args = getTail(args);
    if (!args){
      if (isFloat){
	dblDiff = 0 - dblDiff;
      }else{
	intDiff = 0 - intDiff;
      }
    }
  }else{
      printf("Invalid input for <#procedure:->\n");
      return NULL;
  }
  head = args;
  if (head){
    while (args && args->type == cellType){
      if (getFirst(args)== NULL){
	if (getTail(args)==NULL){
	  break;
	}else{
	  printf("Invalid input for <#procedure:->\n");
	  return NULL;
	}
      }else if (getFirst(args)->type == integerType){
	if (!isFloat){
	  intDiff -= getFirst(args)->intValue;
	}else{
	  dblDiff -= getFirst(args)->dblValue;
	}
      }else if (getFirst(args)->type == floatType){
	if (!isFloat){
	  isFloat = 1;
	  dblDiff = intDiff;
	}
	dblDiff -= getFirst(args)->dblValue;	
      }else if (getFirst(args)->type == closeType){
	break;
      }else{
	printf("-: expects type <number> as arguments\n");
	return NULL;
      }
      args = getTail(args);
    }
    
  }
  Value *value = (Value*) malloc(sizeof(Value));

  if (isFloat){
    value->type = floatType;
      value->dblValue = dblDiff; 
  }else{
    
    value->type = integerType;
    value->intValue = intDiff; 
  }
 
  return value;
}

// Arithmetic mutiplication.
Value *multiply(Value *args, Environment *env){
  int intProd = 1;
  double dblProd = 1.0;
  int isFloat = 0;
  Value  *head;
  head = args;
  if (head){
    while (args && args->type == cellType){
      if (getFirst(args)== NULL){
	if (getTail(args)==NULL){
	  break;
	}else{
	  printf("Invalid input for <#procedure:*>\n");
	  return NULL;
	}
      }else if (getFirst(args)->type == integerType){
	if (!isFloat){
	  intProd *= getFirst(args)->intValue;
	}else{
	
	  dblProd = dblProd * (getFirst(args)->intValue);
	
	}
      }else if (getFirst(args)->type == floatType){
	if (!isFloat){
	  isFloat = 1;
	  dblProd = intProd;
	}

	dblProd *= getFirst(args)->dblValue;	

      }else if (getFirst(args)->type == closeType){
	break;
      }else{
	printf("*: expects type <number> as arguments\n");
	return NULL;
      }
      args = getTail(args);
    }
    
   
 
  }
  Value *value = (Value*) malloc(sizeof(Value));

  if (isFloat){
    value->type = floatType;
    value->dblValue = dblProd; 
  }else{
    
    value->type = integerType;
    value->intValue = intProd; 
  }

  return value;
}

/* 
   Arithmetic division. We do not allow zero division 
   even though it is allowed in DrRacket that (/ 5 0.0)=>+inf.0
*/
Value *divide(Value *args, Environment *env){
  int intQuot = 1;
  double dblQuot = 1.0;
  int isFloat = 0;
  Value  *head;

 
  if (args && args->type == cellType && args->cons->car){
    if (args->cons->car->type == integerType){
      intQuot = args->cons->car->intValue; 
    }else if (args->cons->car->type == floatType){
      dblQuot = args->cons->car->dblValue; 
      isFloat = 1;
    }else{
      printf("/: expects type <number> as arguments\n");
      return NULL;
    }
    args = getTail(args);
    if (!args){
      if (isFloat){
	if (dblQuot == 0){
	  printf("/: division by zero\n");
	  return NULL;
	}
	dblQuot = 1.0 / dblQuot;
      }else{
	if (intQuot == 0){
	  printf("/: division by zero\n");
	  return NULL;
	}
	isFloat = 1;
	dblQuot = 1.0 /intQuot;
      }
    }
  }else{
    printf("Invalid input for <#procedure:->\n");
    return NULL;
  }
  head = args;
  if (head){
    while (args && args->type == cellType){
      if (getFirst(args)== NULL){
	if (getTail(args)==NULL){
	  break;
	}else{
	  printf("Invalid input for <#procedure:*>\n");
	  return NULL;
	}
      }else if (getFirst(args)->type == integerType){
	if (getFirst(args)->intValue == 0){
	  printf("/: division by zero\n");
	  return NULL;
	}
	if (!isFloat){
	  if (intQuot % getFirst(args)->intValue ==0){
	    intQuot /= getFirst(args)->intValue;
	  }else{
	    isFloat = 1;
	    dblQuot = (double)intQuot/ getFirst(args)->intValue;
	  }
	}else{
	  
	  dblQuot /= (getFirst(args)->intValue);
	 
	}
      }else if (getFirst(args)->type == floatType){
	if (getFirst(args)->dblValue == 0){
	  printf("/: division by zero\n");
	  return NULL;
	}
	if (!isFloat){
	  isFloat = 1;
	  dblQuot = intQuot;
	}
	
	dblQuot /= getFirst(args)->dblValue;	
	
      }else if (getFirst(args)->type == closeType){
	break;
      }else{
	printf("*: expects type <number> as arguments\n");
	return NULL;
      }
      args = getTail(args);
    }
    
  
  }
  Value *value = (Value*) malloc(sizeof(Value));
  
  if (isFloat){
    value->type = floatType;
    value->dblValue = dblQuot; 
  }else{
    
    value->type = integerType;
    value->intValue = intQuot; 
  }
 
  return value;
}

// Load function.
Value *loadFunction(Value *args, Environment *env){
  
  int count = listLength(args);

  if (count > 1) {
    printf("load: load one file at a time\n");
 
   
    return NULL;
  } else if (count < 1) {
    printf("load: you must enter the name of a file to load\n");
  
    return NULL;
  }
  
  if (getFirst(args)->type == stringType) {
    FILE *fp;
    char *fileName = getFirst(args)->stringValue;
    char *realName = NULL;
    int i;

    // remove quotes around fileName
    fileName++;
    fileName[strlen(fileName) -1] = '\0';

    fp = fopen(fileName, "r");
    if (!fp) {
      fprintf(stderr, "load: cannot open %s\n", fileName);
      return NULL;
    }

    // somehow do it for the current environment
    loadFromFile(fp, env);
 
    return NULL;
  }  else {

    return NULL;
  }
  return NULL;
}

int loadFromFile(FILE *file, Environment *env){
  List *tokens, *parseTree, *leftoverTokens = NULL;
  leftoverTokens = initializeList();
  int depth = 0; 
  char *expression = (char *)malloc(256 * sizeof(char));
  Value* temp;
  int i;
  // printf(">  ");
  while (fgets(expression, 256, file)) {
    
    tokens = append(leftoverTokens, tokenize(expression)); 
     
     if (!tokens) {
       leftoverTokens->head = NULL;
       continue;
     }
     parseTree = parse(tokens,&depth);   

     if (depth < 0) {
       printf("Syntax error. Too many close parentheses.\n");   // Too many close parentheses. 
       cleanup(tokens->head);
       free(leftoverTokens);
       free(tokens);
       
       return SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS;
     } else if (depth > 0) {
       // There are more open parens than close parens, so these tokens are saved as leftovers. We partially generate a parse tree now.
       leftoverTokens->head = tokens->head;
       continue;
     }else{
       
       if (parseTree && parseTree->head){
	 
	 temp = eval(parseTree->head,env);
	 if (temp){
	   printValue(temp);
	   printf("\n");
	 }
	 
	 destroy(parseTree);
       }
       leftoverTokens->head = tokens->head;
       while (depth == 0){
	 
	 parseTree = parse(tokens,&depth);   
	 
	 if (depth < 0) {
	   printf("Syntax error. Too many close parentheses.\n");   // Too many close parentheses. 
	   cleanup(tokens->head);
	   free(leftoverTokens);
	   free(tokens);
	  
	   break;
	   //return SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS;
	 } else if (depth > 0) {
	   // There are more open parens than close parens, so these tokens are saved as leftovers. We partially generate a parse tree now.
	   leftoverTokens->head = tokens->head;

	   break;
	 } else {
	   
	   if (parseTree && parseTree->head){
	     
	     temp = eval(parseTree->head,env);
	     if (temp){
	
	       printValue(temp);
	       printf("\n"); 
	     }

	     destroy(parseTree);
	   }
	   leftoverTokens->head = tokens->head;
	   
	   if (!leftoverTokens->head){
	     break;
	   }
	   

	 }
       }
       //  printf(">  ");
     }
  }
  if (leftoverTokens->head) {
    printf("syntax error. Too few close parentheses\n");   // Too few close parens at end of input. 
    destroy(leftoverTokens);   
    free(tokens);
    return SYNTAX_ERROR_UNTERMINATED_INPUT;
  }
  // clean up memory 
  destroy(leftoverTokens);
  free(tokens);
  free(expression); 
  

}

int interface(Environment *env){
  List *tokens, *parseTree, *leftoverTokens = NULL;
  leftoverTokens = initializeList();
  int depth = 0; 
  int indentation = 0;
  char *expression = (char *)malloc(256 * sizeof(char));
  Value* temp;
  int i;
  printf(">  ");
  while (fgets(expression, 256, stdin)) {
    
    tokens = append(leftoverTokens, tokenize(expression)); 
     
     if (!tokens) {
       leftoverTokens->head = NULL;
       continue;
     }
     parseTree = parse(tokens,&depth);   

     if (depth < 0) {
       printf("Syntax error. Too many close parentheses.\n");   // Too many close parentheses. 
       cleanup(tokens->head);
       leftoverTokens->head =NULL;
       free(tokens);
       indentation = 0;
       printf("=> ");
       continue;
       //return SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS;
     } else if (depth > 0) {
       // There are more open parens than close parens, so these tokens are saved as leftovers. We partially generate a parse tree now.
       leftoverTokens->head = tokens->head;
       printf("..    ");
     
       for (i=0;i<indentation;i++){
	 printf(" ");
       }
       indentation++;
     }else{
       
       if (parseTree && parseTree->head){
	 
	 temp = eval(parseTree->head,env);
	 if (temp){
	   printf("=> ");
	   printValue(temp);
	   printf("\n"); 
	 }
	 indentation = 0;
	 destroy(parseTree);
       }
       leftoverTokens->head = tokens->head;
       while (depth == 0){
	 
	 parseTree = parse(tokens,&depth);   
	 
	 if (depth < 0) {
	   printf("Syntax error. Too many close parentheses.\n");   // Too many close parentheses. 
	   cleanup(tokens->head);
	   leftoverTokens->head =NULL;
	   free(tokens);
	   indentation = 0;
	   break;
	   //return SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS;
	 } else if (depth > 0) {
	   // There are more open parens than close parens, so these tokens are saved as leftovers. We partially generate a parse tree now.
	   leftoverTokens->head = tokens->head;
	   printf("..      ");

	   for (i=0;i<indentation;i++){
	     printf(" ");
	   }
	   indentation++;
	   break;
	 } else {
	   
	   if (parseTree && parseTree->head){
	     
	     temp = eval(parseTree->head,env);

	     if (temp){
	       printf("=> ");
	       printValue(temp);
	       printf("\n"); 
	     }
	     indentation = 0;
	     destroy(parseTree);
	   }
	   leftoverTokens->head = tokens->head;
	   
	   if (!leftoverTokens->head){
	     break;
	   }
	   
	
	 }
       }
       printf(">  ");
     }
  }
  if (leftoverTokens->head) {
    printf("syntax error. Too few close parentheses\n");   // Too few close parens at end of input. 
    destroy(leftoverTokens);   
    free(tokens);
    return SYNTAX_ERROR_UNTERMINATED_INPUT;
  }
  // clean up memory
  destroy(leftoverTokens);
  free(tokens);
  free(expression); 
  

}

  


// Special form And
Value *evalAnd(Value *args, Environment *env){
  assert(env!=NULL);
  assert(env->bindings!=NULL);
  assert(args!=NULL);
  assert(args->type == cellType);
  
  if (!getTail(args) || (getTail(args)->type==closeType)){
    Value* value = (Value *)malloc(sizeof(Value));
    value->type = booleanType;
    value->boolValue = 1;
    insertItem(env->bindings->tableValue,"#returnValue",value); // value is copied into the hash table.
    free(value);  // value can be freed since there is one copy in hash table.
    value = lookup(env->bindings->tableValue,"#returnValue");
    return value;
  }else{
    //findLast(args); // remove close parenthesis.
    Value *toCheck  = args, *toValidate = NULL;
    while (getFirst(toCheck)){
      if (getFirst(toCheck)->type ==closeType){
	break;
      }
      toValidate = eval(getFirst(toCheck),env);
      if (!toValidate){
	printf("Syntax error in: (and ");
	printValue(args);
	printf("\n");
	return NULL;
      }else if (toValidate->type == booleanType && (!getFirst(toCheck)->boolValue)){
	Value* value = (Value *)malloc(sizeof(Value));
	value->type = booleanType;
	value->boolValue = 0;
	insertItem(env->bindings->tableValue,"#returnValue",value); // value is copied into the hash table.
	free(value);  // value can be freed since there is one copy in hash table.
	value = lookup(env->bindings->tableValue,"#returnValue");
	return value;
      }
      toCheck = getTail(toCheck);
    }
    return toValidate;
  }
}

Value *evalCond(Value *args, Environment *env){
  if (!(args && (args->type == cellType)))
   return NULL;
  int count = listLength(args);
  
  if (count < 1) {
    // It is okay not to have an argument. Just return NULL.
    return NULL;
  } else if ((count == 1) && (getFirst(args)->type == nullType)) {
    printf("cond: cond called on nullType\n");
    return NULL;
  }

  int i; // loop through all the arguments
  Value *current;
  Value *tail;
  Value *evalTest;
  current = getFirst(args);
  tail = getTail(args);
  
  if (getFirst(getTail(current))->type == stringType) {
    if (strcmp(getFirst(getTail(current))->stringValue, "else") == 0) {
      return eval(getFirst(getTail(getTail(current))), env);
     } 
  }
  // printValue(getFirst(getTail(current)));
  evalTest = eval(getFirst(getTail(current)), env);
  for (i=0; i < count - 1; i++) {
    if (current) {
      if (evalTest && 
	  evalTest->type == booleanType &&
	  !(evalTest->boolValue)) {
	current = getFirst(tail);
	tail = getTail(tail);
	evalTest = eval(getFirst(getTail(current)), env);
      } else {
	return eval(getFirst(getTail(getTail(current))), env);
      }
    }
  }
  // then return;

  Value *evalEnd;
  if (!(tail && current)) return NULL;
  else if (getFirst(getTail(current))->type == stringType) {
    if (strcmp(getFirst(getTail(current))->symbolValue, "else") == 0) {
      return eval(getFirst(getTail(getTail(current))), env);
    }
  } else {
    evalEnd =  eval(getFirst(getTail(current)), env);
    if (evalEnd && evalEnd->type == booleanType && !(evalEnd->boolValue)) 
      return NULL;
    else return eval(getFirst(getTail(getTail(current))), env);
  }
  return NULL;
}


Value *evalOr(Value *args, Environment *env){
  assert(env!=NULL);
  assert(env->bindings!=NULL);
  assert(args!=NULL);
  assert(args->type == cellType);
  
  if (!getTail(args)){
    Value* value = (Value *)malloc(sizeof(Value));
    value->type = booleanType;
    value->boolValue = 0;
    insertItem(env->bindings->tableValue,"#returnValue",value); // value is copied into the hash table.
    free(value);  // value can be freed since there is one copy in hash table.
    value = lookup(env->bindings->tableValue,"#returnValue");
    return value;
  }else{
    Value *toValidate = NULL;
    toValidate = eval(getFirst(args),env);
    if (!toValidate){
      printf("Syntax error in: (or ");
      printValue(args);
      printf(")\n");
      return NULL;
    }   
    return toValidate;
  }
}






Environment* checkEnv(char* id, Environment* env){
  Environment* returnEnv = env;
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
  if(returnValue){
    return env;
  }else{
    return NULL;
  }
}



Value *evalSetBang(Value *args, Environment *env){
 //we should modify let functions in order to handle those cases like(let ((y 4)) (set! y 3))
 int count = listLength(args);
 if (count < 1||count > 2){
   printf("set!: has %d parts after key word in ", count);
   printValue(args);
   printf("\n");
   return NULL;
 }
 if (getFirst(args)->type != symbolType){
   printf("set!: not an indentifier ");
   printValue(args);
   printf("\n");
   return NULL;
 }else{
   if(getFirst(args)->type == symbolType && variableCheck(getFirst(args)) == 0){
     printf("set!: cannot mutate module-required identifier\n");
     return NULL;
   }else{
     if((envLookup(getFirst(args)->symbolValue, env)) == NULL){
	  printf("set!: cannot set undefined variable ");
	  printValue(getFirst(args));
	  printf("\n");
	  return NULL;
	}else{
	Environment* newEnv= checkEnv(getFirst(args)->symbolValue, env);
	Value* toBind = eval(getFirst(getTail(args)), env);
	  if(toBind && newEnv){
	    insertItem(newEnv->bindings->tableValue, getFirst(args)->symbolValue, toBind);
	    return NULL;
	  }else{
	    printf("bad syntax\n");
	    return NULL;
	  }
	}
   }
 }
}

Value *smallerOrEqualTo(Value *args, Environment *env){
  assert(args->type == cellType);
  int count = listLength(args);
  if (count < 1){
    printf("in <=: expect at least two arguments, given %d /n", count);
  }else{
    Value* checkArgs = args;
    while(args){
      Value* toCheck = getFirst(args);
   
      if (getFirst(args) -> type != integerType && getFirst(args) -> type != floatType){
	printf("in <=:expects type <real number>, given:");
	printValue(toCheck);
	printf("\n");
	return NULL;
      }
      args = getTail(args);
    }
    
    args = checkArgs;
    int i;
    Value* value = (Value *)malloc(sizeof(Value));
    value -> type = booleanType;
    value -> boolValue = 1;
    for (i = 0; i < count - 1; i++) {
      if(getFirst(args)->type == integerType){
 
	if(getFirst(getTail(args))->type == integerType){
	
	  if (getFirst(getTail(args))->intValue < getFirst(args)->intValue){
	    value -> boolValue = 0;	
	  }
	}else{
	  if (getFirst(getTail(args))->dblValue < getFirst(args)->intValue){
	    value -> boolValue = 0;	  
	  }
	}
      }else{
       	if(getFirst(getTail(args))->type == floatType){
	  if (getFirst(getTail(args))->dblValue < getFirst(args)->intValue){
	    value -> boolValue = 0;	
	  }	  
	  
	}else{
	  if (getFirst(getTail(args))->dblValue < getFirst(args)->dblValue){
	    value -> boolValue = 0;	  
	  }
	}
      }
      args = getTail(args);
    }
    return value;
    
    insertItem(env->bindings->tableValue,"#returnValue",value); // value is copied into the hash table.
    free(value);  // value can be freed since there is one copy in hash table.
    value = lookup(env->bindings->tableValue,"#returnValue");
  } 
}









Value *greaterOrEqualTo(Value *args, Environment *env){
  assert(args->type == cellType);
  int count = listLength(args);
  if (count < 1){
    printf("in >=: expect at least two arguments, given %d /n", count);
  }else{
    Value* checkArgs = args;
    while(args){
      Value* toCheck = getFirst(args);
   
      if (getFirst(args) -> type != integerType && getFirst(args) -> type != floatType){
	printf("in >=:expects type <real number>, given:");
	printValue(toCheck);
	printf("\n");
	return NULL;
      }
      args = getTail(args);
    }
    
    args = checkArgs;
    int i;
    Value* value = (Value *)malloc(sizeof(Value));
    value -> type = booleanType;
    value -> boolValue = 1;
    for (i = 0; i < count - 1; i++) {
      if(getFirst(args)->type == integerType){
 
	if(getFirst(getTail(args))->type == integerType){
	
	  if (getFirst(getTail(args))->intValue > getFirst(args)->intValue){
	    value -> boolValue = 0;	
	  }
	}else{
	  if (getFirst(getTail(args))->dblValue > getFirst(args)->intValue){
	    value -> boolValue = 0;	  
	  }
	}
      }else{
       	if(getFirst(getTail(args))->type == floatType){
	  if (getFirst(getTail(args))->dblValue > getFirst(args)->intValue){
	    value -> boolValue = 0;	
	  }	  
	  
	}else{
	  if (getFirst(getTail(args))->dblValue > getFirst(args)->dblValue){
	    value -> boolValue = 0;	  
	  }
	}
      }
      args = getTail(args);
    }
    return value;
    
    insertItem(env->bindings->tableValue,"#returnValue",value); // value is copied into the hash table.
    free(value);  // value can be freed since there is one copy in hash table.
    value = lookup(env->bindings->tableValue,"#returnValue");
  } 
}

Value *greaterThan(Value *args, Environment *env){
  assert(args->type == cellType);
  int count = listLength(args);
  if (count < 1){
    printf("in >: expect at least two arguments, given %d /n", count);
  }else{
    Value* checkArgs = args;
    while(args){
      Value* toCheck = getFirst(args);
   
      if (getFirst(args) -> type != integerType && getFirst(args) -> type != floatType){
	printf("in >:expects type <real number>, given:");
	printValue(toCheck);
	printf("\n");
	return NULL;
      }
      args = getTail(args);
    }
    
    args = checkArgs;
    int i;
    Value* value = (Value *)malloc(sizeof(Value));
    value -> type = booleanType;
    value -> boolValue = 1;
    for (i = 0; i < count - 1; i++) {
      if(getFirst(args)->type == integerType){
	if(getFirst(getTail(args))->type == integerType){
	
	  if (getFirst(getTail(args))->intValue >= getFirst(args)->intValue){
	    value -> boolValue = 0;	
	  }
	}else{
	  if (getFirst(getTail(args))->dblValue >= getFirst(args)->intValue){
	    value -> boolValue = 0;	  
	  }
	}
      }else{
       	if(getFirst(getTail(args))->type == floatType){
	  if (getFirst(getTail(args))->dblValue >= getFirst(args)->intValue){
	    value -> boolValue = 0;	
	  }	  
	  
	}else{
	  if (getFirst(getTail(args))->dblValue >= getFirst(args)->dblValue){
	    value -> boolValue = 0;	  
	  }
	}
      }
      args = getTail(args);
    }
    return value;
    
    insertItem(env->bindings->tableValue,"#returnValue",value); // value is copied into the hash table.
    free(value);  // value can be freed since there is one copy in hash table.
    value = lookup(env->bindings->tableValue,"#returnValue");
  } 
}



Value *smallerThan(Value *args, Environment *env){
  assert(args->type == cellType);
  int count = listLength(args);
  if (count < 1){
    printf("in <: expect at least two arguments, given %d /n", count);
  }else{
    Value* checkArgs = args;
    while(args){
      Value* toCheck = getFirst(args);
   
      if (getFirst(args) -> type != integerType && getFirst(args) -> type != floatType){
	printf("in <:expects type <real number>, given:");
	printValue(toCheck);
	printf("\n");
	return NULL;
      }
      args = getTail(args);
    }
    
    args = checkArgs;
    int i;
    Value* value = (Value *)malloc(sizeof(Value));
    value -> type = booleanType;
    value -> boolValue = 1;
    for (i = 0; i < count - 1; i++) {
      if(getFirst(args)->type == integerType){
 
	if(getFirst(getTail(args))->type == integerType){
	
	  if (getFirst(getTail(args))->intValue <= getFirst(args)->intValue){
	    value -> boolValue = 0;	
	  }
	}else{
	  if (getFirst(getTail(args))->dblValue <= getFirst(args)->intValue){
	    value -> boolValue = 0;	  
	  }
	}
      }else{
       	if(getFirst(getTail(args))->type == floatType){
	  if (getFirst(getTail(args))->dblValue <= getFirst(args)->intValue){
	    value -> boolValue = 0;	
	  }	  
	  
	}else{
	  if (getFirst(getTail(args))->dblValue <= getFirst(args)->dblValue){
	    value -> boolValue = 0;	  
	  }
	}
      }
      args = getTail(args);
    }
    return value;
    
    insertItem(env->bindings->tableValue,"#returnValue",value); // value is copied into the hash table.
    free(value);  // value can be freed since there is one copy in hash table.
    value = lookup(env->bindings->tableValue,"#returnValue");
  } 
}


// arithmetic equality check
Value *arithmeticEqual(Value *args, Environment *env){
  if (!(args && (args->type == cellType)))
    return NULL;
  
  int count = listLength(args);
  
  if (count < 2) {
    printf("=: expects at least two arguments\n");
    return NULL;
  }
  Value *current;
  Value *tail;
  current = getFirst(args);
  tail = getTail(args);

  assert(current != NULL);
  assert(tail != NULL);
  
  int i;
  int equal = 1;
    
  for (i = 0; i < count - 1; i++) {
    if (((current->type == floatType) || (current->type == integerType)) &&
	((getFirst(tail)->type == floatType) || (getFirst(tail)->type == integerType))) {
      if ((current->type == floatType) && (getFirst(tail)->type == floatType)) {
	if (current->dblValue != getFirst(tail)->dblValue)
	  equal = 0;
      } else if ((current->type == integerType) && (getFirst(tail)->type == integerType)) {
	if (current->intValue != getFirst(tail)->intValue)
	  equal = 0;
      } else if ((current->type == integerType) && (getFirst(tail)->type == floatType)) {
	if (((double)current->intValue) != getFirst(tail)->dblValue)
	  equal = 0;
      } else if ((current->type == floatType) && (getFirst(tail)->type == integerType)) {
	if (current->dblValue != ((double)getFirst(tail)->intValue))
	  equal = 0;
      }
      current = getFirst(tail);
      tail = getTail(tail);
    } else {
      printf("=: one of your arguments isn't an int or float.\n");
      return NULL;
    }
  }
  Value *value = (Value *)malloc(sizeof(Value));
  value->type = booleanType;

  if (equal) {
    value->boolValue = 1;
  } else
    value->boolValue = 0;
  return value;
}


// check whether things are equal.
int checkEqual(Value *first, Value *second, Environment *env){
  if (!first || !second){
    return 0;
  }
  int equal = 1;
  if (first->type!=second->type){
    equal = 0;
  }else if (first->type == floatType){
    if (first->dblValue != second->dblValue){
      equal = 0;
    }
  }else if (first->type == integerType) {
    if (first->intValue != second->intValue){
      equal = 0;
    }
  }else if (first->type == booleanType) {
    if (first->boolValue != second->boolValue){
      equal = 0;
    }
  }else if (first->type == nullType || first->type == openType || first->type == closeType){
    equal = 1;
  }else if (first->type == stringType) {
    if (strcmp(first->stringValue, second->stringValue)!=0){
      equal = 0;
    }
  }else if (first->type == symbolType){
    first = eval(first, env);
    second = eval(second, env);
    equal = checkEqual(first, second, env);
  }else if (first->type == closureType){
    if (first->closureValue->identifier!=NULL && second->closureValue->identifier!=NULL){
      if (strcmp(first->closureValue->identifier,second->closureValue->identifier)==0){
	equal = 1;
      }else{
	equal = 0;
      }
    }else{
      equal = 0;
    }
  }else if (first->type == primitiveType){
    if (first->primitiveValue != second->primitiveValue){
      equal = 0;
    } 
  }else{
    equal = 0;
  }
  return equal;
}

// not sure whether this function will break in some cases.
Value *equality(Value *args, Environment *env){
  if (!(args && (args->type == cellType)))
    return NULL;
  
  int count = listLength(args);
  
  if (count = 0) {
    printf("equal?: expects two arguments, given 0\n");
    return NULL;
  }else if (count == 1) {
    printf("equal?: expects two arguments, given 1: ");
    printValue(args);
    printf("\n");
    return NULL;
  }else if (count > 2) {
    printf("equal?: expects two arguments, given %d: ", count);
    printValue(args);
    printf("\n");
    return NULL;
  }
  Value *first = getFirst(args);
  Value *second = getFirst(getTail(args));
  int equal = checkEqual(first, second, env);

  Value *value = (Value *)malloc(sizeof(Value));
  value->type = booleanType;

  if (equal) {
    value->boolValue = 1;
  } else{
    value->boolValue = 0;
  }
  assert(env->bindings!=NULL);
  insertItem(env->bindings->tableValue, "#returnValue",value);
  free(value);
  value = lookup(env->bindings->tableValue, "#returnValue");
  return value;
}

// not finished yet.
Value *exponentiate(Value *args, Environment *env){
  return NULL;
}



Environment *searchTopLevel(Environment* current){
  Environment* returnEnv = current;
  if (!current){
    return NULL;
  }
  while (returnEnv->parent){
    returnEnv = returnEnv->parent;
  }
  return returnEnv;
}


// Insert things into top-level environment.
Environment* insertEnv2Top(Environment* toInsert){
  Environment *top = searchTopLevel(toInsert);
  if (!top){
    return NULL;
  }
  if (top == toInsert){
    return top;
  }
  return insertEnv(toInsert, top);
}

Environment* insertEnv(Environment* toInsert, Environment *parent){
  if (!toInsert || !parent){
    return NULL;
  }
  Value *value = (Value *)malloc(sizeof(Value));
  value->type = envType;
  value->envValue = toInsert;
  Value  *currentID = lookup(parent->bindings->tableValue, "#envID");

  char *id = intToString(currentID->intValue);  // string is mallocated inside intToString function.
  //printf("current ID = %s \n",id);
  currentID->intValue +=1;
  insertItem(parent->bindings->tableValue, id, value);
  Environment *toReturn = lookup(parent->bindings->tableValue,id)->envValue;
  
  free(id);
  free(value);
  return toReturn;
}


// not finished. Will continue soon.
Value* checkNull(Value *value, Environment *env){
  assert(env!=NULL);
  int count = listLength(value);
  Value *toCheck;
  Value *current;
  Value *returnValue = (Value *)malloc(sizeof(Value));
  
  if (count <1){
    printf("null?: expects 1 argument, given 0\n");
    returnValue->type = errorType;
    insertItem(env->bindings->tableValue,"#returnValue",returnValue);
    free(returnValue);
    return lookup(env->bindings->tableValue,"#returnValue");
  }else if (count>1){
    printf("null?: expects 1 argument, given %d\n",count);
    returnValue->type = errorType;
    insertItem(env->bindings->tableValue,"#returnValue",returnValue);
    free(returnValue);
    return lookup(env->bindings->tableValue,"#returnValue");
  } else{
    // assert(value != NULL);
    current = getFirst(value);
    if (current->type ==cellType){
      if (getFirst(current) && getFirst(current)->type ==nullType)
	current = getFirst(current);
    }
    // assert(current != NULL);
    returnValue->type = booleanType;
   
    if (current && current->type == nullType) {
      returnValue->boolValue = 1;
    } else {
      returnValue->boolValue = 0;
    }
  
  insertItem(env->bindings->tableValue,"#returnValue",returnValue);
  free(returnValue);
  return lookup(env->bindings->tableValue,"#returnValue");
  }
  return NULL;
}
