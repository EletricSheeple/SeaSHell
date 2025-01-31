#include "../include/status.h"
#include "core.h"
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int expand_special(char *token, char *buffer[], int i);
int expand_variable(char *key, char *buffer[], int i);

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

int split(char *string, char *dest[], char *delim) {
  /* Split string into array of tokens */
  char *token;
  int count = 0;

  // Use strtok to get the first token
  token = strtok(string, delim);

  // Continue tokenizing until no more tokens are found
  while (token != NULL) {
    dest[count++] = token;
    token = strtok(NULL, delim);
  }

  return count;
}

int *get_type(char *buffer[]) {
  // Defines the type of a token
  static int return_list[] = {};

  const char SYMBOL_LIST[] = {'~', '$',  '*', '?', '&', ';',
                              '"', '\'', '`', '!', '>', '<'};

  const char *RESKEYWORD_LIST[] = {"exit", "export", "alias", "unalias"};

  for (int i = 0; buffer[i] != NULL; i++) {
    // String scope
    for (int ii = 0; buffer[ii] != 0x0; ii++) {
      // Char scope
      if (buffer[i][ii + 0] == 0x0) {
        // If single character, check for symbols
        for (int iii = 0; iii < sizeof(SYMBOL_LIST); iii++) {
          if ((buffer[i][ii]) == SYMBOL_LIST[iii]) {
            return_list[i] = SYMBOL;
          }
        }
      }
    }
  }

  return return_list;
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
    if (current_token[0] == '~') { // Word starts with ~
      expand_special(current_token, buffer, i);
    } else if (current_token[0] == '$') {
      expand_variable(current_token, buffer, i);
    }
  }

  return STAT_SUCCESS;
}

int expand_special(char *token, char *buffer[], int i) {
  struct passwd *pw = get_user_info();

  if (token[1] != '\0') {
    struct passwd *targetpw;
    char *username = token + 1; // Skip the '~'
    targetpw = getpwnam(username);

    if (targetpw != NULL) {
      buffer[i] = strdup(targetpw->pw_dir);
      if (buffer[i] == NULL) {
        return STAT_MEMALLOCERR;
      }
    } else {
      return 1;
    }
  } else {
    char *current_token = buffer[i];

    // Handle home directory expansion ~
    if (strcmp(current_token, "~") == 0) {
      char *homeDir = pw->pw_dir;
      buffer[i] = strdup(homeDir);
      if (buffer[i] == NULL) {
        return STAT_MEMALLOCERR;
      }
    }
  }

  return 0;
}

int expand_variable(char *key, char *buffer[], int i) {
  char *env_var = getenv(key + 1); // Skip the '$'
  if (env_var != NULL) {
    buffer[i] = strdup(env_var);
    if (buffer[i] == NULL) {
      return STAT_MEMALLOCERR;
    }
  }
  return 0;
}
