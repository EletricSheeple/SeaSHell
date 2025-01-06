#include "corelib/core.h"
#include "include/control_seq.h"
#include "include/status.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int execution_loop(char *input_buffer, int input_buffer_size);

void insert_char(char *str, char ch, int pos);

void delete_char(char *str, int pos);

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
  int cursor_index = 0;
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
      putchar(ASCII_LF);
      return STAT_SUCCESS;
    }
    case ASCII_CR: {
      break;
    }
    case ASCII_TAB: {
      break;
    }
    case ASCII_EOF: {
      return STAT_SUCCESS;
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
          if (cursor_index < buffer_index) {
            printf("\033[D"); // Move the cursor to the left
            cursor_index++;
          }
        }
        if (seq[1] == 'C') { // Right Arrow
          if (cursor_index > 0) {
            printf("\033[C"); // Move the cursor to the right
            cursor_index--;
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

          while (input_buffer_size <= strlen(cmd)) {
            input_buffer_size =
                (int)(input_buffer_size * DEFAULT_BUFFER_GROWTH_RATE);

            char *tmp = realloc(input_buffer, input_buffer_size);
            if (tmp == NULL) {
              perror("realloc");
              free(input_buffer);
              return STAT_MEMALLOCERR;
            }

            input_buffer = tmp;
          }

          if (cmd != NULL) {
            strncpy(input_buffer, cmd, input_buffer_size - 1);
            input_buffer[input_buffer_size - 1] = '\0';
            buffer_index = strlen(input_buffer);
            cursor_index = 0;
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

            while (input_buffer_size <= strlen(cmd)) {
              input_buffer_size =
                  (int)(input_buffer_size * DEFAULT_BUFFER_GROWTH_RATE);

              char *tmp = realloc(input_buffer, input_buffer_size);
              if (tmp == NULL) {
                perror("realloc");
                free(input_buffer);
                return STAT_MEMALLOCERR;
              }

              input_buffer = tmp;
            }

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
        delete_char(input_buffer, ((buffer_index - cursor_index) - 1));
        buffer_index--;
      }
      printf("\r\033[2C\033[K%s", input_buffer);
      if (cursor_index > 0) {
        printf("\033[%dD", cursor_index);
      }
      fflush(stdout);

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
        input_buffer_size =
            (int)(input_buffer_size * DEFAULT_BUFFER_GROWTH_RATE);
        // casting the result to int to avoid issues with realloc

        char *temp = realloc(input_buffer, input_buffer_size);
        if (temp == NULL) {
          perror("realloc");
          free(input_buffer);
          return STAT_MEMALLOCERR;
        }

        input_buffer = temp;
      }

      // Dynamically insert the character into the string
      insert_char(input_buffer, current_char,
                  ((buffer_index++) - cursor_index));
      // input_buffer[(buffer_index++) - cursor_index] = current_char;

      /* Moving the cursor to the start of the string
       * Then moving it twice to the left (to avoid overwriting the prompt
       * string)
       * Print the input buffer Moving the cursor back to it's
       * original position */
      // TODO: Remove the hardcoding of the prompt length
      printf("\r\033[2C%s", input_buffer);
      if (cursor_index > 0) {
        printf("\033[%dD", cursor_index);
      }
      fflush(stdout);
      break;
    }
    }
  }

  return STAT_SUCCESS;
}

void insert_char(char *str, char ch, int pos) {
  int len = strlen(str);

  // Ensure the position is valid
  if (pos < 0 || pos > len) {
    printf("Invalid position!\n");
    return;
  }

  // Shift characters to the right to make space for the new character
  for (int i = len; i >= pos; i--) {
    str[i + 1] = str[i]; // Move each character one position to the right
  }

  // Insert the new character at the specified position
  str[pos] = ch;

  // Null-terminate the string
  str[len + 1] = '\0';
}

void delete_char(char *str, int pos) {
  int len = strlen(str);

  // Ensure the position is valid
  if (pos < 0 || pos >= len) {
    return;
  }

  /* Shift characters to the left starting from the position after the one to be
   * deleted */
  for (int i = pos; i < len; i++) {
    str[i] = str[i + 1]; // Move each character one position to the left
  }
}
