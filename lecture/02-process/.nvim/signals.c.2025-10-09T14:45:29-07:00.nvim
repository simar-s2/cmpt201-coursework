#define _POSIX_C_SOURSE 200809
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *message = "CTRL-c pressed\n";
void handle_sigint(int signum) {
  write(STDOUT_FILENO, message, strlen(message));
}

int main() {
  struct sigaction act;
  act.sa_handler = handle_sigint;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);

  int ret = sigaction(SIGINT, &act, NULL);
  if (ret == -1) {
    perror("Sigaction() failed");
    exit(EXIT_FAILURE);
  }

  while (true) {
    sleep(1);
  }
}
