# include <stdio.h>
# include "parser.h"

/*
eval the expressions
 */
Value *eval(Value *expr, Environment *env);

//apply the function to arguments
Value *apply(Value *function, Value *actualArgs, Environment *env);
//look up the identifier in the envirnment
Value *envLookup(char *id, Environment *env);
// defines a variable
Value *evalDefine(Value *args, Environment *env);
// evaluates a quote
Value* evalQuote(Value* args);
// letrec uses this function
Value* evalLetrec(Value* args, Environment* env);
// eval's let
Value* evalLet(Value* args, Environment* env);
// evaluates an if statement
Value* evalIf(Value* args, Environment* env);
// evaluates the special form lambda
Value* evalLambda(Value* args, Environment* env);
// evaluates each part of an expression
Value *evalEach(Value *args, Environment *env);
// makes a primitive and converts a C function to a value
Value *makePrimitiveValue(Value *(*f)(Value *, Environment *));
// creates the top-level environment and binds
// primitives
Environment *createTopFrame();
// frees the top-level frame
void freeTopFrame(Environment *env);
// creates a new environment
Environment *createFrame(Environment* parent);
// creates a new environment and specifies the size
// of the hashtable to be used
Environment *createFrameWithSize(Environment *parent, int size);
// searches the top level environment
Environment *searchTopLevel(Environment* current);

// Value *exponentiate(Value *args, Environment *env);

//  adds numbers
Value *add(Value *args, Environment *env);
// loads a files for the interpreter to use
Value *loadFunction(Value *args, Environment *env);
// checks types for the interpreter
int typeCheck(Value* value);
// binds an id to a function
void bind(char identifier[], Value *function, Environment *env);

// validate arguments
int validateArgs(Value *value, Environment *env);

// subtracts
Value *subtract(Value *args, Environment *env);
// multiplies
Value *multiply(Value *args, Environment *env);
// divides
Value *divide(Value *args, Environment *env);
// loads a file into the interpreter
// file must be an open FILE
int loadFromFile(FILE *file, Environment *env);
// evaluates an "and" expression
Value *evalAnd(Value *args, Environment *env);
// evaluates a "cond" special form
Value *evalCond(Value *args, Environment *env);
// evaluates a "or"
Value *evalOr(Value *args, Environment *env);
// evaluates a set!
Value *evalSetBang(Value *args, Environment *env);
// evaluates a let*
Value *evalLetStar(Value *args, Environment *env);



Value *smallerOrEqualTo(Value *args, Environment *env);

Value *smallerThan(Value *args, Environment *env);

Value *greaterThan(Value *args, Environment *env);

Value *greaterOrEqualTo(Value *args, Environment *env);

Value *arithmeticEqual(Value *args, Environment *env);

int checkEqual(Value *first, Value *second, Environment *env);

Value *equality(Value *args, Environment *env);

Environment* insertEnv2Top(Environment* toInsert);

Environment* insertEnv(Environment* toInsert, Environment *parent);

char* intToString(int number);

Value* checkNull(Value *value, Environment *env);

