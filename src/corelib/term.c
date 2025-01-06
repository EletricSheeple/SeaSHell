#include "../include/status.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int terminal_mode_switch(int mode) {
  /* Switch terminal input mode between raw and cooked
   * Controls how terminal input is processed:
   *   Raw mode (0): Input is processed character by character without echo
   *   Cooked mode (1): Input is processed line by line with echo
   *
   * Raw mode is useful for reading single keystrokes without waiting for Enter
   * Cooked mode is the normal terminal mode with line buffering
   *
   * Mode settings:
   *   Raw (0):
   *     - Disables canonical mode (no line buffering)
   *     - Disables echo
   *     - Sets minimum input to 1 character
   *     - No input timeout
   *   Cooked (1):
   *     - Enables canonical mode
   *     - Enables echo
   *
   * Params:
   *   mode: Integer specifying mode (0=raw, 1=cooked)
   * Returns:
   *   STAT_SUCCESS on success
   *   STAT_TERMERR on terminal attribute error
   *   STAT_IOERR on invalid mode */

  struct termios tty;

  // Get the current terminal attributes
  if (tcgetattr(STDIN_FILENO, &tty) < 0) {
    perror("tcgetattr");
    return STAT_TERMERR;
  }

  switch (mode) {
  case 0: // RAW mode
    tty.c_lflag &= ~(ICANON | ECHO |
                     ISIG); // Disable canonical mode, signal handling and echo
    tty.c_cc[VMIN] = 1;     // Minimum number of characters to read
    tty.c_cc[VTIME] = 0;    // No timeout
    break;

  case 1:                                  // COOKED mode
    tty.c_lflag |= (ICANON | ECHO | ISIG); // Enable canonical mode and echo
    break;

  default:
    fprintf(stderr, "Invalid mode. Use 0 for RAW and 1 for COOKED.\n");
    return STAT_IOERR;
  }

  // Set the new attributes
  if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) < 0) {
    perror("tcsetattr");
    return STAT_TERMERR;
  }

  return STAT_SUCCESS;
}
