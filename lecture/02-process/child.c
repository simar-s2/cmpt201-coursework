#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main(void) {
  pid_t pid = fork();
  if (pid) {
    int wstatus = 0;
    if (waitpid(pid, &wstatus, 0) == -1) {
      perror("waitpid");
      wait(EXIT_FAILURE);
    }

    if (WIFEXITED(wstatus)) {
      printf("Child done with exit status: %d\n", WEXITSTATUS(wstatus));
    }
  }
}
