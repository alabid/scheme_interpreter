enum TOKEN_TYPE{
  cellType, booleanType, integerType, floatType, stringType, symbolType, openType, closeType
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
  } payload;
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

// This function frees the linked list and its cons cells.
void cleanup(List* list);

// This function prints the linked list.
void print(List* list);

// This function tokenizes a line of scheme code.
