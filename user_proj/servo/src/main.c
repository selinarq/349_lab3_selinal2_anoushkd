#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <servo.h>

//user interface to type in commands to control servo
int main(int argc, char const *argv[]){
  //store commands in array
  char command[50];
  int channel;
  int angle;

  printf("Welcome to Servo Controller!\n");
  printf("Commands\n");
  printf("enable <ch>: Enable servo channel\n");
  printf("disable <ch>: Disable servo channel\n");
  printf("set <ch> <angle>: Set servo angle\n");

  while (1) {
    //read string word
    scanf("%49s", command);

    if (strcmp(command, "enable") == 0) {
      if (scanf("%d", &channel) == 1)
        servo_enable(channel);
      else
        printf("Invalid command\n");
    }
    else if (strcmp(command, "disable") == 0) {
      if (scanf("%d", &channel) == 1)
        servo_disable(channel);
      else
        printf("Invalid command\n");
    }
    else if (strcmp(command, "set") == 0) {
      if (scanf("%d %d", &channel, &angle) == 2 && angle >= 0 && angle <= 180)
        servo_set(channel, angle);
      else
        printf("Invalid command\n");
    }
    else {
      printf("Invalid command\n");
    }
  }
  return 0;
}