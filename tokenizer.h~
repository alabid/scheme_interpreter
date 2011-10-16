#define MAX 256;

enum TOKEN_TYPE{
  cellType, booleanType, integerType, floatType, stringType, symbolType, openType, closeType, procedureType
};

typedef struct __Value{
  int type;
  union {
    int boolValue;
    int intValue;
    double dblValue;
    char* stringValue;
    char* procedureValue;
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

typedef struct __LinkedList{
  struct __Value *head;
} List;

// This function initializes a linked list. It will assign head as NULL.
void initialize(List *list);

// This function inserts a new cons cell to the linked list.
int insertCell(List *list, Value *value);

// This function deletes a cons cell from the linked list.
int deleteCell(List *list, Value *value);

// This function reverses the linked list.
int reverse(List *list);

// This function frees cons cells of the linked list.
void cleanup(List* list);

// This function prints the linked list.
void printToken(List* list);

List* tokenize(char* expression);

/* This function creates a new struct __Value and
   copys the payload stored in the Value value. 
   If value's payload contains a pointer, it only performs shallow copy.*/
Value* copyValue(Value *value);

// This function frees its cons cells and also frees the list.
void destroy(List* list)
