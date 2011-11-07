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
	//printf("going here\n");
	//printValue(expr);
	//printf("\nending here\n");
	return args;
      }
      else{
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

	if (!operator){
	  printf("syntax error, missing components here");
	  return NULL;
	}
	if (operator->type == cellType){
	  operator = eval(operator, env);
	}
	
	if (operator->type == symbolType){
	  args = getTail(getTail(expr));
	  //printf("checking args?: ");
	  //printValue(args);
	  //printf("\n"); //if (args  == NULL){
	  //return eval(operator,env); //	  }else 
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
	  }else{
	    //printf("validation result is shown: %d\n",validateArgs(args, env));
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
	    //printValue(evaledArgs);
	    //printf("\n");
	    return apply(evaledOperator, evaledArgs);
	  }
	}else if (typeCheck(operator)==1){
	  printf("A literal ");
	  printValue(operator);
	  printf(" cannot be a procedure.\n");
	  return NULL; 
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
	     
	    printf("Unknown identifier %s.\n",operator->symbolValue);
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
	//assert(1==2);
      }
      return expr;      
    }
}

int validateArgs(Value *value, Environment* env){
 
  if (value && value->type==cellType){
  
    Value *toCheck = getFirst(value);
    if (toCheck){
      
      //printf("I'm here to validate args\n\n\n\n");
      switch (toCheck->type)
	{
	case cellType:
	case openType:
	  return 2;
	  break;
	case symbolType:
	  toCheck = eval(toCheck,env);
	  //printf("validation intermediate: ");
	  //printValue(toCheck);
	  //printf("\n");
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
      insertItem(frame->bindings->tableValue, getFirst(curArg)->symbolValue, getFirst(curValue));
      curArg = getTail(curArg);
      curValue = getTail(curValue);
    }
    if (curArg || curValue){
      printf("Wrong number of parameters for the procedure.\n");
      // destroyFrame(frame);
      return NULL;
    }
    
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
      case primitiveType:
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
  Value *head = args, *openParen;
  int i=0;
  //printf("before eval each: ");
  //printValue(args);
  //printf("\n");
  while (args){ 
    i++;
    assert(args->type==cellType);    
    //temp = args->cons->car;
    if (getFirst(args) && (getFirst(args))->type==cellType){
      tail = getTail(args);
      
      openParen = getFirst(getFirst(args));
  
      if (openParen && openParen->type == openType){

	temp =  deepCopy(eval(getFirst(args), env));

	printf("printing temp:");
	printValue(temp);
	printf("\n");
	//assert(getFirst(getFirst(args))!=NULL);
	//assert(getFirst(getFirst(args))->type==openType);
	
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
  /*
  printf("before going back ");
  assert(head!=NULL);
  assert(head->type==cellType);
  assert(head->cons!=NULL);
  assert(head->cons->car!=NULL);
  printf("this position is: ");
  printValue(head->cons->car);
  printf("\n");
 
  printValue(head);

  printf("\n");
  */
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
      Environment* newenv = creatFrame(env);
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
	  if(getTail(listofExpression)){
	    listofExpressions = getTail(listofExpressions);
	  }else{
	    return toReturn;
	  }
	}else{
	  
	  
	
      }
      
  
    
	  
    
    
}

// This function evaluates if statement.
Value* evalIf(Value* args, Environment* env){
  // args = evalEach(args,env);
  
  int count = listLength(args);
  //printf("count = %d \n",count);
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
  // Value *tempArgs;

  // printf("result: %d", evalTest->type);
 
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
  printf("start printing for lambda: ");
  printValue(toCheck);
  printf("\n");
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
    removeLast(toCheck);
    printf("Show me the parse tree: ");
    printValue(toCheck);
    printf("\n");
    closure->body = toCheck;
    returnValue->closureValue = closure;
    return returnValue; 
  }else if (toCheck && toCheck->type==nullType){
    Value *returnValue = (Value *)malloc(sizeof(Value));
    returnValue->type = closureType;
    Closure *closure = initializeClosure(env);
    toCheck = getTail(args);
    removeLast(toCheck);
    closure->body = toCheck;
    returnValue->closureValue = closure;
    printf("Show me the parse tree: ");
    printValue(toCheck);
    printf("\n");
    return returnValue; 
  }else{
    //printValue();
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
  // bind("/", makePrimitiveValue(divide), frame);
  // bind("exp", makePrimitiveValue(exponentiate), frame);
  bind("load", makePrimitiveValue(loadFunction), frame);
  bind("car", makePrimitiveValue(car), frame);
  bind("cdr", makePrimitiveValue(cdr), frame);
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
    
    printf("this is the head before adding: ");
    printValue(head);
    printf("\n");
    cleanup(getTail(head));
  }
  Value *value = (Value*) malloc(sizeof(Value));
  //result = getFirst(head);
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
    printf("this is the head after adding: ");
    printValue(head);
    printf("\n");
  }
  return value;
}




Value *subtract(Value *args){
  int intDiff = 0;
  double dblDiff = 0;
  int isFloat = 0;
  Value  *head = args;
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
  }else{
      printf("Invalid input for <#procedure:->\n");
      return NULL;
  }
 
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
    
    printf("this is the head before subing: ");
    printValue(head);
    printf("\n");
    cleanup(getTail(head));
  }
  Value *value = (Value*) malloc(sizeof(Value));
  //result = getFirst(head);
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
    printf("this is the head after adding: ");
    printValue(head);
    printf("\n");
  }
  return value;
}

Value *multiply(Value *args){
  int intProd = 1;
  double dblProd = 1;
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
	  dblProd *= getFirst(args)->dblValue;
	}
      }else if (getFirst(args)->type == floatType){
	if (!isFloat){
	  isFloat = 1;
	  dblProd *=intProd;
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
    
    printf("this is the head before adding: ");
    printValue(head);
    printf("\n");
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
    printf("this is the head after multiplying: ");
    printValue(head);
    printf("\n");
  }
  return value;
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

void bind(char identifier[], Value *function, Environment *env){
  if (!env || !function){
    return;
  }
  char *variable = (char *)malloc(sizeof(char)*(strlen(identifier)+1));
  //printf("the length of variable is %zd.\n",strlen(identifier)+1);
  strcpy(variable, identifier);
 
  variable[strlen(identifier)]='\0';
  
  //printf("the variable is %s.\n",variable);
  insertItem(env->bindings->tableValue, variable, function);
  //printf("checking the variable:  ");
  //printValue(lookup(env->bindings->tableValue,identifier));
  //printf("\n");
}
