#include "core.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

const char *error_messages[] = {
    "Success", // STAT_SUCCESS

    // Memory and resource errors
    "Failed to allocate memory",  // STAT_MEMALLOCERR
    "Failed to change directory", // STAT_CHDIRERR

    // Process and execution errors
    "Failed to fork process",    // STAT_FORKERR
    "Command execution failure", // STAT_EXECERR

    // Terminal handling errors
    "Terminal attribute error", // STAT_TERMERR
    "I/O operation error",      // STAT_IOERR

    // Command processing errors
    "Invalid command format",   // STAT_CMDINVALID
    "Command not found",        // STAT_CMDNOTFOUND
    "Variable expansion error", // STAT_EXPANSIONERR

    // History errors
    "History operation failure" // STAT_HISTORYERR
};

static FILE *log_file = NULL;

/**
 * @brief Opens the log file for appending. The log file is used by both
 *        `log_error()` and `log_message()` functions.
 */
void open_log_file() {
  if (!log_file) {
    log_file = fopen("log.txt", "a");
    if (!log_file) {
      perror("Failed to open log file");
    }
  }
}

/**
 * @brief Closes the log file if it is open.
 *        This should be called when the program finishes logging.
 */
void close_log_file() {
  if (log_file) {
    fclose(log_file);
    log_file = NULL;
  }
}

/**
 * @brief Logs an error message to the log file using a predefined error
 *        message based on the provided error code. The log file and
 *        timestamp are also included in the log entry.
 *
 * @param error_code The error code indicating the specific error.
 *                   It should be a value between 0 and 10 (inclusive).
 */
void log_error(int error_code) {
  // Check for valid error code (0 - 10)
  if (error_code < 0 || error_code > 10) {
    return; // Invalid error code, do not log
  }

  open_log_file(); // Ensure the log file is opened

  if (!log_file) {
    return; // Failed to open log file, do not proceed
  }

  // Get the current timestamp
  time_t now = time(NULL);
  struct tm *timestamp = localtime(&now);
  char timestamp_str[64];
  strftime(timestamp_str, sizeof(timestamp_str), "[%Y-%m-%d %H:%M:%S]",
           timestamp);

  // Log the error with the predefined message to file
  fprintf(log_file, "%s: %s (Error Code: %d): %s\n", timestamp_str, __FILE__,
          error_code, error_messages[error_code]);
  fflush(log_file); // Ensure the log is written immediately

  // Also print it to stderr (only for errors)
  fprintf(stderr, "%s: %s (Error Code: %d): %s\n", timestamp_str, __FILE__,
          error_code, error_messages[error_code]);
}

/**
 * @brief Logs a custom message to the log file. The message is formatted
 *        based on the provided log level and custom string format.
 *
 * @param level The log level (e.g., INFO, ERROR, FAILURE, DEBUG).
 * @param fmt The format string for the custom log message.
 *            This can include placeholders for additional arguments.
 * @param ... Additional arguments for formatting the log message.
 */
void log_message(LogLevel level, const char *fmt, ...) {
  open_log_file(); // Ensure the log file is opened

  if (!log_file) {
    return; // Failed to open log file, do not proceed
  }

  // Define log level strings
  const char *level_str[] = {
      "\x1b[34m[INFO]",    // INFO - Blue
      "\x1b[33m[ERROR]",   // ERROR - Yellow
      "\x1b[31m[Failure]", // FAILURE - Red
      "\x1b[35m[Debug]"    // DEBUG - Magenta
  };

  // Get the current timestamp
  time_t now = time(NULL);
  struct tm *timestamp = localtime(&now);
  char timestamp_str[64];
  strftime(timestamp_str, sizeof(timestamp_str), "[%Y-%m-%d %H:%M:%S]",
           timestamp);

  // Prepare the log message
  va_list args;
  va_start(args, fmt);

  // INFO and DEBUG: Print only to stdout (no file logging)
  if (level == INFO || level == DEBUG) {
    fprintf(stdout, "%s%s: %s\x1b[0m ", timestamp_str, level_str[level],
            __FILE__);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
  }
  // ERROR and FAILURE: Print to stderr and also log to the file
  else if (level == ERROR || level == FAILURE) {
    fprintf(stderr, "%s%s: %s\x1b[0m ", timestamp_str, level_str[level],
            __FILE__);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    // Log to file 
    fprintf(log_file, "%s: %s: ", timestamp_str, level_str[level]);
    vfprintf(log_file, fmt, args);
    fprintf(log_file, "\n");
  }

  va_end(args);

  fflush(log_file); // Ensure the log is written immediately
}
