#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "log.h"
#include "analyze.h"
#include "journal.h"

int _asprintf (char **string_ptr, const char *format, ...)
{
        va_list arg;
        int     rv = 0;

        va_start (arg, format);

        rv = vasprintf (string_ptr, format, arg);

        va_end (arg);

        return rv;
}

int
_working_journal_log (const char *domain, const char *file, const char *function, int line,
                         tools_loglevel_t level, const char *fmt, ...)
{
        char                timestr[128] = {0,};
        char                string[PATH_MAX] = {0,};
        va_list             ap;
        time_t              now = {0,};
        char                *str1 = NULL;
        char                *str2 = NULL;
        char                *msg  = NULL;
        struct tm           *tm_now = NULL;
        const char          *basename = NULL;
        size_t              len  = 0;
        int                 ret  = 0;
        
        if (level > cgv.log_level || level == LOG_NONE)
                goto out;

        static char *level_strings[] = {"",  /* NONE */
                                        "M", /* EMERGENCY */
                                        "A", /* ALERT */
                                        "C", /* CRITICAL */
                                        "E", /* ERROR */
                                        "W", /* WARNING */
                                        "N", /* NOTICE */
                                        "I", /* INFO */
                                        "D", /* DEBUG */
                                        "T", /* TRACE */
                                        ""};

        if (!domain || !file || !function || !fmt) {
                fprintf (stderr,
                         "logging: %s:%s():%d: invalid argument\n",
                         __FILE__, __PRETTY_FUNCTION__, __LINE__);
                return -1;
        }

        basename = strrchr (file, '/');
        if (basename)
                basename++;
        else
                basename = file;

        pthread_mutex_lock (&gjournal.log_lock);
        {
                if (!gjournal.working_journal_fd) {
                        memset (string, 0, sizeof (string));
                        sprintf (string, "%s/working_journal.log",cgv.working_directory);

                        if (cgv.first_open_for_log == 1) {
                                gjournal.working_journal_fd = fopen (string, "w+");
                                cgv.first_open_for_log = 0;
                        } else {
                                gjournal.working_journal_fd = fopen (string, "a+");
                        }
                }
        }
        pthread_mutex_unlock (&gjournal.log_lock);

        now = time (NULL);
        if (now == ((time_t) -1))
                goto out;
        
        tm_now = localtime (&now);
        if (tm_now == NULL)
                goto out;

        va_start (ap, fmt);

        sprintf(timestr, "%d-%02d-%02d %02d:%02d:%02d",tm_now->tm_year+1900, tm_now->tm_mon+1,
                        tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);

        ret =  _asprintf (&str1, "[%s] %s [%s:%d:%s] %s: ", timestr, level_strings[level],
                        basename, line, function,
                        domain);
        if (ret == -1)
                goto err;

        ret = vasprintf (&str2, fmt, ap);
        if (ret == -1)
                goto err;

        va_end (ap);

        len = strlen (str1);
        msg = malloc (len + strlen (str2) + 1);
        if (!msg)
                goto err;

       strcpy (msg, str1);
       strcpy (msg + len, str2);

       pthread_mutex_lock (&gjournal.log_lock);
       {
               if (gjournal.working_journal_fd) {
                    fprintf (gjournal.working_journal_fd, "%s\n", msg);
                    fflush (gjournal.working_journal_fd);
               } else if (cgv.log_level >= level) {
                    fprintf (stderr, "%s\n", msg);
                    fflush (stderr);
               }
       }
       pthread_mutex_unlock (&gjournal.log_lock);
err:
       FREE(msg);
       FREE(str1);
       FREE(str2);
out:
        return 0;
}
