#include "../include/status.h"
#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int proc_manager(char *buffer) {
  /* Process manager for command execution
   * Takes a command string and handles execution via fork/exec
   * Handles built-in commands like cd and exit directly
   * For external commands, forks and executes in child process
   *
   * Params:
   *   buffer: Null-terminated command string to execute
   * Returns:
   *   STAT_SUCCESS on successful execution
   *   STAT_CHDIRERR on cd command failure
   *   STAT_FORKERR on fork failure
   *   STAT_EXECERR on exec failure */

  int word_count = 0;
  fflush(stdout);

  for (int i = 0; buffer[i] != '\x00'; i++) {
    // Getting the number of words by counting N of spaces
    if (buffer[i] == ' ') {
      word_count++;
    }
  }

  char *fields[word_count + 1];

  split(buffer, fields, " \t\n");

  fields[word_count + 1] = NULL;

  expand(fields);

  if (strcmp(fields[0], "exit") == 0) {
    return STAT_VEXIT;
  } else if (strcmp(fields[0], "cd") == 0) {
    if (fields[1] == NULL) {
      char *tmp[2] = {"~", NULL};
      expand(tmp);
      chdir(tmp[0]);
      return STAT_SUCCESS;
    }

    if (chdir(fields[1]) != 0) {
      perror("cd");
      return STAT_CHDIRERR;
    }
    return STAT_SUCCESS;
  } else if (strcmp(fields[0], "export") == 0) {
    // Set an env var
    if (fields[2] != NULL) {
      // export KEY value
      setenv(fields[1], fields[2], 1);
    } else {
      // export KEY=value
      char *tmp[2]; // key and value
      split(fields[1], tmp, "=");

      setenv(tmp[0], tmp[1], 1);
    }
    return STAT_SUCCESS;
  }

  pid_t pid = fork();
  if (pid < 0) {
    // Fork failed
    perror("Fork");
    return STAT_FORKERR;
  } else if (pid > 0) {
    // Parent process
    wait(NULL); // Wait for the child process to finish
  } else {
    // Child process
    execvp(fields[0], fields); // Execute the process with arguments
    // If execvp returns, it must have failed
    perror("execvp");
    exit(STAT_EXECERR);
  }

  memset(fields, '\x00', word_count);
  return STAT_SUCCESS;
}
