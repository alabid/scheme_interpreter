#include "parser.h"

Value *eval(Value *parseTree, Environment *env);

Value *apply(Value *function, Value *actualArgs);

Value *envLookup(char *id, Environment *env);

Value *evalDefine(Value *args, Environment *env);


Value* evalLet(Value* args, Environment* env);

Value* evalIf(Value* args, Environment* env);

Value* evalLambda(Value* args, Environment* env);

Value *evalEach(Value *args, Environment *env);

Value *makePrimitiveValue(Value *(*f)(Value *));

Environment *createTopFrame();

Environment *createFrame(Environment* parent);

Value *exponentiate(Value *args);

Value *add(Value *args);

Value *loadFunction(Value *args);


