#ifndef __THREAD_MANAGER_H
#define __THREAD_MANAGER_H
#include <pthread.h>
 
#include "shark_posix.h"


typedef enum _manager_state{

	survive = 0,
	destroy,
	will_destroy,
	have_destroy,
	notice_other,
	all_destroy,
	will_continue,
}manager_state;



typedef int  (*process)(void *arg,const char *path,int *ret,int *tmp_errno);



typedef enum _error_dispose{

        all_stop = 0,
        limit_error,
        all_continue
}error_dispose;
/*
typedef enum _worker_type{

        disk_type = 0,
        digioceand_type,
        check_type
}worker_type;
*/

typedef struct  _error_path_list{

        char                    *path;
        unsigned int            group_index;
        unsigned int            op_list_index;
        unsigned int            op_index;
	int			ret;
	int			tmp_errno;
        struct  list_head       list;
}error_path_list;

typedef struct _error_list{

	struct	list_head	digi_list;
	struct	list_head	disk_list;
	int			digisum;
	int			disksum;
	pthread_mutex_t         mutex;

	int			disk_have_destroy;
	int			disk_min_group;
	int			disk_min_op_list;
	int			disk_min_op;

	int			digi_have_destroy;
	unsigned int		digi_min_group;
	unsigned int		digi_min_op_list;
	unsigned int		digi_min_op;
}error_list;

typedef struct _worker{

        process                 fn;
        void                    *args;
        struct  list_head       list;
        char                    *abs_path;
        int                     ret;
        int                     tmp_errno;
        unsigned int            group_index;
        unsigned int            op_list_index;
        unsigned int            op_index;
	int			op_type;
        worker_type             type;
	int             	file_type;  
}worker_list;


typedef struct  _thread_pool{

        char                    *path_mount;
        journal_file_group_t	*group;

        struct  list_head       worker_list;
        unsigned int            worker_size;
        int                     thread_size;
        pthread_t               *thread;
        pthread_mutex_t         mutex;
        pthread_cond_t          cond;

        int                     group_read;
        unsigned int            perform_sum;
	int			building;
	unsigned int		read_index;

	unsigned int 		group_index;
	unsigned int 		op_list_index;

	error_dispose           error_type;
	worker_type             type;
	unsigned int		check_total_file;
        int                     destroy;

}thread_pool;

typedef struct  _thread_manager{

        thread_pool             *disk_pool;
        thread_pool             *digi_pool;
        int                     pool_sum;
        int                     thread_sum;
        char                    mask[2];
	char			normal[2];
        pthread_mutex_t         mutex;

	unsigned int		group_index;
	unsigned int		op_list_index;

	unsigned int		destroy_group;
	unsigned int		destroy_op_list;
        int                     destroy;
}thread_manager;

char *copy_str(const char *path, int n);
void get_configuration();


int free_error(error_path_list *date);
void free_error_list();
void error_init();

int free_oper_worker(worker_list *worker);
int free_oper_list(thread_pool *pool);
int pool_destroy(thread_pool *pool);
thread_pool *pool_init(const char *mount_path, worker_type type);
void manager_destroy();
void manager_init();
worker_list *oper_malloc(worker_type type, process pro, journal_file_op_list_t *args, thread_pool *pool, int group_index);
int build_worker_queue(thread_pool *pool);
worker_list *pool_oper_worker(thread_pool *pool);
int update_disk_min_error(worker_list *worker, error_path_list *date, int error_num);
int update_digi_min_error(worker_list *worker, error_path_list *date, int error_sum);
int file_add_error_list(worker_list *worker, int error_num);
int pool_oper_tolerate(thread_pool *pool, worker_list *worker);
int pool_oper_increase(thread_pool *pool, worker_list *worker);
void *oper_thread(void  *date);




manager_state check_pool_state();
manager_state check_pool_destroy();
manager_state check_pool_will_destroy();
manager_state check_pool_have_destroy(int  *which);
manager_state check_pool_notice_destroy(int which);
int check_pool_all_destroy();
int get_manager_state(manager_state state);

#endif
