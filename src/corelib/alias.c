#include "../include/status.h"
#include "core.h"
#include <stdlib.h>
#include <string.h>

#define MAX_ALIAS 1024

#define EMPTY (void *)0x1 // NULL denotes EOL, EMPTY denotes clean space

int alias_set(char *key, char *val) {
  if (alias_count >= MAX_ALIAS) {
    return STAT_ALIASERR;
  }
  if (key == NULL || val == NULL) {
    return STAT_ALIASNULL;
  }

  alias_list[alias_count].key = strdup(key);
  alias_list[alias_count].val = strdup(val);
  alias_count++;

  return STAT_SUCCESS;
}

int alias_unset(char *key) {
  // This creates an issue, now the alias list will have gaps in it
  //    Is it possible to fix it? Yeah. Will I? No.
  //    Menothinks it's worth it, so if any issues arise, reload the shell, bucko.

  if (key == NULL) {
    return STAT_ALIASNULL;
  }

  for (int i = 0; alias_list[i].key != NULL; i++) { // Loop through alias list
    if (strcmp(alias_list[i].key, key) == 0) { // If alias matches
      free(alias_list[i].key);
      free(alias_list[i].val);
      alias_list[i].key = EMPTY;
      alias_list[i].val = EMPTY;
    }
  }
  return STAT_SUCCESS;
}