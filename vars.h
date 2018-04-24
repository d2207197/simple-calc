#ifndef VARS_H
#define VARS_H

typedef enum  { dtype, itype } VarType;

typedef struct {
  char* name;
  VarType type;
  double value;
} Var;


typedef struct  {
  Var * vars;
  unsigned int size;
  unsigned int capacity;
} Vars;


void vars_init ( Vars *pvs, unsigned capacity );
void vars_add ( Vars *pvs, const char * var_name, VarType var_type );
Var* vars_find ( Vars *pvs, const char * var_name, int len );

#endif
 
