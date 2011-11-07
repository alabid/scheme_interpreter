# include <stdio.h>
# include "parser.h"

Value *eval(Value *expr, Environment *env);

Value *apply(Value *function, Value *actualArgs);

Value *envLookup(char *id, Environment *env);

Value *evalDefine(Value *args, Environment *env);

Value* evalQuote(Value* args);

Value* evalLetrec(Value* args, Environment* env);

Value* evalLet(Value* args, Environment* env);

Value* evalIf(Value* args, Environment* env);

Value* evalLambda(Value* args, Environment* env);

Value *evalEach(Value *args, Environment *env);

Value *makePrimitiveValue(Value *(*f)(Value *));

Environment *createTopFrame();

Environment *createFrame(Environment* parent);

Value *exponentiate(Value *args);

Value *add(Value *args);

Value *loadFunction(Value *args, Environment *env);

int typeCheck(Value* value);

void bind(char identifier[], Value *function, Environment *env);

int validateArgs(Value *value, Environment *env);

Value *subtract(Value *args);

Value *multiply(Value *args);

Value *divide(Value *args);

int loadFromFile(FILE *file, Environment *env);

Value *evalAnd(Value *args, Environment *env);

Value *evalCond(Value *args, Environment *env);

Value *evalOr(Value *args, Environment *env);

Value *evalSetShirek(Value *args, Environment *env);

Value *evalLetStar(Value *args, Environment *env);

Value *consFunction(Value *args);

Value *lessOrEqualThan(Value *args);

Value *bigOrEqualThan(Value *args);

Value *arithmeticEqual(Value *args);

Value *checkEqual(Value *args);
