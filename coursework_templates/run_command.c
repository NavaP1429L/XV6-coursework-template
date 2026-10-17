
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

void execute_line(char** args) {

  // Use fork() to create a child process. Determine if the process
  // is a parent or a child, and run exec() if it is a child, and
  // run wait() if it is a parent.
  int f = fork();

  if (f < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }
  else if (f == 0) {
    exec(args[0], args);
    fprintf(2, "exec failed\n");
    exit(1);
  }
  else {
    wait(0);
  }
}


int main(int argc, char *argv[]) {

  // Two pointers: to a command and to a null terminator.
  char *args[2] = {0};

  // Fixed-size storage for reading in a line of input, and a pointer
  // to a particular character in this storage.
  char buf[512], *b;

  // Point b to the first character in the line.
  b = buf;
  args[0] = buf;

  // Read characters from the standard input. When we find '\n' that
  // means one line has terminated - execute the command.
  fprintf(2, "Please enter command and press enter (type 'q' to exit):\n");

  while (1) {
    b = buf;
    args[0] = buf;
    args[1] = 0;

    while (read(0, b, 1) > 0) {
      if (*b == '\n') {
        *b = '\0';

        if (buf[0] == 'q' && buf[1] == '\0') {
          exit(0);
        }

        execute_line(args);

        fprintf(2, "Please enter command and press enter (type 'q' to exit):\n");
        break;
      }
      else {
        b++;
      }
    }
  }

  exit(0);
}
}
