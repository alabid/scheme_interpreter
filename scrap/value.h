/* By Jie Lin, Kan Wang, Daniel Alabi, Yichen Shen.
   CS 251, Fall 2011
*/

enum VALUE_TYPE{
  nullType, tableType, cellType, listType, booleanType, integerType, floatType, stringType, symbolType, openType, closeType, closureType, envType, primitiveType
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
    struct __HashTable *tableValue;
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

// This function prints the each token.
void printToken(Value* value);

// This function prints the linked list.
void printTokens(Value* value);

// This function frees its cons cells and also frees the list.
void destroy(List* list);

typedef struct __Environment{
  Value* bindings;
  struct __Environment* parent;
}Environment;


typedef struct __Closure{
  Value* body;
  List* args; 
  Environment* parent;
}Closure;


// initialize a closure by mallocating memory for body, args.
// args is a "linked list" so this function also mallocates hash table.
Closure *initializeClosure(Environment* env);

// cleanup frees the contents of the closure, but does not free closure itself.
void cleanupClosure(Closure *closure);

// destroy frees the contents of the closure and closure itself.
void destroyClosure(Closure *closure);

typedef struct __HashTable{
  struct __ConsCell* entries;
  int size; // actual data size
  int capacity; //the maximum size
}HashTable;

// The following are functions for hash table.
// Note, every parameter except atmoic type (int, float, one character) must be mallocated in the heap.
/* 
   This function initializes one hash table with size = capacity
   If capacity is less than or equal to zero, NULL is returned.
*/
HashTable* initializeTable(int size);

/*
  Hash function that uses open addressing.
  No duplicate keys are allowed.
*/
int hash(HashTable* table, char* id);

/*
  Insert key = id, payload = value into the hash table.
  Auto doubling the hash table if the size of the hash table reaches 2/3 of the capacity, 
  where 2/3 is the load factor.
*/
int insertItem(HashTable* table, char* id, Value* value);

/*
  Auto doubling the hash table if the size of the hash table reaches 2/3 of the capacity, 
  where 2/3 is the load factor.
*/
int autoDouble(HashTable* table);

/*
  Return the entry with key = id
*/
ConsCell* lookupEntry(HashTable* table, char* id);

/*
  Return the payload with key = id
*/
Value* lookup(HashTable* table, char* id);

/*
  Delete an item with key = id.
  This function does not free the payload since we did not malloc memory 
  inside the insertItem function for payload.
*/
int deleteItem(HashTable* table, char* id);

// cleanupTable only cleans the cons cells and keys.
void cleanupTable(HashTable* table);

// cleanupEnvironment
void cleanupEnvironment(Environment *env);
/* 
   Destroy the table by freeing the cons cells, keys and hash table itself.
   This function cannot free any payloads.
*/
void destroyTable(HashTable *table);

/*
  Print the table for debugging purposes.
*/
void printTable(HashTable* table);

/*
  returns the car of the List
*/
Value *car (Value *value);

/*
  returns the cdr of the List
*/
Value *cdr (Value *value);

void cleanupValue(Value* value);

/*
  This function is used to print the result after evaluation.
*/
void printValue(Value* head);

/*
give me the length of the list
*/
int listLength(Value *value);

/*
  Returns all identifiers in the bindings as a linked list.
*/
List* getKeys(HashTable* table);
/*
  Returns all payloads in the hash table as a linked list.
*/
List* getValues(HashTable* table);

/*
  This function accepts a Value that is the head of the list, and prints out the list.
*/
void printList(Value* value);

Value *cdrFree(Value *value, int freeCar);

int listLength(Value *value);

int properListLength(Value *value);

Value *getFirst(Value *value);

Value *getTail(Value *value);

Value *deepCopy(Value *value);

Value *deepCopyList(Value *value);

void freeValue(Value *value);

Value* deepCopyFun(Value *function);

void removeLast(Value* value);

//void printArgs(Value *curValue, int withQuotes);

