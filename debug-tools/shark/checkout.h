#ifndef __CHECKOUT_H__
#define __CHECKOUT_H__

#include "thread_manager.h"


extern int    conf_thread_size;
/*False data for testing*/
extern  char  digioceand_mount[1024] ;
extern  char  disk_mount[1024] ;
/*False data for testing*/


typedef enum _checkout_state{

        check_is_direc = 0,
        check_is_file,
        check_is_symlink
}checkout_state;

typedef struct _checkout_date{

        checkout_state                type;
        struct list_head              list;
        char                          *check_path;
        char                          *disk_path;
        struct stat                   stats[2];
        struct list_head              check_list;
        struct list_head              disk_list;
/*        char                          *check_md5;
        char                          *disk_md5;
*/
}checkout_date;

typedef struct _xattr_link{

        char                *key;
        char                *value;
        struct list_head    list;
}xattr_link;


/*---------check_thread-----------*/


void checkout_pool_destroy();
void check_pool_init(const char *mount_path, worker_type type);
int free_check(checkout_date *date);
int free_check_worker(worker_list *worker);
int free_check_list();
checkout_date *check_malloc(const char *check_path,const char *disk_path, checkout_state status);
worker_list *check_worker_malloc(worker_type type, process pro, void *args);
worker_list *pool_check_worker();
int pool_check_change(worker_list *worker);
void check_thread(void);


/*---------check_thread----------*/

extern char *copy_str(const char *path, int n);

/*---------check-----------------*/
int free_check(checkout_date   *date);
checkout_date *check_malloc(const char *check_path, const char *test_path, checkout_state state);
int check_stat_compare(struct stat buf1, struct stat buf2);
xattr_link *xattr_malloc(size_t key_len, size_t value);
int xattr_get(checkout_date *date, int choose , int  *sum);
int check_xattr_get(checkout_date *date);
int check_xattr_compare(checkout_date *date);
int traversal_mount(const char *path, int depth);
void check_all(void);
/*---------check_thread----------*/

int check_op_direc(void *args,const char *path, int *ret, int *tmp_errno);
int  check_op_file(void *args, const char *path, int *ret, int *tmp_errno);
int  check_op_symlink(void *args, const char *path, int *ret, int *tmp_errno);


#endif
