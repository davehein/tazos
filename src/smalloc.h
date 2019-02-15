#define HDR_SIZE 8

#define STR_ADD(strstruct, offset) ((StringT *)(((long)strstruct) + offset))
  
typedef struct StringS {
  struct StringS * next;
  int size;
  char str[1];
  } StringT;

StringT *smalloc(int size);
void sfree(StringT *curr);
void smallocinit(char *ptr, int size);
