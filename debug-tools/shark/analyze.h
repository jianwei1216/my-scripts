#ifndef __ANALYZE_H__
#define __ANALYZE_H__
#include <stdio.h>
#include <assert.h>
#include "shark_ops.h"

#define LOCK_INIT(x)            pthread_mutex_init (x, 0)
#define LOCK(x)                 pthread_mutex_lock (x)
#define UNLOCK(x)               pthread_mutex_unlock (x)
#define LOCK_DESTROY(x)         pthread_mutex_destroy (x)

#define COND_LOCK_INIT(x)       pthread_cond_init(x,0)
#define COND_WAIT(x,y)          pthread_cond_wait(x,y)
#define COND_SIGNAL(x)          pthread_cond_signal(x)
#define COND_LOCK_DESTROY(x)    pthread_cond_destroy(x)

#define SHARK_DEBUG
#ifdef SHARK_DEBUG
#define SHARK_ASSERT(x) assert(x)
#else
#define EC_EXPAND_ASSERT(x)                                             \
                do {                                                    \
                   } while (0)
#endif

#define HANDLE_ERROR_EN(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define HANDLE_ERROR(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define LIST_STRING "*************************op_list_num=%d *************************\n"
#define GROUP_STRING "#########################group_num=%d #########################\n"

typedef struct {
        int                 thread_shutdown;
        int                 disk_cur_offset;
        int                 cluster_cur_offset;
        int                 ops_group_disk_num;
        int                 ops_group_cluster_num;
        pthread_t           thread_num;
        pthread_mutex_t     write_lock;
        pthread_mutex_t     read_lock;
        pthread_mutex_t     log_lock;
        pthread_cond_t      write_cond_lock;
        struct list_head    group_queue;
        FILE                *working_journal_fd;
        FILE                *execute_journal_fd;
        FILE                *execute_error_journal_fd;
        FILE                *check_journal_fd;
}global_journal_t;

extern global_journal_t gjournal;

typedef struct {
        struct list_head        group_head;
        int                     opsgroups_index;
}journal_file_group_t;

typedef struct {
        char                path[PATH_MAX];
        struct list_head    op_list_head;
        struct list_head    op_group;
        int                 is_need_open;
        int                 file_type;
        int                 is_new;
        int                 op_type;
        int                 ops_index;
        int                 op_list_type;

/*-------------linshiyue---------------*/
	int		    perform_op_index;
/*-----------add for perform  op  count*/
}journal_file_op_list_t;

typedef enum _worker_type{

        disk_type = 0,
        digioceand_type,
        check_type
}worker_type;

extern global_journal_t gjournal;

//extern void free_journal_group_mem (journal_file_group_t *group);
extern void gjournal_procedure_destroy ();
extern int result_handle_fn (char *buf);
extern int check_handle_fn (char *buf);
extern int cut_string_operation (file_op_t *op, char *line, int op_type);
extern int analyze_string_write (file_ops_group_t *group,
                            file_ops_t *op_list, file_op_t *op);
extern journal_file_group_t *analyze_string_read (worker_type type);
extern int handle_groups (file_ops_group_t *g);
extern void *thread_writev (void *arg);
extern void gjournal_init (global_journal_t *gjournal);
extern void free_journal_group_mem (journal_file_group_t *group);
extern void free_journal_op_list_mem (journal_file_op_list_t *op_list);
extern void free_journal_op_mem (file_op_t *op);

#endif
