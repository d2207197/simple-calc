
#include "vars.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void vars_init ( Vars *pvs, unsigned capacity )
{
  pvs->capacity = capacity;
  pvs->size = 0;
  pvs->vars = calloc ( capacity, sizeof( Var ) );
}

void vars_add ( Vars *pvs, const char * var_name, VarType var_type )
{
  unsigned size = pvs->size;
  pvs->vars[size].name = malloc ( strlen ( var_name )+1 );
  strcpy ( pvs->vars[size].name, var_name );

  pvs->vars[size].type = var_type;
  pvs->size++;
}


Var* vars_find ( Vars *pvs, const char * var_name, int len )
{
  int i;
  unsigned size;

  if ( pvs == NULL )
    return NULL;

  size = pvs->size;
  if ( len  == 0 )
    len = strlen ( var_name );

  for ( i=0; i<size; i++ )
    {
      if ( strncmp ( pvs->vars[i].name, var_name, len ) == 0 )
	  return &pvs->vars[i];
    }
  return NULL;
}

