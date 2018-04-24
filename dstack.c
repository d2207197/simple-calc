#include "dstack.h"
#include <stdlib.h>
#include <stdio.h>

void dstack_init ( dstack* stack, unsigned capacity )
{
  stack->stack = stack->top = malloc ( capacity * sizeof( double) );
  stack->capacity = capacity;
}

void dstack_push ( dstack* stack, double item )
{
  if ( ( stack->top - stack->stack )  >= stack->capacity ) {
    fprintf ( stderr, "stack is full\n" );
    abort();
  }
    
  *(stack->top) = item ;
  stack->top++;
}

double dstack_pop ( dstack* stack )
{
  return *(--(stack->top));
}

double dstack_top ( const dstack* stack )
{
  return *(stack->top-1);
}

void dstack_print ( const dstack* stack )
{
  double *cur = stack->top -1;
  while ( cur >= stack->stack )
    printf ("%g : ", *(cur--) );
}

int dstack_size ( const dstack* stack )
{
  return stack->top - stack->stack;
}
