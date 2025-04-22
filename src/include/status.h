#ifndef _STATUS_H_
#define _STATUS_H_

/* Success status */
#define STAT_SUCCESS 0
#define STAT_VEXIT 1           /* Virtual exit, nothing is wrong, but shell should exit */

/* Memory and resource errors */
#define STAT_MEMALLOCERR -1    /* Memory allocation failure */
#define STAT_CHDIRERR -2       /* Directory change failure */

/* Process and execution errors */
#define STAT_FORKERR -3        /* Process fork failure */
#define STAT_EXECERR -4        /* Command execution failure */

/* Terminal handling errors */
#define STAT_TERMERR -6        /* Terminal attribute error */
#define STAT_IOERR -7          /* I/O operation error */

/* Command processing errors */
#define STAT_CMDINVALID -8     /* Invalid command format */
#define STAT_CMDNOTFOUND -9    /* Command not found */
#define STAT_EXPANSIONERR -10  /* Variable expansion error */

/* History errors */
#define STAT_HISTORYERR -11    /* History operation failure */

#endif // !_STATUS_H_
