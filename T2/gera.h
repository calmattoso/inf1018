typedef int (*funcp) ();
void gera(FILE *f, void **code, funcp *entry);
void libera(void *code);