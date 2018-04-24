#include <stdio.h>
#include "vars.h"
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include "dstack.h"
#include "cstack.h"

void parse_error ( const char* exp, const char* p_ch, char* msg )
{
  const char *p_tmp = p_ch;
  
  fprintf (stderr, "Parse Error: \n    %s    ", exp);
  while( p_tmp-- != exp )
    fprintf ( stderr, " "); 
  fprintf ( stderr, "^ %s\n", msg);
}

/* Operator 的優先權判定 */
int op_priority ( char op )
{
  switch ( op )
    {
      /* Unary operator 優先權最高 */
    case 's': case 'c': case 'l': case 'e':
      return 4;

      /* Binary operator 次之 */
    case '/':
      return 3;
    case '*':
      return 2;
    case '+': case '-':
      return 1;

      /* 右括號優先權應大於左括號 */
    case ')':
      return 0;
    case '(':
      return -1;

      /* 意料之外的 op(不會發生，除非程式寫錯) */
    default:
      fprintf (stderr, "unknown op '%c'\n", op );
      abort();

    }
}

/* 計算 operand stack 與 operator stack 裡的算式 */
void partial_eval ( dstack* ps_operand, cstack* ps_operator, char cur_op )
{
  double result;

  while ( cstack_size (ps_operator) != 0 && 
	  /* 給 '\0' 代表把整個 stack 都計算掉 */
	  ( cur_op == '\0' || 
	    /* 計算到優先權低於目前的 op 為止
	       意即 cur_op 是 '*' 的話，僅會處理優先權大於等於 '*' 的 op 如 '*', '/', 's' ... */
	    op_priority (cstack_top (ps_operator)) >= op_priority (cur_op))) 
    {
      switch ( cstack_pop (ps_operator))
	{
	case '+':
	  result = dstack_pop (ps_operand) + dstack_pop (ps_operand);
	  break;
	case '-':
	  result = dstack_pop (ps_operand);
	  result = dstack_pop (ps_operand) - result;
	  break;
	case '*':
	  result = dstack_pop (ps_operand) * dstack_pop (ps_operand);
	  break;
	case '/':
	  result = dstack_pop (ps_operand);
	  result = dstack_pop (ps_operand) / result;
	  break;
	case 's':
	  result = sin ( dstack_pop (ps_operand) );
	  break;
	case 'c':
	  result = cos ( dstack_pop (ps_operand) );
	  break;
	case 'l':
	  result = log ( dstack_pop (ps_operand) );
	  break;
	case 'e':
	  result = exp ( dstack_pop (ps_operand) );
	  break;
	}
      /* 計算結果丟回 operand 的 stack */
      dstack_push (ps_operand, result);
    }
}

typedef struct {
  char* name;
  size_t len;
  char c;
} funct_entry;

/* 查詢 function 列表 */
funct_entry* in_funct_table ( const char* s )
{	 
  int i;
  static funct_entry funct_list[4] = { { "sin", 3, 's' },
				       { "cos", 3, 'c' },
				       { "log", 3, 'l' },
				       { "exp", 3, 'e' }};

  for ( i = 0; i < 4; i++ )
    if ( strncmp ( s, funct_list[i].name,  funct_list[i].len ) == 0 )
      {
	s+= funct_list[i].len;
	if ( !isalpha ( *s ) && ! ( *s == '_' ) ) /* 確定後面不是字母或'_' */
	  return &(funct_list[i]);
      }
  return NULL;
}

/* 查詢 variables 列表 */
Var* in_vars_table ( Vars* pvs, const char* s )
{
  const char * cur = s;
  while ( isalpha ( *cur ) || isdigit ( *cur ) || *cur == '_' )
    cur++;
  return vars_find ( pvs, s, cur - s );
}

/* 判斷 c 是否屬於 s 中的一種 */
int chr_in_str ( char c, const char* s )
{
  for  ( ; *s ; s++ )
    if ( c == *s )
      return 1;

  return 0;
}

/* 回傳第一個不是空白的字元的位置 */
const char * eat_spaces ( const char* s )
{
  while ( *s != '\0' && isspace (*s ) )
    s++;
  return s;
}

/* 計算式子 */
int calc_expression ( const char* exp, double *result, Vars* pvs )
{
  const char* p_ch = exp;
  int sign = 1;
  dstack s_operand;		/* 存放 operand 的 stack */
  cstack s_operator;		/* 存放 operator 的 stack */
  funct_entry *fe;
  int exp_level = 0;
  Var *pv;

  /* 共 4 個 state (Finite State Machine 流程) */
  enum { UNARY_OP, OPERAND, RIGHT_PAREN, BINARY_OP } state = UNARY_OP;

  /* 初始化 Stacks */
  dstack_init ( &s_operand, 32 );
  cstack_init ( &s_operator, 32 );

  /* 開始計算式子，直到 '\0' 出現 */
  while ( *p_ch != '\0' )
    {
      p_ch = eat_spaces ( p_ch ); /* 吃掉空白 */
      switch ( state )
	{
	case UNARY_OP:
	  /******************/
	  /* UNARY_OP state */
	  /******************/
	  if ( *p_ch == '+' )	/* 正負號 */
	    {
	      p_ch += 1;
	      state = UNARY_OP;
	    } 
	  else if ( *p_ch == '-' ) /* 正負號 */
	    {
	      sign *= -1;
	      p_ch += 1;
	      state = UNARY_OP;
	    } 
	  else if ( ( fe = in_funct_table ( p_ch ) ) ) /* Functions */
	    {
	      p_ch += fe->len;
	      cstack_push (&s_operator, fe->c );
	      state = UNARY_OP;
	    }
	  else 			/* 無 Unary 的 Operator */
	    state = OPERAND;
	  break;

	case OPERAND:
	  /*****************/
	  /* OPERAND state */
	  /*****************/
	  if ( *p_ch == '(' )	/* Left Parenthesis */
	    {
	      cstack_push ( &s_operator, '(' );
	      exp_level++;
	      p_ch++;
	      state = UNARY_OP;
	    }
	  else if ( isdigit (*p_ch) )	/* Digits */
	    {
	      dstack_push ( &s_operand, strtod ( p_ch, (char**) &p_ch ) * sign );
	      sign = 1;
	      state = RIGHT_PAREN;
	    }
	  else if (  ( pv = in_vars_table ( pvs, p_ch ) ) ) /* Variables */
	    {
	      dstack_push ( &s_operand, pv->value*sign );
	      sign = 1;
	      p_ch += strlen ( pv->name );
	      state = RIGHT_PAREN;
	    }
	  else 			/* Error */
	    {
	      parse_error (exp, p_ch, "Expect an operand" );
	      return 0;
	    }
	  break;

	case RIGHT_PAREN:
	  /*********************/
	  /* RIGHT PARENTHESIS */
	  /*********************/
	  if ( *p_ch ==  ')' ) 	/* Right Parenthesis */
	    {
	      partial_eval ( &s_operand, &s_operator, *p_ch );
	      cstack_pop ( &s_operator ); /* 拿掉 '(' */
	      exp_level--;
	      p_ch++;
	    }
	  state = BINARY_OP;
	  break;

	case BINARY_OP:
	  /*******************/
	  /* BINARY_OPERATER */
	  /*******************/
	  switch ( *p_ch )
	    {
	    case '+': case '-':	/* 加減乘除 */
	    case '*': case '/':
	      if (cstack_size (&s_operator) != 0 && 
		  op_priority (*p_ch) < op_priority (cstack_top (&s_operator)))
		partial_eval ( &s_operand, &s_operator, *p_ch );

	      cstack_push ( &s_operator, *p_ch );
	      p_ch++;
	      state = UNARY_OP;
	      break;

	    case '\0':		/* 結束字元，跳過 */
	      break;

	    default:		/* 預料之外的符號 */
	      parse_error ( exp, p_ch , "Invalid Syntax");
	      return 0;
	    }
	}
    }
      
  if ( exp_level != 0  )	/* 左右括號沒有對應 */
    {
      parse_error ( exp, p_ch -1 , "Unexpect end of expression" );
      return 0;
    }

  if ( state == OPERAND )	/* 不應在期待一個 operand 時結束 */
    {
      parse_error (exp, p_ch -1, "Expect an operand" );
      return 0;
    }
    
  partial_eval ( &s_operand, &s_operator, *p_ch );
  *result = dstack_top (&s_operand );
  return 1;
}

char *vartok (char *str );

int main(int argc, char** argv)
{
  char buf[128];
  double result;
  if ( argc == 1 ) 
    {
      printf (">>> " );
      while (fgets ( buf, 128, stdin ) != NULL )
	{
	  if ( calc_expression ( buf, &result, NULL ) != 0 )
	    printf ( " = %g\n", result );
	  else 
	    fprintf ( stderr, "\nTry again.\n" );
	  printf (">>> ");
	}
    }
  else if ( argc == 2 )
    {
      Vars vars;
      FILE *fp;
      int idx;

      vars_init ( &vars, 10 );
      fp = fopen ( argv[1], "r" ); 

      while ( fgets ( buf, 128, fp ) != NULL )
	{
	  char *s = strspn (buf, " \t") + buf;
	  
	  if( strncmp ( s, "int", 3 ) == 0)
	    {
	      s = vartok ( s+3 );
	      while ( s != NULL ) { 
		vars_add ( &vars, s, itype );
		s = vartok ( NULL );
	      }
	    }
	  else if ( strncmp ( s, "float", 5 ) == 0 )
	    {
	      s = vartok ( s+5 );
	      while ( s != NULL ) { 
		vars_add ( &vars, s, dtype );
		s = vartok ( NULL );
	      }
	    }
	  else
	    {
	      char *exp = strchr ( s, '=' ) +1 ;
	      Var *pv;
	      if ( exp == NULL )
		abort();
	      *strchr ( exp, ';' ) = '\0';

	      *strpbrk ( s, " =\t" ) = '\0';

	      pv = vars_find ( &vars, s, 0);
	      if ( calc_expression ( exp, &result, & vars ) == 0 )
		abort();
		
	      if ( pv->type == itype )
		pv->value =  floor ( result );
	      else if ( pv->type == dtype )
		pv->value =  result;
	      else if ( pv == NULL )
		abort();

	    }
	}
      for ( idx = 0; idx < vars.size; idx++ )
	{
	  if ( vars.vars[idx].type == itype ) 
	    printf ( "%s = %d (int)\n",  vars.vars[idx].name, (int) vars.vars[idx].value );
	  else if ( vars.vars[idx].type == dtype ) 
	    printf ( "%s = %g (float)\n",  vars.vars[idx].name, vars.vars[idx].value );
	} 
    }
  
  return 0;
  
}


static char* oldstr;

char *vartok (char *str )
{
  char *token;

  if ( str == NULL )
    str = oldstr;
  
  str += strspn (str, " \t");
  if (*str == '\0')
    {
      oldstr = str;
      return NULL;
    }

  token = str;
  str = strpbrk (token, ";,");
  if ( str == NULL )
    /* This token finishes the string.  */
    abort();
  else if ( *str == ';' )
    {
      oldstr = strpbrk ( token, "; \t");
      *oldstr = '\0';
    }
  else
    {
      *strpbrk ( token, ", \t") = '\0';
      oldstr = str + 1 ;
    }
  return token;
}

