#include <stdio.h>
#include <unistd.h>

int main() {
  for (int i = 0; i < 5; i++) {
    printf("sleep %d\n", i + 1);
    sleep(3);
  }

  printf("DONE\n");
}
