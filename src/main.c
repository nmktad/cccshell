#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USER_INPUT_BUF_SIZE 256
#define MAX_ARGS 32

enum COMMAND { EXIT, ECHO, CMD_UNKNOWN };

int parse(char *user_input, char **parsed_command) {
  int count = 0;
  char *token = strtok(user_input, " ");
  while (token != NULL) {
    parsed_command[count++] = token;
    token = strtok(NULL, " ");
  }
  parsed_command[count] = NULL;
  return count;
}

enum COMMAND get_command(char *cmd) {
  if (cmd == NULL)
    return CMD_UNKNOWN;

  if (strcmp(cmd, "exit") == 0)
    return EXIT;

  if (strcmp(cmd, "echo") == 0)
    return ECHO;

  return CMD_UNKNOWN;
}

int main(int argc, char *argv[]) {
  setbuf(stdout, NULL);

  while (1) {
    printf("$ ");

    char *buf = malloc(USER_INPUT_BUF_SIZE * sizeof(char));
    char **parsed_command = malloc(MAX_ARGS * sizeof(char *));

    if (fgets(buf, USER_INPUT_BUF_SIZE, stdin) == NULL) {
      printf("\n");
      break;
    }

    buf[strcspn(buf, "\n")] = '\0';

    int arg_num = parse(buf, parsed_command);

    enum COMMAND cmd = get_command(parsed_command[0]);
    parsed_command += 1;

    switch (cmd) {
    case EXIT:
      if (arg_num > 2) {
        printf("Too many arguments passed for the command %s, expected %d "
               "arguments, have %d arguments\n",
               buf, 2, arg_num);
      }

      int exit_command_arg;
      if (sscanf(parsed_command[0], "%d", &exit_command_arg) != 1) {
        printf("Invalid argument passed, expected number");
      }

      return exit_command_arg;

    case CMD_UNKNOWN:
      printf("%s: command not found\n", buf);

    case ECHO:
      while (*parsed_command != NULL) {
        printf("%s ", *parsed_command);
        parsed_command += 1;
      }
      printf("\n");
    }
  }
}
