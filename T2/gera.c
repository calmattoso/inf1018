/* Carlos Augusto Lima Mattoso 1210553 3WA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gera.h"

/*****************************************************************************                                                                            
*   Constant Definitions                                                     *
******************************************************************************/

#define true  1
#define false 0

#define MAX_SB_TRANS 3139 /* largest possible size for translated SB      */
#define MC_TABLE_LEN  17  /* number of machine code instructions          */

#define max(a,b) ((a>b)?(a):(b))

typedef unsigned char  uint8;

/*****************************************************************************                                                                            
*   Private Data                                                             *
******************************************************************************/

typedef enum tag_machine_code {
  T_ENTER,     /* initial function setup         */
  T_CALL,      /* call function                  */
  T_ARG,       /* push var arg to function call  */
  T_ARG_CONST, /* push const arg to func. call   */
  T_VARS,      /* create vars. / remove args.    */
  T_ADD_REG,   /* add to %eax using ref. w/ ebp  */
  T_ADD_CONST, /* add to %eax a constant value   */
  T_SUB_REG,   /* sub on %eax using ref. w/ ebp  */
  T_MUL_REG,   /* mult on %eax using ref. w/ ebp */
  T_MUL_CONST, /* mult on %eax constant value    */
  T_CMP,       /* cmpl using ref. w/ ebp         */
  T_MOV,       /* movl to %eax using ref. w/ ebp */
  T_MOV_CONST, /* movl to %eax constant value    */
  T_MOV_MEM,   /* movl from %eax to ref. w/ ebp  */
  T_JMP,       /* jmp - jump to end function     */
  T_JNE,       /* jne - skip return              */
  T_LEAVE      /* terminate function             */
} tag_machine_code;

typedef struct machine_code {
  uint8 n_bytes;
  uint8 code[6];
} machine_code;

/* Array with all the possible machine instructions and their respective 
     length in bytes. These will later be used by a series of functions
     to construct the possible operations of the SB language.
   The comments below with | mark the end of the instruction.
*/
static machine_code mc_table[ MC_TABLE_LEN ] = {  
  /* T_ENTER -- push %ebp, [1,2]mov %ebp %esp */ 
    { 3, { 0x55, 0x89, 0xe5 } },

  /* T_CALL -- call, function addr. offset */
    { 5, { 0xe8, 0x00, 0x00, 0x00, 0x00 } }, 

  /* T_ARG -- push, (%ebp), ebp_offset */
    { 3, { 0xff, 0x75, 0x00 } },

  /* T_ARG_CONST -- push, 4 byte value */
    { 5, { 0x68, 0x00, 0x00, 0x00, 0x00 } },

  /* T_VARS -- add, to %esp, 1 byte value */
    { 3, { 0x83, 0xc4, 0x00 } }, 

  /* T_ADD_REG -- add to %eax, (%ebp), ebp_offset */ 
    { 3, { 0x03, 0x45, 0x00 } },

  /* T_ADD_CONST -- add to %eax, 4 byte value */ 
    { 5, { 0x05, 0x00, 0x00, 0x00, 0x00 } },

  /* T_SUB_REG -- sub on %eax, (%ebp), ebp_offset */ 
    { 3, { 0x2b, 0x45, 0x00 } },

  /* T_MUL_REG -- imul on %eax, (%ebp), ebp_offset */ 
    { 4, { 0x0f, 0xaf, 0x45, 0x00 } },

  /* T_MUL_CONST -- imul on %eax, 4 byte value */ 
    { 6, { 0x69, 0xc0, 0x00, 0x00, 0x00, 0x00 } },

  /* T_CMP -- cmpl, (%ebp), ebp_offset, base_comp ($0), dummy */ 
    { 4, { 0x83, 0x7d, 0x00, 0x00 } },

  /* T_MOV -- mov to %eax, (%ebp), ebp_offset */ 
    { 3, { 0x8b, 0x45, 0x00 } },

  /* T_MOV_CONST -- mov to %eax, const value */ 
    { 5, { 0xb8, 0x00, 0x00, 0x00, 0x00 } }, 

  /* T_MOV_MEM -- mov from %eax, to (%ebp), ebp_offset */ 
    { 3, { 0x89, 0x45, 0x00 } },

  /* T_JMP -- jmp, n_bytes offset */ 
    { 5, { 0xe9, 0x00, 0x00, 0x00, 0x00 } },

  /* T_JNE -- jne, 1 byte offset */ 
    { 2, { 0x75, 0x00 } },

  /* T_LEAVE -- [0,1]mov %ebp,%esp, pop %ebp, ret */ 
    { 4, { 0x89, 0xec, 0x5d, 0xc3 } }
};

/* Fix call and jmp refs */
static unsigned int fix_refs[100];
static int refs_count = 0;

/* Table with function addresses */
static unsigned int func_addrs[30];
static int func_count = 0;

/*****************************************************************************                                                                            
*   Private Functions Definitions                                            *
******************************************************************************/

/* Helper Functions */
static uint8 get_ebp_offset(char * s);
static void  error(const char *msg, int line);
static int   copy_array(uint8 *dst, uint8 * src, int n);
static int   get_number_len(int n);

/* Token String Generation */
static void preprocess_file(FILE *f, char ** s, int * sizes);

/* Code Generation */
static int make_code(void ** code, char * lex, int len);
  /* Partial Code Generation */
  static int make_function(uint8 * code, char * lex, int * step_bytes);
  static int make_assignment(uint8 * code, char * lex, int * step_bytes);
  static int make_call(uint8 * code, char * lex, int * step_bytes);
  static int make_arithmetic(uint8 * code, char * lex, int * step_bytes);
  static int make_ret(uint8 * code, char * lex, int * step_bytes);
  static int make_leave(uint8 * code, char * lex, int * step_bytes);

#ifdef DEV
  /* Debugging */
  void dump(uint8 * code, int n_bytes){
    int i;
    for(i = 0; i < n_bytes; i++)
      printf("%-2d  %02x\n", i, code[i]);
  }
#endif

/*****************************************************************************                                                                            
*   Exported Functions Implementation                                        *
******************************************************************************/

void gera(FILE *f, void **code, funcp *entry){
  char * trans_code;
  int * sizes = (int *) malloc(2 * sizeof(int));

  refs_count = 0;
  func_count = 0;

  #ifdef DEV
    int actual_size = 0;
  #endif

  preprocess_file(f, &trans_code, sizes);
  #ifdef DEV
    printf("code[%d - %d]: %s\n", sizes[0], strlen(trans_code), trans_code);
    printf("wc bytes: %d\n", sizes[1]);
  #endif

  *code  = (void *) malloc(sizes[1] * sizeof(unsigned char));  
  #ifdef DEV
    actual_size = make_code(code, trans_code, sizes[0]);
    printf("bytes: %d\n", actual_size);
    dump(*code, actual_size);
  #else
    make_code(code, trans_code, sizes[0]);
  #endif
  *entry = (funcp)(func_addrs[func_count - 1]);

  free(trans_code);  
  return;
}

void libera(void *code){
  free(code);
}


/*****************************************************************************                                                                            
*   Private Functions Implementation                                         *
******************************************************************************/


/***** Token String Generation *****/

/*
  Description:
    This function preprocesses the input file, translating the SB code to a 
      series of tokens that represent the original program, while being easier
      to be later processed and translated to machine code.
    Each token uniquely identifies one of the possible constructs of the
      of the language (a function, a variable, etc). Please refer to the table
      below for the correspondences between symbols and constructs:
      +---------------+---------------+----------------------------+
      |   Construct   |     Token     |          Comments          |
      +---------------+---------------+----------------------------+
      | function      | f num         | num -> number of variables |
      | end           | e             |                            |
      | ret?          | r             |                            |
      | call          | c             |                            |
      | p num | v num | p num | v num |                            |
      | num           | num           |                            |
      | $[-]num       | $[-]num       |                            |
      | =             | =             |                            |
      | +|-|*         | +|-|*         |                            |
      +---------------+---------------+----------------------------+                           
    Furthermore, some more complex operations will be translated into a simpler
      format. Below such translations are presented:
      +--------------+-------------+-----------------+
      |  Operation   | Translation |    Comments     |
      +--------------+-------------+-----------------+
      | call 0 v0    | c0v0        |                 |
      | v0 = v1 + v2 | =v0+v1v2    | Same for - or * |
      +--------------+-------------+-----------------+
    This function also makes some optimizations whenever possible.

  Parameters:
    [FILE *]  f : 
      a file with SB code to be preprocessed
    [char **] s : 
      pointer to string which will store the list of symbols that
      represent the SB program.    
    [int **] sizes : 
      pointer to array with 2 elements in which position:
        [0] is length of the string
        [1] is worst case number of bytes for machine code array

*/
static void preprocess_file(FILE *f, char ** s, int sizes[2]){
  char * transl = (char *) malloc(MAX_SB_TRANS + 10), aux[3];
  int * ret = sizes;
  int i0, i1, i2, func, line = 1;
  char c, c0, v0, v1, v2, op;
  int len = 0, n_bytes = 0, ret_flag = false, func_begin = -1, num_vars = -1;

  if( transl == NULL ){
    fprintf(stderr, "Lack of memory!\n");
    exit(EXIT_FAILURE);
  }

  ret[0] = ret[1] = 0;
  transl[0] = '\0';

  while ((c = fgetc(f)) != EOF) {
    switch (c) {
      case 'f': {  /* function */
        if (fscanf(f, "unction%c", &c0) != 1 || func_begin != -1) 
          error("comando invalido", line);

        /* Add function symbol to translation output */
        sprintf(transl + len, "f ");
        func_begin = len + 1;
        len += 2;
        n_bytes += mc_table[T_ENTER].n_bytes + mc_table[T_VARS].n_bytes;
        break;
      }
      case 'e': {  /* end */
        if (fscanf(f, "nd%c", &c0) != 1 || func_begin == -1) 
          error("comando invalido", line);

        /* Since <end> has been reached, we're done processing the current 
            function. So the return flag is reset for the next function */
        ret_flag = false;

        /* Save how many variables the function uses */
        sprintf(aux, "%X", ++num_vars);
        strncpy(transl + func_begin, aux, 1);
        func_begin = -1;

        /* Add end symbol to translation output */
        sprintf(transl + len, "e");
        len++;
        n_bytes += mc_table[T_LEAVE].n_bytes;
        break;
      }
      case 'v': 
      case 'p': {  /* atribuicao */
        v0 = c;
        if (fscanf(f, "%d = %c", &i0, &c0) != 2)
          error("comando invalido", line);

        /* Ignore if an always occurring return was found */
        if(ret_flag)
          break;

        num_vars = (v0 == 'v') ? max(num_vars, i0) : num_vars;

        /* Add assignment operation to translation output */
        sprintf(transl + len, "=%c%d", v0, i0);
        len += 3;
        n_bytes += mc_table[T_MOV_MEM].n_bytes;

        if (c0 == 'c') { /* call */
          if (fscanf(f, "all %d %c%d", &func, &v1, &i1) != 3) 
            error("comando invalido", line);

          /* Remember highest variable idx seen */
          num_vars = (v1 == 'v') ? max(num_vars, i1) : num_vars;

          /* Add call operation to translation output */
          sprintf(transl + len, "c%d%c%d", func, v1, i1);
          len += 2 + get_number_len(func) + get_number_len(i1);
          n_bytes += mc_table[T_ARG_CONST].n_bytes + mc_table[T_CALL].n_bytes +
            mc_table[T_VARS].n_bytes; /* worst case */
        }
        else { /* operacao aritmetica */
          v1 = c0;
          if (fscanf(f, "%d %c %c%d", &i1, &op, &v2, &i2) != 4)
            error("comando invalido", line);

          num_vars = (v1 == 'v') ? max(num_vars, i1) : num_vars ;
          num_vars = (v2 == 'v') ? max(num_vars, i2) : num_vars ;

          /* Add arithmetic operation to translation output */
          sprintf(transl + len, "%c%c%d%c%d", op, v1, i1, v2, i2);
          len += 3 + get_number_len(i1) + get_number_len(i2);
          n_bytes += mc_table[T_MOV_CONST].n_bytes + 
            mc_table[T_MUL_CONST].n_bytes; /* worst case */
        }

        break;
      }
      case 'r': {  /* ret */        
        if (fscanf(f, "et? %c%d %c%d", &v0, &i0, &v1, &i1) != 4)
           error("comando invalido", line);

        /* Ignore if an always occurring return was found */
        if(ret_flag)
          break;

        num_vars = (v0 == 'v') ? max(num_vars, i0) : num_vars ;
        num_vars = (v1 == 'v') ? max(num_vars, i1) : num_vars ;

        /* Add return operation to translation output, only if the 
             return conditon is $0, a variable or a parameter. In case
             it's not any of these, it will never happen.
           In the case of $0, it's certain that no code past this return
             will ever execute, so we set a flag to ignore all code before
             the end of the function.    
        */
        if((v0 == '$' && i0 == 0) || v0 != '$'){
          if(v0 == '$' && i0 == 0)
            ret_flag = true;

          sprintf(transl + len, "r%c%d%c%d", v0, i0, v1, i1);
          len += 3 + get_number_len(i0) + get_number_len(i1);
          n_bytes += mc_table[T_CMP].n_bytes + mc_table[T_JNE].n_bytes +
            mc_table[T_MOV_CONST].n_bytes + mc_table[T_JMP].n_bytes; /* worst case */
        }

        break;
      }
      default: 
        error("comando desconhecido", line);
    }

    line++;

    fscanf(f, " ");
  }

  ret[0] = len;
  ret[1] = n_bytes;

  (*s) = transl;
}

/***** Code Generation *****/

/*
  Description:
    This function appends the start of function machine code to <code>.

  Parameters:
    [uint8 *]  code:
      pointer to array of machine code instructions
    [char * ]  lex:
      result of input SB file preprocessing
    [int   ]   len:
      length of <lex>

  Returns:
    The total ammount of bytes of the generated code.
*/
static int make_code(void ** code, char * lex, int len){
  int i, step_str = 1, step_bytes = 0, total_bytes = 0;
  uint8 * instrs = (uint8 *) (*code), * aux = NULL;

  for(i = 0; i < len; i += step_str, instrs += step_bytes){
    switch(lex[i]){
      case 'f': {
        step_str = make_function(instrs, lex + i, &step_bytes);
        break;
      }
      case '=': {
        step_str = make_assignment(instrs, lex + i, &step_bytes);
        break;
      }
      case 'r': {
        step_str = make_ret(instrs, lex + i, &step_bytes);
        break;
      }
      case 'e': {
        step_str = make_leave(instrs, lex + i, &step_bytes);
        break;
      }
      default: {
        fprintf(stderr, "[cg] Error at code string[%d] -- %s\n", i, lex);
        exit(EXIT_FAILURE);
      }
    }

    total_bytes += step_bytes;
  }

  /* dummy function start */
  func_addrs[func_count++] = (unsigned int) instrs;

  /* adjust jumps to end */
  for(i = 0; i < refs_count; i++){
    aux = ((uint8 *) fix_refs[i]);
    
    *((int *)(aux + 1)) = (unsigned int) ((func_addrs[aux[1] + 1] 
      - mc_table[T_LEAVE].n_bytes) - (unsigned int)(aux + mc_table[T_JMP].n_bytes));
  }
  func_count--;

  return total_bytes;
}

/*
  Description:
    This function appends the start of function machine code to <code>.

  Parameters:
    [uint8 *]  code:
      pointer to array of machine code instructions
    [char * ]  lex:
      result of input SB file preprocessing
    [int *  ]  step_bytes:
      number of bytes which were appended to <code>

  Returns:
    The number of characters to skip until the next symbol.
*/
static int make_function(uint8 * code, char * lex, int * step_bytes){
  /* add function entry setup */
  *step_bytes = copy_array(code, mc_table[T_ENTER].code, 
    mc_table[T_ENTER].n_bytes);
  func_addrs[func_count++] = (unsigned int)code;

  /* create variables, if necessary */
  if(lex[1] != '0'){
    *step_bytes += copy_array(code + *step_bytes, mc_table[T_VARS].code, 
      mc_table[T_VARS].n_bytes);
    /* set the number of bytes to be summed to %esp */
    code[*step_bytes - 1] = ((lex[1] != 'A') ? (lex[1] - '0') : 10) * -4;
  }

  return 2;
}

/*
  Description:
    This function appends an assignment operation's machine code to <code>.

  Parameters:
    [uint8 *]  code:
      pointer to array of machine code instructions
    [char * ]  lex:
      result of input SB file preprocessing
    [int *  ]  step_bytes:
      number of bytes which were appended to <code>

  Returns:
    The number of characters to skip until the next symbol.
*/
static int make_assignment(uint8 * code, char * lex, int * step_bytes){
  int op_len = 0;
 
  lex++; /* skip the = symbol */

  /* call */
  if(lex[2] == 'c')
    op_len = make_call(code, lex + 2, step_bytes);
  /* arithmetic operation */
  else  /* lex[2] == '+'|'-'|'*', arithmetic op. */
    op_len = make_arithmetic(code, lex + 2, step_bytes);
   
  /* Whether it's a function call or an arithmetic operation on the rhs,
       we will have the result on %eax, which is then copied to the
       variable or parameter on the lhs. */

  /* add mov from %eax to the var/param */
  copy_array(code + *step_bytes, mc_table[T_MOV_MEM].code, 
    mc_table[T_MOV_MEM].n_bytes);

  code[*step_bytes + 2] = get_ebp_offset(lex); /* set the (%ebp) offset */
  *step_bytes += mc_table[T_MOV_MEM].n_bytes;

  return (op_len + 3);
}

/*
  Description:
    This function appends a function's call machine code to <code>.

  Parameters:
    [uint8 *]  code:
      pointer to array of machine code instructions
    [char * ]  lex:
      result of input SB file preprocessing
    [int *  ]  step_bytes:
      number of bytes which were appended to <code>

  Returns:
    The number of characters to skip until the next symbol.
*/
static int make_call(uint8 * code, char * lex, int * step_bytes){
  int op_len = 1, number = 0, func_id = 0;

  /* Used to skip to the argument */
  sscanf(lex + 1, "%d", &func_id);
  op_len += get_number_len(func_id);

  /* First, push the argument */

  /* constant value */
  if(lex[op_len] == '$'){
    *step_bytes = mc_table[T_ARG_CONST].n_bytes;
    copy_array(code, mc_table[T_ARG_CONST].code, *step_bytes);

    sscanf(lex + op_len + 1, "%d", &number);
    *((int *)(code + 1)) = number;
  }
  /* local var or param */
  else {
    *step_bytes = mc_table[T_ARG].n_bytes;
    copy_array(code, mc_table[T_ARG].code, *step_bytes);

    code[*step_bytes - 1] = get_ebp_offset(lex + op_len);
  }  

  /* Then, add call to function */
  copy_array(code + *step_bytes, mc_table[T_CALL].code, 
    mc_table[T_CALL].n_bytes);  

  *((int *)(code + *step_bytes + 1)) = (unsigned int) (func_addrs[func_id] - 
    (unsigned int)(code + *step_bytes + mc_table[T_CALL].n_bytes));

  *step_bytes += mc_table[T_CALL].n_bytes;

  /* Finally, remove the pushed argument */
  copy_array(code + *step_bytes, mc_table[T_VARS].code, 
    mc_table[T_VARS].n_bytes);
  code[*step_bytes + 2] = 4;

  *step_bytes += mc_table[T_VARS].n_bytes;  
  return (op_len + get_number_len(number) + 1);
}

/*
  Description:
    This function appends an arithmetic operation's machine code to <code>.

  Parameters:
    [uint8 *]  code:
      pointer to array of machine code instructions
    [char * ]  lex:
      result of input SB file preprocessing
    [int *  ]  step_bytes:
      number of bytes which were appended to <code>

  Returns:
    The number of characters to skip until the next symbol.
*/
static int make_arithmetic(uint8 * code, char * lex, int * step_bytes){
  int number = 0, next_opr = 0;
  tag_machine_code type;

  /* First, move to %eax the first operand */
  /* var/param */  
  if(lex[1] != '$'){
    *step_bytes = mc_table[T_MOV].n_bytes;
    copy_array(code, mc_table[T_MOV].code, *step_bytes);

    code[2] = get_ebp_offset(lex + 1);
  }
  /* const value */
  else {
    *step_bytes = mc_table[T_MOV_CONST].n_bytes;
    copy_array(code, mc_table[T_MOV_CONST].code, *step_bytes);

    sscanf(lex+2, "%d", &number);
    *((int *)(code + 1)) = number;
  }

  /* skip to next operand */
  next_opr = get_number_len(number) + 2;
  number = 0;
  
  /* set the base type for the operation. Later, based on the actual type
    of the second operand, <type> will be adjusted to either define the use of a
    local var/param or of a constant value */
  switch(lex[0]){
    case '+': {
      type = T_ADD_REG;
      break;
    }
    case '-': {
      type = T_SUB_REG; 
      break;
    }
    case '*': {
      type = T_MUL_REG;  
      break;
    }
  }

  /* second operand if a var/local param */
  if(lex[next_opr] != '$'){
    copy_array(code + *step_bytes, mc_table[type].code,
      mc_table[type].n_bytes);

    if(type == T_MUL_REG)
      code[*step_bytes + 3] = get_ebp_offset(lex + next_opr);
    else
      code[*step_bytes + 2] = get_ebp_offset(lex + next_opr);

    *step_bytes += mc_table[type].n_bytes;
  }
  /* constant value */
  else {
    if(type == T_SUB_REG)
      copy_array(code + *step_bytes, mc_table[T_ADD_CONST].code,
        mc_table[T_ADD_CONST].n_bytes);
    else
      copy_array(code + *step_bytes, mc_table[type + 1].code,
        mc_table[type + 1].n_bytes);

    sscanf(lex + next_opr + 1, "%d", &number);

    if(type == T_MUL_REG)
      *((int *)(code + *step_bytes + 2)) = number;
    else
      *((int *)(code + *step_bytes + 1)) = 
        ((type == T_SUB_REG) ? -number : number);

    if(type == T_SUB_REG)
      *step_bytes += mc_table[T_ADD_CONST].n_bytes;
    else
      *step_bytes += mc_table[type + 1].n_bytes;
  }  

  return (next_opr + get_number_len(number) + 1);
}

/*
  Description:
    This function appends a return operation's machine code to <code>.

  Parameters:
    [uint8 *]  code:
      pointer to array of machine code instructions
    [char * ]  lex:
      result of input SB file preprocessing
    [int *  ]  step_bytes:
      number of bytes which were appended to <code>

  Returns:
    The number of characters to skip until the next symbol.
*/
static int make_ret(uint8 * code, char * lex, int * step_bytes){
  int number = 0, op_len = 0;
  *step_bytes = 0;  

  /* We do not need to compare $0 with $0, so just compare vars/params */
  if(lex[1] != '$'){
    /* append comparison, setting the var/param */
    *step_bytes = copy_array(code, mc_table[T_CMP].code, mc_table[T_CMP].n_bytes);
    code[2] = get_ebp_offset(lex + 1);

    /* append jne to skip mov if cmp fails */
    *step_bytes += copy_array(code + *step_bytes, mc_table[T_JNE].code, 
      mc_table[T_JNE].n_bytes);
  }  

  /* Below, a jmp to the end of the function will only be added if
      e (end) does not 'immediately' follow r (ret) in <lex> */

  /* move return value to %eax */
  if(lex[3] == '$'){
    sscanf(lex + 4, "%d", &number);
    op_len = 4 + get_number_len(number);

    /* fix previous jne */
    if(*step_bytes != 0)
      code[*step_bytes - 1] = (uint8) (mc_table[T_MOV_CONST].n_bytes + 
        (lex[op_len] == 'e' ? 0 : mc_table[T_JMP].n_bytes));

    copy_array(code + *step_bytes, mc_table[T_MOV_CONST].code, 
      mc_table[T_MOV_CONST].n_bytes);    

    /* set value to be moved to %eax */
    *((int *)(code + *step_bytes + 1)) = number;    

    *step_bytes += mc_table[T_MOV_CONST].n_bytes;
  }
  else {
    op_len = 5;

    /* fix previous jne */
    if(*step_bytes != 0)
      code[*step_bytes - 1] = (uint8) (mc_table[T_MOV].n_bytes +
        (lex[op_len] == 'e' ? 0 : mc_table[T_JMP].n_bytes));

    copy_array(code + *step_bytes, mc_table[T_MOV].code, 
      mc_table[T_MOV].n_bytes); 
    code[*step_bytes + 2] = get_ebp_offset(lex + 3);

    *step_bytes += mc_table[T_MOV].n_bytes; 
  }

  /* jump to end (if it does not immediately follow ret), storing current
       function id (to later replace w/ offset) */
  if(lex[op_len] != 'e'){
    copy_array(code + *step_bytes, mc_table[T_JMP].code, 
      mc_table[T_JMP].n_bytes);
    *((int *)(code + *step_bytes + 1)) = (func_count - 1);

    fix_refs[refs_count++] = (unsigned int) code + *step_bytes;

    *step_bytes += mc_table[T_JMP].n_bytes;
  }

  return op_len;
}

/*
  Description:
    This function appends the end of function machine code to <code>.

  Parameters:
    [uint8 *]  code:
      pointer to array of machine code instructions
    [char * ]  lex:
      result of input SB file preprocessing
    [int *  ]  step_bytes:
      number of bytes which were appended to <code>

  Returns:
    The number of characters to skip until the next symbol.
*/
static int make_leave(uint8 * code, char * lex, int * step_bytes){
  *step_bytes = copy_array(code, mc_table[T_LEAVE].code, 
    mc_table[T_LEAVE].n_bytes);  

  return 1;
}

/***** Helper Functions *****/

/*
  Description:
    Calculates ebp offset for a given variable or parameter.

  Parameters:
    [char * ] str : preprocessing encoded string in the form: (v|p)num

  Returns:
    %ebp offset
*/
static uint8 get_ebp_offset(char * s){
  if(s[0] == 'p')
    return 8 + 4 * (s[1] - '0');
  return -4 * (s[1] - '0' + 1);
}

/*
  Description:
    This function returns the number of digits that <n> takes in a string.

  Parameters:
    [int ] n : the number

  Returns:
    The number of digits in a number.
*/
static int get_number_len(int n){
  int len = ((n <= 0) ? 1 : 0);

  while(n != 0){
    n /= 10;
    len++;
  }

  return len;
}

/*
  Description:
    Copies the <n> first elements of array <src> to array <dst>. <dst> should
      have at least <n> available positions.

  Parameters:
      [int *]  dst : destination array
      [int *]  src : source  array
       [int ]  n : number of elements to be copied
  Returns: <n>
*/
static int copy_array(uint8 *dst, uint8 * src, int n) {
  memcpy(dst, src, n);
  return n;
}

/*
  Description:
    This function outputs an optional error message to stderr and terminates
      the execution of the compilation process.

  Parameters:
    [char *]  msg  : an optional error messaged to be logged
      [int ]  line : line in the SB file with a problem
*/
static void error (const char *msg, int line) {
  fprintf(stderr, "Erro %s na linha %d\n", 
          (msg == NULL ? "" : msg), line);
  exit(EXIT_FAILURE);
}
