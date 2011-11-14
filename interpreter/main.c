#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"



int main(int argc, char*argv[]) {
  Environment *topEnv = createTopFrame();

  if (argc > 1) {
    int i;

    for (i = 1; i < argc ; i++) {
      FILE *file = fopen( argv[i], "r" );

        /* fopen returns 0, the NULL pointer, on failure */
        if (!file) {
	  printf( "Could not load: %s\n", argv[i]);
	  return 1;
        } else {
	  printf("loading... %s \n", argv[i]);
	  loadFromFile(file, topEnv);
        }
    }
  }

  interface(topEnv);
  
  destroyTopFrame(topEnv);
  return -1;
}
