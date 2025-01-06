#include "../include/status.h"
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct passwd *get_user_info() {
  /* Get current user information */
  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);

  if (pw == NULL) {
    perror("getpwuid");
    return NULL;
  }

  return pw;
}

int split(char *string, char *dest[]) {
  /* Split string into array of tokens */
  char *token;
  int count = 0;

  // Use strtok to get the first token
  token = strtok(string, " \t\n");

  // Continue tokenizing until no more tokens are found
  while (token != NULL) {
    dest[count++] = token;
    token = strtok(NULL, " \t\n");
  }

  return count;
}

int expand(char *buffer[]) {
  /* Expand shell variables and special characters */
  struct passwd *pw = get_user_info();
  if (pw == NULL) {
    return STAT_EXPANSIONERR;
  }

  for (int i = 0; buffer[i] != NULL; i++) {
    char *current_token = buffer[i];

    // Handle home directory expansion ~
    if (strcmp(current_token, "~") == 0) {
      char *homeDir = pw->pw_dir;
      buffer[i] = strdup(homeDir);
      if (buffer[i] == NULL) {
        return STAT_MEMALLOCERR;
      }
    }
    // Handle ~user expansion
    else if (current_token[0] == '~' && current_token[1] != '\0') {
      struct passwd *targetpw;
      char *username = current_token + 1; // Skip the '~'
      targetpw = getpwnam(username);

      if (targetpw != NULL) {
        buffer[i] = strdup(targetpw->pw_dir);
        if (buffer[i] == NULL) {
          return STAT_MEMALLOCERR;
        }
      }
    }
    // Handle general environment variable expansion like $VAR
    else if (current_token[0] == '$') {
      char *env_var = getenv(current_token + 1); // Skip the '$'
      if (env_var != NULL) {
        buffer[i] = strdup(env_var);
        if (buffer[i] == NULL) {
          return STAT_MEMALLOCERR;
        }
      }
    }
  }

  return STAT_SUCCESS;
}
