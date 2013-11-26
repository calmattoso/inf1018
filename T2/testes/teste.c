#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../gera.h"

int main(int argc, char ** argv){
	FILE * source_code;
	void * code;
	funcp entry;
	int i;

	if( argc < 2 ){
		fprintf(stderr, "Must specify file path!\n");
		exit(EXIT_FAILURE);
	}

	source_code = fopen(argv[1], "rt");
	if(source_code == NULL){
		fprintf(stderr, "Could not open \"%s\"\n", argv[1]);
	}
	else {
		gera(source_code, &code, &entry);
		for(i = 0; i <= 10; i++)
			printf("%d\n", (*entry)(i,20,30,40,50,60,70,80,90,100));
		libera(code);
	}
	
	return 0;
}
