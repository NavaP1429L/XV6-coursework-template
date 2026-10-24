#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

void execute_left_side(char** args, int *p) {

  // Create a child process for executing the
  // left-side command.
  int f = fork();
  if (f < 0) {
    fprintf(2, "run_command: fork failed\n");
    exit(1);
  } else if (f == 0) {
    // close standard output and use the pipe instead
    close(1);
    dup(p[1]);

    // run the first command
    exec(args[0], args);

    fprintf(2, "run_command: exec failed\n");
    exit(1);
  }
  // Do not use wait() here as before. Parent should continue
  // in the main method.
}

void execute_right_side(char** args, int *p) {

  // Create a child process for executing the
  // right-side command.
  int f = fork();
  if (f < 0) {
    fprintf(2, "run_command: fork failed\n");
    exit(1);
  } else if (f == 0) {
    // close standard input and use the pipe instead
    close(0);
    dup(p[0]);

    // run the second command
    exec(args[3], &args[3]);

    fprintf(2, "run_command: exec failed\n");
    exit(1);
  } else {
    // wait for the two children
    wait(0);
    wait(0);
  }
}

int main(int argc, char *argv[]) {

  // Array of pointers for storing a command and a null terminator.
  char *args[6] = {0};
  args[2] = 0;
  args[5] = 0;

  // Pipe for communication between commands.
  int p[2];

  // Storage for reading in data from the standard input.
  char buf[512], *b;
  args[0] = buf;

  // Point b to the first character in the line.
  b = buf;

  while (1) {
    // Read characters from the standard input. When we find '\n' that
    // means one line has terminated.
    b = buf;

    // make a new pipe for each pair of commands
    pipe(p);

    fprintf(2, "\nPlease enter the first command and press enter (type 'q' to exit):\n");

    while (read(0, b, 1) > 0) {
      if (*b == '\n') {
        // Replace the end of line character by the string terminating
        // character.
        *b = '\0';

        if (buf[0] == 'q')
          exit(0);
        else {
          // run the first command
          execute_left_side(args, p);

          // close the write side so the second command can finish
          close(p[1]);
          break;
        }
      } else if (*b == ' ') {
        // Terminate the string.
        *b = '\0';
        b++;

        // Save the pointer to the next string.
        args[1] = b;
      } else {
        b++;
      }
    }

    fprintf(2, "\nPlease enter the second command and press enter:\n");
    args[3] = b;

    while (read(0, b, 1) > 0) {
      if (*b == '\n') {
        // Replace the end of line character by the string terminating
        // character.
        *b = '\0';

        // run the second command
        execute_right_side(args, p);
        break;
      } else if (*b == ' ') {
        // Terminate the string.
        *b = '\0';
        b++;

        // Save the pointer to the next string.
        args[4] = b;
      } else {
        b++;
      }
    }
  }

  exit(0);
}
