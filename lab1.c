#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {

  char *buffer = NULL;
  size_t bufsize = 0;
  size_t linelen;

  printf("Please enter some text: ");
  linelen = getline(&buffer, &bufsize, stdin);

  if (linelen == -1) {
    perror("getline");
    free(buffer);
    return 1;
  }

  printf("Tokens:\n");

  char *saveptr;
  char *token = strtok_r(buffer, " \n", &saveptr);

  while (token != NULL) {
    printf("  %s\n", token);
    token = strtok_r(NULL, " \n", &saveptr);
  }

  free(buffer);
  return 0;
}
