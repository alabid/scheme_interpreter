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
	}
	while (operator->type == cellType){
	 
	  operator = eval(operator, env);

	  if (!operator){
	    printf("Invalid procedure!\n");
	    return NULL;
	  }

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
	  }else if (strcmp(operator->symbolValue,"let*")==0){
	    /*eval let* goes here*/
	    return evalLetStar(args, env);
	  } else if(strcmp(operator->symbolValue, "load") == 0) {
	    while (env && env->parent){
	      env = env->parent;
	    } // go to top-level environment.
	    return loadFunction(args, env);
	  } else{
	  
	    if (validateArgs(args, env)==-1){
	      printf("Syntax error! Invalid arguments for the procedure: ");
	      printValue(operator);
	      printf("\n");
	      return NULL;
	    } 
	    Value *evaledOperator = eval(operator, env);
	    Value *evaledArgs = evalEach(args, env);
	    if (!evaledOperator){
	      printf("Unknown procedure: ");
	      printValue(operator);
	      printf("\n");
	    }
	    
	    return apply(evaledOperator, evaledArgs);
	  } 
	}else if (typeCheck(operator)==1){
	  printf("A literal ");
	  printValue(operator);
	  printf(" cannot be a procedure.\n");
	  return NULL; 
	}else if (typeCheck(operator)==2){
	  //printf("Argument list is ");
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
	  return apply(operator, evaledArgs);	  
	}
      }else if (typeCheck(getFirst(expr))==1){
	
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
	    
	    printf("Unknown identifier %s.\n",operator->symbolValue);
	  }
	  
	  return NULL;
	}
      }
      
    case closeType:
      
      return NULL;
      break;
    default:
      
      if (getTail(expr)){
	assert(getFirst(getTail(expr))!=NULL);
	assert(getFirst(getTail(expr))->type==closeType);
	Value *toRemove = getTail(expr);
	free(toRemove->cons->car);
	free(toRemove->cons);
	free(toRemove);
	expr->cons->cdr = NULL;
	
      }
      return expr;      
    }
}

int validateArgs(Value *value, Environment* env){
 
  if (value && value->type==cellType){
  
    Value *toCheck = getFirst(value);
    if (toCheck){
   
      switch (toCheck->type)
	{
	case cellType:
	case openType:
	  return 2;
	  break;
	case symbolType:
	  toCheck = eval(toCheck,env);
	  
	  if (toCheck && (toCheck->type==primitiveType || toCheck->type == closureType)){
	    return -1;
	  }else{
	    return 1;
	  }
	  break;
	case primitiveType:
	case closureType:
	  return -1;
	default:
	  break;
	}
    }
  }
  
  return 0;
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
  //printf("printing actual arguments: ");
  //printValue(actualArgs);
  //printf("\n");
 
  if (!function){
    return actualArgs;
  }else if (function->type == primitiveType){
    return function->primitiveValue(actualArgs);
  }else if (function->type == closureType){
    List *formalArgs = function->closureValue->args;
    //eval(formalArgs->head, function->closureValue->parent);
    Environment *frame = createFrame(function->closureValue->parent);
    /* Bind formalArgs to actualArgs in frame here. */
    Value *curArg = formalArgs->head;
    Value *curValue = actualArgs;
    while (curArg && curValue){
      assert(getFirst(curArg)->type ==symbolType);
      //printf("the binding: ");
      //printValue(getFirst(curArg));
      //printf(" => ");
      //printValue(getFirst(curValue));

      insertItem(frame->bindings->tableValue, getFirst(curArg)->symbolValue, getFirst(curValue));
      //printf("  and find it =>");
      //printValue(lookup(frame->bindings->tableValue, getFirst(curArg)->symbolValue));
      //printf("\n");
      curArg = getTail(curArg);
      curValue = getTail(curValue);
      
    }
    if (curArg || curValue){
      printf("Wrong number of parameters for the procedure.\n");
      // destroyFrame(frame);
      return NULL;
    }
    
    Value *returnValue = eval(function->closureValue->body, frame);
    destroyEnvironment(frame);
    return returnValue;
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
      //assert(getFirst(args)->symbolValue);
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
  Value *head = args, *openParen;
  while (args){ 

    assert(args->type==cellType);    
    //temp = args->cons->car;
    if (getFirst(args) && (getFirst(args))->type==cellType){
      tail = getTail(args);
   
      openParen = getFirst(getFirst(args));
  
      if (openParen && openParen->type == openType){

	temp =  deepCopy(eval(getFirst(args), env));

	//printf("printing temp:");
	//printValue(temp);
	//printf("\n");
	//assert(getFirst(getFirst(args))!=NULL);
	//assert(getFirst(getFirst(args))->type==openType);
	assert(args!=NULL);
	assert(args->type==cellType);
	assert(args->cons->car !=NULL);
	//assert(args->cons->car->type ==integerType);
	//free(args->cons->car);
	freeValue(args->cons->car);
	
	args->cons->car = temp;
	if (!temp){
	  return NULL;
	}
	//printf("printing tail:");
	//printValue(tail);
	//printf("\n");
	/*
	if (!temp && tail && tail->type == cellType && tail->cons->car &&  tail->cons->car->type == closeType){
	  cleanup(tail);
	  args->cons->cdr = NULL;
	  free(args->cons);
	  free(args);
	  previous->cons->cdr = NULL;
	  return NULL;
	  break;
	}else if (!temp){
	  free(args->cons);
	  free(args);
	  previous->cons->cdr = NULL;
	  return NULL;
	  break;
	  }*/
	previous = args;
	args = tail;

      }else{
	printf("error\n");
	return NULL;
      }
      
    }else{
      
      tail = getTail(args);
      
      //printf("start to print:  ");
      //printValue(eval(args->cons->car, env));
      //printf("  done\n");
      
      temp = deepCopy(eval(args->cons->car, env));
      
      if (temp==NULL){ 	
	//printf("NOPE\n");
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
	
      }
      previous = args;
      args = tail;
    }
    
  }
  return head;
}


Value* evalLetrec(Value* args, Environment* env){
  Value *toCheck;
  //removeLast(args);
  int count = listLength(args);
  if (count < 2){
    printf("letrec: bad syntax in: (letrec ");
    printValue(args);
    printf("\n");
    return NULL;
  }
  if (getFirst(args)->type== nullType){
    while (getTail(getTail(getTail(args)))){
      eval(getTail(args),env);
      args = getTail(args);
    }
    return eval(getTail(args), env);
  }
  printf("in let: ");
  printValue(getFirst(getTail(getFirst(getTail(getFirst(args))))));
  printf("\n");
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
    printf("in let 2: ");
    removeLast(toCheck);
    printValue(toCheck);
    
    printf("\n");
    if (getFirst(getTail(getFirst(toCheck))) -> type !=symbolType){
      printf("bad syntax in letrec: not an indentifier\n");
      return NULL;
    }else{
      Environment* newEnv = createFrame(env);
      Value* listofBinds = toCheck;
      
      while (listofBinds){
	insertItem(newEnv->bindings->tableValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue, NULL);
	listofBinds = getTail(listofBinds);
      }
      //printTable(newEnv->bindings->tableValue);
      //printf("\n");
   
      //int i;
      // for (i=0;i<2;i++){
      // if (i==1 && !unknown){
      //  break;
      //}
      listofBinds = toCheck;
      while (listofBinds){
	printf("in let 3: ");
	printValue((getFirst(listofBinds)));
	//if (i==0){
	removeLast(getTail(getFirst(listofBinds)));
	//}
	
	printf("\n");
	printf("in let 4: ");
	printValue(getTail(getFirst(listofBinds)));
	printf("\n");
	Value* toBind = getTail(getTail(getFirst(listofBinds)));
	if (typeCheck(toBind) > 0 && typeCheck(toBind)!=3){
	  Value* toBind = eval(getTail(getTail(getFirst(listofBinds))), newEnv);
	}else{
	  toBind = NULL;
	}
	//if (toBind){
	printf("in let 4.5: ");
	printValue(toBind);
	printf("  and the type of it is: %d\n",toBind->type);
	printf("\n");
	if (toBind){
	  if (toBind->type == cellType){
	    if (getTail(toBind)){
	      printf("syntax error in letrec: too many values for single identifier.\n");
	    }else{
	      toBind = getFirst(toBind);
	    }
	  }
	}
	insertItem(newEnv->bindings->tableValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue, toBind);
	//}else if(toBind->type == symbolType){
	//Value *value = eval(envLookup(toBind->symbolValue, newenv), newenv);
	//if (value){
	//  insertItem(newenv->bindings->tableValue, toBind->symbolValue, value);
	//}else{
	//printf("syntax error: unknown identifier\n");
	//free(toBind);
	//free(value);
	//return NULL;
	//}
	//}else{ 
	//printf("syntax error in letrec\n");
	//return NULL;
	
	//}else{
	// listofBinds = getTail(listofBinds);
	//}
        listofBinds = getTail(listofBinds);
        printValue(listofBinds);
        printf("\n");
      }
      listofBinds = toCheck;
      while(listofBinds){
	removeLast(getTail(getFirst(listofBinds)));
	assert(getFirst(getTail(getFirst(listofBinds)))->type == symbolType);
	if (envLookup(getFirst(getTail(getFirst(listofBinds)))->symbolValue, newEnv) == NULL){
	  Value* checkBind = (getTail(getFirst(getTail(getFirst(listofBinds)))));
	  printf("in let 4.75: ");
 
	  printValue(checkBind);
	  printf("  and the type of it is: %d\n",checkBind->type);
	  printf("\n");
	  if (checkBind->type == symbolType){
	    if(envLookup(checkBind->symbolValue, newEnv) != NULL){
	      insertItem(newEnv->bindings->tableValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue, checkBind);
	    }else{
	      printf("syntax eror: unknown identifier");
	    }
	  }else{
	    printf("syntax error: expr not allowed in this expression");
	  }
	}
        listofBinds = getTail(listofBinds);
      }
      
      Value* listofExpressions = getTail(args);
      printf("in let 5: ");
      removeLast(listofExpressions);
      printValue(listofExpressions);
      printf("  and the type of it is: %d\n",getFirst(listofExpressions)->type);
      printf("\n");
      if (!listofExpressions){
	printf(" letrec: bad syntax.\n ");
	return NULL;
      }
      while(listofExpressions){
	Value* toReturn = eval(getFirst(listofExpressions), newEnv);
	printf("in let 6: ");
	//removeLast(getTail(getFirst(listofBinds)));
	printValue(toReturn);
	if (toReturn)
	  printf("  and the type of it is: %d\n",toReturn->type);
	printf("\n");
	if(typeCheck(toReturn) < 3 && typeCheck(toReturn) > 0 ){
	  if(getTail(listofExpressions)){
	    printf("in let 7: ");
	    //removeLast(getTail(getFirst(listofBinds)));
	    
	    printValue(getTail(listofExpressions));
	    if (getTail(listofExpressions))
	      printf("  and the type of it is: %d\n",getTail(listofExpressions)->type);
	    printf("\n");
	    listofExpressions = getTail(listofExpressions);
	    //printf("hello world");
	  }else{
	    return toReturn;
	  }
	}else{
	  listofExpressions = getTail(listofExpressions);
	}
      }
      printf(" letrec: bad syntax\n");

      return NULL;
    }
  }
  return NULL;
}


Value* evalLet(Value* args, Environment* env){
  Value *toCheck;
 
  int count = listLength(args);
  if (count < 2){
    printf("let: bad syntax in: (let ");
    printValue(args);
    printf("\n");
    return NULL;
  }
  if (getFirst(args)->type== nullType){
    while (getTail(getTail(getTail(args)))){
	eval(getTail(args),env);
	args = getTail(args);
    }
    return eval(getTail(args), env);
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
    
    removeLast(toCheck);
   
    if (getFirst(getTail(getFirst(toCheck))) -> type !=symbolType){
      printf("bad syntax in let: not an indentifier\n");
      return NULL;
    }else{
      Environment* newEnv = createFrame(env);
      Value* listofBinds = toCheck;
      while(listofBinds){

	removeLast(getTail(getFirst(listofBinds)));

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
	  }
	  insertItem(newEnv->bindings->tableValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue, toBind);
	  
	}else{
	  printf("syntax error\n");
	  destroyEnvironment(newEnv);
	  return NULL;
	}
	
	listofBinds = getTail(listofBinds);
	
      }
      Value* listofExpressions = getTail(args);
     
      removeLast(listofExpressions);
     
      if (!listofExpressions){
	printf("syntax error in let: bad syntax.\n ");
	destroyEnvironment(newEnv);
	return NULL;
      }
      while(listofExpressions){
	Value* toReturn = eval(getFirst(listofExpressions), newEnv);

	if(typeCheck(toReturn) < 3 && typeCheck(toReturn) > 0 ){
	  if(getTail(listofExpressions)){
	  
	  
	    listofExpressions = getTail(listofExpressions);
	   
	  }else{
	    destroyEnvironment(newEnv);
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
// This function evaluates if statement.
Value* evalIf(Value* args, Environment* env){
  // args = evalEach(args,env);
  
  int count = listLength(args);
  // printf("count = %d \n",count);
  // printValue(getTail(args));
  // printValue(getTail(getTail(args)));
  
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

// not finished yet.
Value* evalLambda(Value* args, Environment* env){
  if (!env || !args){
    printf("Syntax error for lambda. Missing components here\n");
    return NULL;
  }
  assert(args->type = cellType);
  Value *toCheck = getFirst(args);
  //printf("start printing for lambda: ");
  //printValue(toCheck);
  //printf("\n");
  if (getFirst(toCheck) && getFirst(toCheck)->type == openType){
    Value *returnValue = (Value *)malloc(sizeof(Value));
    returnValue->type = closureType;
    Closure *closure = initializeClosure(env);
    toCheck = getTail(toCheck);
    
    while (toCheck && getFirst(toCheck) && getFirst(toCheck)->type!=closeType){
      push(closure->args, deepCopy(getFirst(toCheck)));
      toCheck = getTail(toCheck);
    } 
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
   
    //printf("Show me the parse tree: ");
    //printValue(getFirst(toCheck));
    //printf("\n");
    closure->body = deepCopy(getFirst(toCheck));
    returnValue->closureValue = closure;
 
    return returnValue; 
  }else if (toCheck && toCheck->type==nullType){
    Value *returnValue = (Value *)malloc(sizeof(Value));
    returnValue->type = closureType;
    Closure *closure = initializeClosure(env);
    toCheck = getTail(args);
    
    //removeLast(toCheck);
    closure->body = deepCopy(getFirst(toCheck));
    returnValue->closureValue = closure;
    // printf("Show me the parse tree: ");
    //printValue(getFirst(toCheck));
    //printf("\n");
    return returnValue; 
  }else{
    
    printf("Syntax error for lambda! Missing parameters.\n");
    return NULL;
  }



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
  bind("+", makePrimitiveValue(add), frame);
  bind("-", makePrimitiveValue(subtract), frame);
  bind("*", makePrimitiveValue(multiply), frame);
  bind("/", makePrimitiveValue(divide), frame);
  // bind("exp", makePrimitiveValue(exponentiate), frame);
  // bind("load", makePrimitiveValue(loadFunction), frame);
  bind("car", makePrimitiveValue(car), frame);
  bind("cdr", makePrimitiveValue(cdr), frame);
  return frame;
}

void freeTopFrame(Environment *env){
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



// Arithmetic addition function.
Value *add(Value *args){
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
	printf("+: expects type <number> as arguments");
	return NULL;
      }
      args = getTail(args);
    }
    
    
    cleanup(getTail(head));
  }
  Value *value = (Value*) malloc(sizeof(Value));
 
  if (isFloat){
    value->type = floatType;
      value->dblValue = dblSum; 
  }else{
    
    value->type = integerType;
    value->intValue = intSum; 
  }
  if (head){
    freeValue(head->cons->car);
    
    head->cons->car = value;
    head->cons->cdr = NULL;
   
  }
  return value;
}



// Arithmetic subtraction.
Value *subtract(Value *args){
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
	printf("-: expects type <number> as arguments");
	return NULL;
      }
      args = getTail(args);
    }
    
    
    cleanup(getTail(head));
  }
  Value *value = (Value*) malloc(sizeof(Value));

  if (isFloat){
    value->type = floatType;
      value->dblValue = dblDiff; 
  }else{
    
    value->type = integerType;
    value->intValue = intDiff; 
  }
  if (head){
    freeValue(head->cons->car);
    
    head->cons->car = value;
    head->cons->cdr = NULL;
   
  }
  return value;
}

// Arithmetic mutiplication.
Value *multiply(Value *args){
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
	printf("*: expects type <number> as arguments");
	return NULL;
      }
      args = getTail(args);
    }
    
   
    cleanup(getTail(head));
  }
  Value *value = (Value*) malloc(sizeof(Value));
  //result = getFirst(head);
  if (isFloat){
    value->type = floatType;
    value->dblValue = dblProd; 
  }else{
    
    value->type = integerType;
    value->intValue = intProd; 
  }
  if (head){
    freeValue(head->cons->car);
    
    head->cons->car = value;
    head->cons->cdr = NULL;
 
  }
  return value;
}

/* 
   Arithmetic division. We do not allow zero division 
   even though it is allowed in DrRacket that (/ 5 0.0)=>+inf.0
*/
Value *divide(Value *args){
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
	printf("*: expects type <number> as arguments");
	return NULL;
      }
      args = getTail(args);
    }
    
    cleanup(getTail(head));
  }
  Value *value = (Value*) malloc(sizeof(Value));
  
  if (isFloat){
    value->type = floatType;
    value->dblValue = dblQuot; 
  }else{
    
    value->type = integerType;
    value->intValue = intQuot; 
  }
  if (head){
    freeValue(head->cons->car);
    
    head->cons->car = value;
    head->cons->cdr = NULL;
  }
  return value;
}


// Load function.
Value *loadFunction(Value *args, Environment *env){
  // read in the lines of a file one at a time
  // eval each line line by line
  // char *expression = (char *)malloc(256 * sizeof(char));
  int count = listLength(args);

  if (count > 1) {
    printf("load: load one file at a time\n");
    // free(expression);
    cleanup(args);
    return NULL;
  } else if (count < 1) {
    printf("load: you must enter the name of a file to load\n");
    // free(expression);
    cleanup(args);
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

    //while (fgets(expression, 256, fp)) {
    //printf("line: %s\n", expression);
    //} 
    // somehow do it for the current environment
    loadFromFile(fp, env);
    // free(expression);
    // then close file somehow
    cleanup(args);
    fclose(fp);
    return NULL;
  }  else {
    // free(expression);
    cleanup(args);
    return NULL;
  }
  return NULL;
}


int loadFromFile(FILE *file, Environment *env) {
  List *tokens, *parseTree, *leftoverTokens = NULL;
  leftoverTokens = initializeList();
  int depth = 0;
  char *expression = (char *)malloc(256 * sizeof(char));
  Value* temp;
  while (fgets(expression, 256, file)) {
    
     tokens = append(leftoverTokens, tokenize(expression)); 
     
     if (!tokens) {
       leftoverTokens->head = NULL;
       continue;
     }
    
     parseTree = parse(tokens,&depth);   
     
     if (depth < 0) {
       printf("syntax error. Too many close parentheses.\n");   // Too many close parentheses. 
       cleanup(tokens->head);
       //free(parseTree);
       free(leftoverTokens);
       free(tokens);
       return SYNTAX_ERROR_TOO_MANY_CLOSE_PARENS;
     } else if (depth > 0) {
       // There are more open parens than close parens, so these tokens are saved as leftovers. We partially generate a parse tree now.
       leftoverTokens->head = tokens->head;
       
      } else {
       if (parseTree && parseTree->head){
	 //printf("going to print parse tree: ");
	 //printValue(parseTree->head);
	 //printf("\n");
	 //printf("going to print parse tree again: ");
	 //printValue(deepCopyList(parseTree->head));
	 //printf("\n");
	 temp = eval(parseTree->head,env);
	 if (temp){
	   printValue(temp);
	   printf("\n");
	 }
	 
	 //leftoverTokens->head = tokens->head;
	 destroy(parseTree);
	 // cleanup(leftoverTokens->head);
       }
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


// bind primitive functions to the top-level environment.
void bind(char identifier[], Value *function, Environment *env){
  if (!env || !function){
    return;
  }
  char *variable = (char *)malloc(sizeof(char)*(strlen(identifier)+1));
  strcpy(variable, identifier);
  variable[strlen(identifier)]='\0';
  insertItem(env->bindings->tableValue, variable, function);
}


// To implement in round 2
Value *evalAnd(Value *args, Environment *env){
  return NULL;
}

Value *evalCond(Value *args, Environment *env){
  return NULL;
}
Value *evalOr(Value *args, Environment *env){
  return NULL;
}

Value *evalSetBang(Value *args, Environment *env){
  return NULL;
}

Value *evalLetStar(Value *args, Environment *env){
 Value *toCheck;
  
  int count = listLength(args);
  if (count < 2){
    printf("let*: bad syntax in: (letrec ");
    printValue(args);
    printf("\n");
    return NULL;
  }
   if (getFirst(args)->type== nullType){
    while (getTail(getTail(getTail(args)))){
	eval(getTail(args),env);
	args = getTail(args);
    }
    return eval(getTail(args), env);
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
    
    removeLast(toCheck);
    
    if (getFirst(getTail(getFirst(toCheck))) -> type !=symbolType){
      printf("bad syntax in let*: not an indentifier\n");
      return NULL;
    }else{
      Environment* newEnv = createFrame(env);
      Value* listofBinds = toCheck;
      
      while (listofBinds){
	insertItem(newEnv->bindings->tableValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue, NULL);
	listofBinds = getTail(listofBinds);
      }
     
      listofBinds = toCheck;
      while (listofBinds){
	
	removeLast(getTail(getFirst(listofBinds)));
	
	Value* toBind = eval(getTail(getTail(getFirst(listofBinds))), newEnv);
	
	if (toBind){
	  if (toBind->type == cellType){
	    if (getTail(toBind)){
	      printf("syntax error in let*: too many values for single identifier.\n");
	    }else{
	      toBind = getFirst(toBind);
	    }
	  }
	  insertItem(newEnv->bindings->tableValue, getFirst(getTail(getFirst(listofBinds)))->symbolValue, toBind);
	  
	}else{ 
	  printf("syntax error in let*\n");
	  return NULL;
	}
		
	listofBinds = getTail(listofBinds);

      }
      Value* listofExpressions = getTail(args);
      
      removeLast(listofExpressions);
      
      if (!listofExpressions){
	printf(" let*: bad syntax.\n ");
	return NULL;
      }
      while(listofExpressions){
	Value* toReturn = eval(getFirst(listofExpressions), newEnv);

	if(typeCheck(toReturn) < 3 && typeCheck(toReturn) > 0 ){
	  if(getTail(listofExpressions)){
	    
	    listofExpressions = getTail(listofExpressions);
	  
	  }else{
	    return toReturn;
	  }
	}else{
	  listofExpressions = getTail(listofExpressions);
	}	
      }
      printf(" let*: bad syntax\n");

      return NULL;
    }
  }
  return NULL;
}

Value *consFunction(Value *args){
  return NULL;
}

Value *lessOrEqualThan(Value *args){
  return NULL;
}

Value *bigOrEqualThan(Value *args){
  return NULL;
}

Value *arithmeticEqual(Value *args){
  return NULL;
}

Value *checkEqual(Value *args){
  return NULL;
}

// not finished yet.
Value *exponentiate(Value *args){
  return NULL;
}
