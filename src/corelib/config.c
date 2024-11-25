#include <stdio.h>
#include "../include/status.h"

int init_configuration(char *filepath) {
  
  // Configuration path
  FILE *fp;

  if ((fp = fopen(filepath, "a+")) == NULL) {
    perror("fopen");
    return -999;
  }

  /* Proposition of config: 
   *
   * # `set` keyword for configuration options
   * set history_path=path
   * set prompt_type=0
   *
   * # Aliases
   * alias key="pair"
   *
   * # Environment variables
   * export VAR=result
   *
   * # Comment
   *
   */

  return STAT_SUCCESS;
}
