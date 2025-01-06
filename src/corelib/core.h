#ifndef _CORE_H_
#define _CORE_H_

/* Terminal handling functions */
/**
 * Switch terminal between raw and cooked input modes
 * @param mode 0 for raw mode, 1 for cooked mode
 * @return STAT_SUCCESS on success, -1 on failure
 */
int terminal_mode_switch(int mode);

/* String manipulation functions */
/**
 * Split a string into an array of tokens on spaces
 * @param buffer Input string to split
 * @param dest Array to store resulting tokens
 * @return Number of tokens found on success, -1 on error
 */
int split(char *buffer, char *dest[]);

/* Shell expansion functions */
/**
 * Expand shell variables and special characters in place
 * @param buffer Array of strings to expand
 * @return STAT_SUCCESS on success, -1 on error
 */
int expand(char *buffer[]);

/**
 * Get current user information from password database
 * @return Pointer to passwd struct on success, NULL on failure
 */
struct passwd *get_user_info(void);

/* Process management functions */
/**
 * Execute commands and manage processes
 * @param buffer Command string to execute
 * @return STAT_SUCCESS on success, error code on failure
 */
int proc_manager(char *buffer);

/* Display functions */
/**
 * Print shell prompt with formatting
 * @param type 0 for full prompt, 1 for simple prompt
 * @return STAT_SUCCESS on success, -1 on failure
 */
int print_handle(int type);

/* Command history functions */
/**
 * Initialize command history system
 * @return STAT_SUCCESS on success, -1 on failure
 */
int init_history(void);

/**
 * Add command to history
 * @param cmd Command string to add
 * @return STAT_SUCCESS on success, -1 on failure
 */
int add_history(const char *cmd);

/**
 * Retrieve command from history by index
 * @param index History entry index
 * @return Command string at index on success, NULL on failure
 */
char *get_history(int index);

/**
 * Free all history entries and cleanup
 * @return STAT_SUCCESS on success, -1 on failure
 */
int free_history(void);

typedef enum { INFO, ERROR, FAILURE, DEBUG } LogLevel;

void log_message(LogLevel level, const char *fmt, ...);

#endif // !_CORE_H_
