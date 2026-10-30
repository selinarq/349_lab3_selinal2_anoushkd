#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <servo.h>
#include <string.h>


#define NUM_CHANNELS 2


//user interface to type in commands to control servo
int main(int argc, char const *argv[]){
  (void) argc;
  (void) *argv;

  //store commands in array
  int channel;
  int angle;

  printf("Welcome to Servo Controller!\n");
  printf("Commands\n");
  printf("enable <ch>: Enable servo channel\n");
  printf("disable <ch>: Disable servo channel\n");
  printf("set <ch> <angle>: Set servo angle\n");

  char buffer[128];

  while (1) {
    //read string word
    int length = read(STDIN_FILENO, buffer, 128);
    if (length > 0) {
      //check if enable command
      if (sscanf(buffer, "enable %d\r", &channel) == 1) {
        servo_enable(channel, 1);
        printf("enabled!\n");
        //check for disable command
      } else if (sscanf(buffer, "disable %d\r", &channel) == 1) {
        servo_enable(channel, 0);
        printf("disabled!\n");
        //check for setting the angle
      } else if (sscanf(buffer, "set %d %d\r", &channel, &angle) == 2) {
        if (angle < 0 || angle > 180) {
          printf("3: Invalid command\n");
        } else {
          servo_set(channel, angle);
          printf("Set: %d %d\n", channel, angle);
        }
      } else {
        printf("Invalid command\n");
      }
    }
    fflush(stdout);
  }

  return 0;
}