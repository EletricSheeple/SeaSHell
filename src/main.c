#include "corelib/core.h"
#include "include/control_seq.h"
#include "include/status.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int execution_loop(char *input_buffer, int input_buffer_size);

int main() {
  const int DEFAULT_INPUT_BUFFER_SIZE = 16;

  char *input_buffer;
  int input_buffer_size = DEFAULT_INPUT_BUFFER_SIZE;

  input_buffer = calloc(input_buffer_size, sizeof(char));
  if (input_buffer == NULL) {
    perror("calloc");
    return STAT_MEMALLOCERR;
  }
  terminal_mode_switch(0);

  init_history();
  execution_loop(input_buffer, input_buffer_size);

  terminal_mode_switch(1);
  free(input_buffer);
  return STAT_SUCCESS;
}

int execution_loop(char *input_buffer, int input_buffer_size) {
  const float DEFAULT_BUFFER_GROWTH_RATE = 1.25;
  const int DEFAULT_INPUT_BUFFER_SIZE = input_buffer_size;

  int buffer_index = 0;
  int cursor_index = buffer_index;
  int buffer_narg = 0;
  char current_char;
  bool display_handle = true;
  int history_index = -1; // -1 Represents current, 0 = 1st index

  while (1 == 1) {

    if (display_handle == true) {
      print_handle(0);
      fflush(stdout);
      display_handle = false;
    }

    if (read(STDIN_FILENO, &current_char, 1) == -1) {
      perror("read");
      break;
    }

    switch (current_char) {
    case ASCII_ETX: {
      // This does nothing at all, even tho it was supposed to...
      break;
    }
    case ASCII_CR: {
      break;
    }
    case ASCII_TAB: {
      break;
    }
    case ASCII_EOF: {
      break;
    }
    case ASCII_FF: {
      printf("\033c");
      fflush(stdout);
      display_handle = true;
      break;
    }
    case '\033': {
      // Check for escape sequence
      char seq[2];
      if (read(STDIN_FILENO, &seq[0], 1) != 1)
        break;
      if (read(STDIN_FILENO, &seq[1], 1) != 1)
        break;

      if (seq[0] == '[') {
        if (seq[1] == 'D') { // Left Arrow
          if (cursor_index > 0) {
            printf("\033[D"); // Move the cursor to the left
            --cursor_index;
          }
        }
        if (seq[1] == 'C') { // Right Arrow
          if (cursor_index < buffer_index) {
            printf("\033[C"); // Move the cursor to the right
            cursor_index++;
          }
        }

        if (seq[1] == 'A') { // Up arrow
          // Clear current line
          while (buffer_index > 0) {
            printf("\b \b");
            buffer_index--;
            fflush(stdout);
          }

          history_index++;

          char *cmd = get_history(history_index);
          if (cmd != NULL) {
            strncpy(input_buffer, cmd, input_buffer_size - 1);
            input_buffer[input_buffer_size - 1] = '\0';
            cursor_index = buffer_index = strlen(input_buffer);
            printf("%s", input_buffer);
          } else {
            history_index--;
          }
        } else if (seq[1] == 'B') { // Down arrow
          // Clear current line
          while (buffer_index > 0) {
            printf("\b \b");
            buffer_index--;
          }

          history_index--;
          if (history_index >= 0) {
            char *cmd = get_history(history_index);
            if (cmd != NULL) {
              strncpy(input_buffer, cmd, input_buffer_size - 1);
              input_buffer[input_buffer_size - 1] = '\0';
              buffer_index = strlen(input_buffer);
              printf("%s", input_buffer);
            }
          } else {
            // Clear line when going past most recent history
            memset(input_buffer, 0, input_buffer_size);
            buffer_index = 0;
            history_index = -1;
          }
        }
      }
      fflush(stdout);
      break;
    }
    case ASCII_DEL: {
      // Backspace
      if (buffer_index > 0) {
        printf("\b \b");
        fflush(stdout);

        // Deleting char from buffer
        input_buffer[--buffer_index] = '\x00';
      }

      break;
    }
    case ASCII_LF: {
      // Null terminate the input buffer
      input_buffer[buffer_index] = '\0';
      putchar('\n');
      fflush(stdout);

      // Only handle non-empty commands
      if (buffer_index > 0) {
        add_history(input_buffer);
        history_index = -1; // Reset history navigation
        // Process the command
        proc_manager(input_buffer);
      }

      // Reset buffer state
      memset(input_buffer, 0, input_buffer_size);
      cursor_index = buffer_index = 0;
      input_buffer_size = DEFAULT_INPUT_BUFFER_SIZE;
      display_handle = true;
      break;
    }

    default: {
      if (buffer_index >= input_buffer_size) {
        // Buffer is too small, will resize it now
        /* The > 0 check is needed because buffer_index is set to 0
         * after ASCII_LF.*/
        input_buffer_size =
            (int)(input_buffer_size * DEFAULT_BUFFER_GROWTH_RATE);
        // casting the result to int to avoid issues with realloc

        input_buffer = realloc(input_buffer, input_buffer_size);
        if (input_buffer == NULL) {
          perror("realloc");
          break;
        }
      }

      input_buffer[buffer_index++] = current_char;
      cursor_index = buffer_index;

      putchar(current_char);
      fflush(stdout);
      break;
    }
    }
  }

  return STAT_SUCCESS;
}
