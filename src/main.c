#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USER_INPUT_BUF_SIZE 256
#define MAX_ARGS 32

enum COMMAND { EXIT, ECHO, TYPE, CMD_UNKNOWN };

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

  if (strcmp(cmd, "type") == 0)
    return TYPE;

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
    char *command = parsed_command[0];
    parsed_command += 1;

    switch (cmd) {
    case TYPE:
      if (arg_num != 2) {
        printf("Too many arguments passed for the command %s, expected %d "
               "arguments, have %d arguments\n",
               command, 2, arg_num);
      }
      enum COMMAND check_given_cmd = get_command(parsed_command[0]);
      if (check_given_cmd == CMD_UNKNOWN) {
        printf("%s: not found\n", parsed_command[0]);
      } else {
        printf("%s is a shell builtin\n", parsed_command[0]);
      }
      break;

    case EXIT:
      if (arg_num != 2) {
        printf("Too many arguments passed for the command %s, expected %d "
               "arguments, have %d arguments\n",
               command, 2, arg_num);
      }

      int exit_command_arg;
      if (sscanf(parsed_command[0], "%d", &exit_command_arg) != 1) {
        printf("Invalid argument passed, expected number\n");
      } else {
        return exit_command_arg;
      }

    case CMD_UNKNOWN:
      printf("%s: command not found", buf);
      break;

    case ECHO:
      while (*parsed_command != NULL) {
        printf("%s ", *parsed_command);
        parsed_command += 1;
      }
      printf("\n");
      break;
    }
  }
}
