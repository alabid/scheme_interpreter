/* By Jie Lin, Kan Wang, Daniel Alabi, Yichen Shen.
   CS 251, Fall 2011
*/

enum TOKEN_TYPE{
  cellType, booleanType, integerType, floatType, stringType, symbolType, openType, closeType, closureType, envType, funType, bindingType
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
    struct __Closure *closureValue;
    struct __Value* (*primitiveValue)(struct __Value*);
    struct __Environment* envValue;
    struct __Binding* bindingValue;
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
List* initializeList();

// This function inserts a new cons cell to the linked list.
int insertCell(List *list, Value *value);

int push(List *list, Value *value);

// This function pops the head of the linked list.
Value* pop(List *list);

// This function reverses the linked list.
int reverse(List *list);

// This function frees cons cells of the linked list.
void cleanup(Value* head);

// This function prints the linked list.
void printToken(Value* value);

// This function frees its cons cells and also frees the list.
void destroy(List* list);

typedef struct __Environment{
  Value* bindings;
  struct __Environment* parent;
}Environment;

typedef struct __Binding{
  char* name;
  Value* value;
}Binding;

typedef struct __Closure{
  Value* parseTree;
  Value* paramList;
  Environment* parent;
}Closure;

typedef struct __HashTable{
  struct __ConsCell* entries;
  int size; // actual data size
  int capacity; //the maximum size
} HashTable;

HashTable* initializeTable(int size);

int hash(HashTable* table, char* id);

int insertItem(HashTable* table, char* id, Value* value);

int autoDouble(HashTable* table);

ConsCell* lookupEntry(HashTable* table, char* id);

Value* lookup(HashTable* table, char* id);

Value* deleteItem(HashTable* table, char* id);

void cleanupTable(HashTable* table);

void printTable(HashTable* table);
