#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 255

int main(int argc, char *argv[]) {
  while (1) {
    setbuf(stdout, NULL);

    printf("$ ");

    char *buf = malloc(BUF_SIZE * sizeof(char));

    fgets(buf, BUF_SIZE, stdin);
    buf[strcspn(buf, "\n")] = 0;

    printf("%s: command not found\n", buf);
  }

  return 0;
}
