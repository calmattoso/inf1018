/* Carlos_Augusto_Lima_Mattoso 1210553 3WA */

#include <stdio.h>
#include <stdlib.h>

#ifdef _DEV
  #include "gera.h"
#endif

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
    [FILE *] f : a file with SB code to be preprocessed

  Returns:
    [char *] a string with the list of symbols that represent the SB program.

*/
char * preprocess_file(FILE *f);

void gera(FILE *f, void **code, funcp *entry){
  /* Return NULL in both return parameters if somehting goes wrong */ 
  code  = NULL;
  entry = NULL;
  asdassda

  if(f == NULL){
    fprintf(stderr, "SB File not open!\n");
    exit(EXIT_FAILURE);
  }

  return;
}