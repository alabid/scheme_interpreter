#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"



int main(int argc, char*argv[]) {
  Environment *topEnv = createTopFrame();
 
  interface(topEnv);
  
  destroyTopFrame(topEnv);
  return -1;
}
