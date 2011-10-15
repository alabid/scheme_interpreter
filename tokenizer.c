#include "tokenizer.h"

enum TOKEN_TYPE{
  booleanType, integerType, floatType, stringType, symbolType, openType, closeType
};

typedef struct __Value{
  int type;
  union {
    int boolValue;
    int intValue;
    double dblValue;
    char* stringValue;
    char* symbolValue;
    char open;
    char close;
    struct __ConsCell *cons;
  };
} Value;

typedef struct __ConsCell{
  struct __Value *car;
  struct __Value *cdr;
} ConsCell;
