#include <limits.h> // PATH_MAX (if available)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h> // access()

#define USER_INPUT_BUF_SIZE 256
#define MAX_ARGS 32

enum COMMAND { CMD_EXIT, CMD_ECHO, CMD_TYPE, CMD_UNKNOWN, CMD_PWD };

int run_external(char **argv) {
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return -1;
  }

  if (pid == 0) {
    execvp(argv[0], argv);
    perror("execvp");
    _exit(127);
  }

  int status;
  if (waitpid(pid, &status, 0) < 0) {
    perror("waitpid");
    return -1;
  }

  return status;
}

int parse(char *user_input, char **argv, int max_args) {
  int count = 0;
  char *token = strtok(user_input, " ");

  while (token != NULL && count < max_args - 1) {
    argv[count++] = token;
    token = strtok(NULL, " ");
  }

  argv[count] = NULL;
  return count;
}

enum COMMAND get_command(const char *cmd) {
  if (cmd == NULL) {
    return CMD_UNKNOWN;
  }

  if (strcmp(cmd, "exit") == 0)
    return CMD_EXIT;
  if (strcmp(cmd, "echo") == 0)
    return CMD_ECHO;
  if (strcmp(cmd, "type") == 0)
    return CMD_TYPE;
  if (strcmp(cmd, "pwd") == 0)
    return CMD_PWD;

  return CMD_UNKNOWN;
}

int is_builtin_name(const char *name) {
  enum COMMAND c = get_command(name);
  return (c != CMD_UNKNOWN);
}

int find_in_path(const char *name, char *out_buf, size_t out_buf_size) {
  char *path_env = getenv("PATH");
  if (!path_env || !name || !*name) {
    return 0;
  }

  char path_copy[4096];
  strncpy(path_copy, path_env, sizeof(path_copy));
  path_copy[sizeof(path_copy) - 1] = '\0';

  char *dir = strtok(path_copy, ":");
  while (dir != NULL) {
    if (snprintf(out_buf, out_buf_size, "%s/%s", dir, name) >=
        (int)out_buf_size) {
      dir = strtok(NULL, ":");
      continue;
    }

    if (access(out_buf, X_OK) == 0) {
      return 1;
    }

    dir = strtok(NULL, ":");
  }

  return 0;
}

int main(void) {
  setbuf(stdout, NULL);

  while (1) {
    printf("$ ");

    char buf[USER_INPUT_BUF_SIZE];
    char *argv[MAX_ARGS];

    if (fgets(buf, sizeof(buf), stdin) == NULL) {
      printf("\n");
      break;
    }

    buf[strcspn(buf, "\n")] = '\0';

    int arg_num = parse(buf, argv, MAX_ARGS);
    if (arg_num == 0) {
      continue;
    }

    const char *command = argv[0];
    enum COMMAND cmd = get_command(command);

    char **args = &argv[1];
    int argc = arg_num - 1;

    switch (cmd) {
    case CMD_TYPE: {
      if (argc != 1) {
        printf("%s: expected 1 argument, got %d\n", command, argc);
        break;
      }

      const char *target = args[0];

      if (is_builtin_name(target)) {
        printf("%s is a shell builtin\n", target);
        break;
      }

      char path_buf[4096];
      if (find_in_path(target, path_buf, sizeof(path_buf))) {
        printf("%s is %s\n", target, path_buf);
      } else {
        printf("%s: not found\n", target);
      }
      break;
    }

    case CMD_PWD: {
      char cwd[1024];
      getcwd(cwd, sizeof(cwd));
      printf("%s\n", cwd);
      break;
    }

    case CMD_EXIT: {
      if (argc == 0) {
        return 0;
      }
      if (argc > 1) {
        printf("%s: too many arguments\n", command);
        break;
      }

      char *endptr = NULL;
      long code = strtol(args[0], &endptr, 10);
      if (endptr == args[0] || *endptr != '\0') {
        printf("%s: numeric argument required\n", command);
        break;
      }

      return (int)code;
    }

    case CMD_ECHO: {
      for (int i = 0; i < argc; i++) {
        printf("%s", args[i]);
        if (i + 1 < argc) {
          printf(" ");
        }
      }
      printf("\n");
      break;
    }
    case CMD_UNKNOWN:
    default:
      char path_buf[4096];
      if (find_in_path(argv[0], path_buf, sizeof(path_buf))) {
        int status = run_external(argv);
        if (status == -1) {
          printf("something went wrong");
        }
        break;
      } else {
        printf("%s: command not found\n", command);
      }
    }
  }

  return 0;
}
