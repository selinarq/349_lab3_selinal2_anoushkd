#include <stdio.h>

#define UNUSED __attribute__((unused))

#include <servo.h>

int main(UNUSED int argc, UNUSED char const *argv[]){
  fflush(stdout);
  printf("Hello World from User.\n");

  char input[10];
  write(1, "Type: ", 6);
  //user types in their input, kernel reads these characters
  int n = read(0, input, 10);
  //print to terminal what user typed
  write(1, input, n);

  if (sbrk(1) == (void *)-1){
    write(1, "failed\n", 7);
  }
  else{
    write(1, "works\n", 6);
  }
  return 0;
}