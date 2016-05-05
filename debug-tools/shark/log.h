#ifndef __LOG_H__
#define __LOG_H__

#define FREE(x) do {free (x); x = NULL;}while(0)

#define working_journal_log(dom, levl, fmt...) do {                                  \
                       _working_journal_log (dom, __FILE__, __FUNCTION__, __LINE__,  \
                                                                 levl, ##fmt);       \
                } while (0)

typedef enum {
        LOG_NONE,
        LOG_EMERG,
        LOG_ALERT,
        LOG_CRITICAL,   /* fatal errors */
        LOG_ERROR,      /* major failures (not necessarily fatal) */
        LOG_WARNING,    /* info about normal operation */
        LOG_NOTICE,
        LOG_INFO,       /* Normal information */
        LOG_DEBUG,      /* internal errors */
        LOG_TRACE,      /* full trace of operation */
}tools_loglevel_t;

int _asprintf (char **string_ptr, const char *format, ...);

int
_working_journal_log (const char *domain, const char *file, const char *function, int line,
                         tools_loglevel_t level, const char *fmt, ...);
#endif
