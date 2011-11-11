#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"



int main(int argc, char*argv[]) {
  Environment *topEnv = createTopFrame();
  Environment *temp = createFrame(topEnv);
  int i = 0;
  Environment *second=temp;
  temp = insertEnv(temp, topEnv);
 
  for (i= 0; i<100;i++){
    second = createFrame(temp);
    second = insertEnv(second, temp);
    temp = second;
  }
  
  //loadFromFile(stdin, topEnv);
  
  destroyTopFrame(topEnv);
  return -1;
}
