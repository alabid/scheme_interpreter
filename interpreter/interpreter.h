# include <stdio.h>
# include "parser.h"

Value *eval(Value *expr, Environment *env);

Value *apply(Value *function, Value *actualArgs, Environment *env);

Value *envLookup(char *id, Environment *env);

Value *evalDefine(Value *args, Environment *env);

Value* evalQuote(Value* args);

Value* evalLetrec(Value* args, Environment* env);

Value* evalLet(Value* args, Environment* env);

Value* evalIf(Value* args, Environment* env);

Value* evalLambda(Value* args, Environment* env);

Value *evalEach(Value *args, Environment *env);

Value *makePrimitiveValue(Value *(*f)(Value *, Environment *));

Environment *createTopFrame();

void freeTopFrame(Environment *env);

Environment *createFrame(Environment* parent);

Environment *createFrameWithSize(Environment *parent, int size);

Environment *searchTopLevel(Environment* current);

Value *exponentiate(Value *args, Environment *env);

Value *add(Value *args, Environment *env);

Value *loadFunction(Value *args, Environment *env);

int typeCheck(Value* value);

void bind(char identifier[], Value *function, Environment *env);

int validateArgs(Value *value, Environment *env);

Value *subtract(Value *args, Environment *env);

Value *multiply(Value *args, Environment *env);

Value *divide(Value *args, Environment *env);

int loadFromFile(FILE *file, Environment *env);

Value *evalAnd(Value *args, Environment *env);

Value *evalCond(Value *args, Environment *env);

Value *evalOr(Value *args, Environment *env);

Value *evalSetBang(Value *args, Environment *env);

Value *evalLetStar(Value *args, Environment *env);



Value *smallerOrEqualTo(Value *args, Environment *env);

Value *greaterOrEqualTo(Value *args, Environment *env);

Value *arithmeticEqual(Value *args, Environment *env);

int checkEqual(Value *first, Value *second, Environment *env);

Value *equality(Value *args, Environment *env);

Environment* insertEnv2Top(Environment* toInsert);

Environment* insertEnv(Environment* toInsert, Environment *parent);

char* intToString(int number);

Value* checkNull(Value *value, Environment *env);
