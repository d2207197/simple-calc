#include "cstack.h"
#include <stdlib.h>
#include <stdio.h>

void cstack_init ( cstack* stack, unsigned capacity )
{
  stack->stack = stack->top = malloc ( capacity * sizeof( char) );
  stack->capacity = capacity;
}

void cstack_push ( cstack* stack, char item )
{
  if ( ( stack->top - stack->stack )  >= stack->capacity ) {
    fprintf ( stderr, "stack is full\n" );
    abort();
  }
    
  *(stack->top) = item ;
  stack->top++;
}

char cstack_pop ( cstack* stack )
{
  return *(--stack->top);
}

char cstack_top ( const cstack* stack )
{
  return *(stack->top-1);
}

void cstack_print ( const cstack* stack )
{
  char *cur = stack->top -1;
  while ( cur >= stack->stack )
    printf ("%c : ", *(cur--) );
}

int cstack_size ( const cstack* stack )
{
  return stack->top - stack->stack;
}
