#ifndef DSTACK_H
#define DSTACK_H


typedef struct  {
  unsigned int capacity;
  double* top;
  double* stack;
} dstack;

void dstack_init ( dstack* stack, unsigned capacity );
void dstack_push ( dstack* stack, double item );
double dstack_pop ( dstack* stack );
double dstack_top ( const dstack* stack );
void dstack_print ( const dstack* stack );
int dstack_size ( const dstack* stack );

#endif 
