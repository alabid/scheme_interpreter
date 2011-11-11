#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"



int main(int argc, char*argv[]) {
  Environment *topEnv = createTopFrame();
  //Environment *temp = createFrame(topEnv);
  //int i = 0;
  //for (i= 0; i<100;i++){
  // insertEnv(temp);
  //}
  
  loadFromFile(stdin, topEnv);
  
  destroyTopFrame(topEnv);
  return -1;
}
