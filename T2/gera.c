/* Carlos_Augusto_Lima_Mattoso 1210553 3WA */

#include <stdio.h>
#include <stdlib.h>

#ifdef _DEV
  #include "gera.h"
#endif

/*****************************************************************************                                                                            *
*   Constant Definitions                                                     *
******************************************************************************/

#define true 1
#define false 0

#define MAX_SB_TRANS 784 /* largest possible size for translated SB */


/*****************************************************************************                                                                            *
*   Private Functions Definitions                                            *
******************************************************************************/


static void error (const char *msg, int line);

static  int get_number_len(int n);
static void preprocess_file(FILE *f, char ** s, int ** sizes);


/*****************************************************************************                                                                            *
*   Exported Functions Implementation                                        *
******************************************************************************/


void gera(FILE *f, void **code, funcp *entry){
  /* Return NULL in both return parameters if somehting goes wrong */ 
  code  = NULL;
  entry = NULL;

  if(f == NULL){
    fprintf(stderr, "SB File not open!\n");
    exit(EXIT_FAILURE);
  }

  return;
}


/*****************************************************************************                                                                            *
*   Private Functions Implementation                                         *
******************************************************************************/


/*
  Description:
    This function preprocesses the input file, translating the SB code to a 
      series of symbols that represent the original program, while being easier
      to be later processed and translated to machine code.
    Each symbol uniquely identifies one of the possible constructs of the
      of the language (a function, a variable, etc). Please refer to the table
      below for the correspondences between symbols and constructs:
      +---------------+---------------+
      |   Construct   |    Symbol     |
      +---------------+---------------+
      | function      | f             |
      | end           | e             |
      | ret?          | r             |
      | call          | c             |
      | p num | v num | p num | v num |
      | num           | num           |
      | $[-]num       | $[-]num       |
      | =             | =             |
      | +|-|*         | +|-|*         |
      +---------------+---------------+
    Furthermore, some more complex operations will be translated into a simpler
      format. Below such translations are presented:
      +--------------+-------------+-----------------+
      |  Operation   | Translation |    Comments     |
      +--------------+-------------+-----------------+
      | call 0 v0    | c0v0        |                 |
      | v0 = v1 + v2 | =v0+v1v2    | Same for - or * |
      +--------------+-------------+-----------------+

  Parameters:
    [FILE *]  f : 
      a file with SB code to be preprocessed
    [char **] s : 
      pointer to string which will store the list of symbols that
      represent the SB program.    
    [int **] sizes : 
      pointer to array with 2 elements in which position:
        [0] is length of the string
        [1] is number of bytes for machine code array

*/
static void preprocess_file(FILE *f, char ** s, int ** sizes){
  char * transl = (char *) malloc(MAX_SB_TRANS + 10);
  int * ret = (int *) malloc(2 * sizeof(int));
  int i0, i1, i2, func, len, line, ret_flag;
  char c, c0, v0, v1, v2, op;

  if( transl == NULL || ret == NULL){
    fprintf(stderr, "Falta de memoria!");
    exit(EXIT_FAILURE);
  }

  ret[0] = ret[1] = 0;
  transl[0] = '\0';

  len = 0;
  ret_flag = false; 
    /* Tells if the current function being processed has a return clause 
       which always happens, making all code after it irrelevant. If
       set to true all code is ignored before the terminating end clause. */ 

  while ((c = fgetc(f)) != EOF) {
    switch (c) {
      case 'f': {  /* function */
        if (fscanf(f, "unction%c", &c0) != 1) 
          error("comando invalido", line);

        /* Add function symbol to translation output */
        sprintf(transl + len, "f");
        len++;

        printf("function\n");

        break;
      }
      case 'e': {  /* end */
        if (fscanf(f, "nd%c", &c0) != 1) 
          error("comando invalido", line);

        if(ret_flag)
          ret_flag = false;

        /* Add end symbol to translation output */
        sprintf(transl + len, "e");
        len++;

        printf("end\n");

        break;
      }
      case 'v': 
      case 'p': {  /* atribuicao */
        v0 = c;

        if (fscanf(f, "%d = %c", &i0, &c0) != 2)
          error("comando invalido", line);

        /* Ignore if an always occurring return was found */
        if(ret_flag)
          continue;

        /* Add assignment operation to translation output */
        sprintf(transl + len, "=%c%d", v0, i0);
        len += 2 + get_number_len(i0);

        if (c0 == 'c') { /* call */
          if (fscanf(f, "all %d %c%d", &func, &v1, &i1) != 3) 
            error("comando invalido", line);

          /* Add call operation to translation output */
          sprintf(transl + len, "c%d%c%d", func, v1, i1);
          len += 2 + get_number_len(func) + get_number_len(i1);

          printf("%c%d = call %d %c%d\n", v0, i0, func, v1, i1);
        }
        else { /* operacao aritmetica */
          v1 = c0;

          if (fscanf(f, "%d %c %c%d", &i1, &op, &v2, &i2) != 4)
            error("comando invalido", line);

          /* Add arithmetic operation to translation output */
          sprintf(transl + len, "%c%c%d%c%d", op, v1, i1, v2, i2);
          len += 3 + get_number_len(i1) + get_number_len(i2);

          printf("%c%d = %c%d %c %c%d\n", v0, i0, v1, i1, op, v2, i2);
        }

        break;
      }
      case 'r': {  /* ret */
        
        if (fscanf(f, "et? %c%d %c%d", &v0, &i0, &v1, &i1) != 4)
           error("comando invalido", line);

        /* Ignore if an always occurring return was found */
        if(ret_flag)
          continue;

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
        }
         

        printf("ret? %c%d %c%d\n", v0, i0, v1, i1);

        break;
      }
      default: 
        error("comando desconhecido", line);
    }

    line ++;

    fscanf(f, " ");
  }



  (*s) = transl;
  (*sizes) = ret;
}

static int get_number_len(int n){
  int len = 0;

  while(n != 0){
    n /= 10;
    len++;
  }

  return len;
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