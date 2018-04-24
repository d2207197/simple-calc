#ifndef CSTACK_H
#define CSTACK_H


typedef struct  {
  unsigned int capacity;
  char* top;
  char* stack;
} cstack;

void cstack_init ( cstack* stack, unsigned capacity );
void cstack_push ( cstack* stack, char item );
char cstack_pop ( cstack* stack );
char cstack_top ( const cstack* stack );
void cstack_print ( const cstack* stack );
int cstack_size ( const cstack* stack );

#endif 
