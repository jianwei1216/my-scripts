/*
 *
 */
#ifndef __SHARK_OPS_H
#define __SHARK_OPS_H

#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>
#include "list.h"
#include <limits.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "shark_file.h"


#if 0
typedef struct {
        double      dir_op_ratio; /* dir 操作概率 */ 
        double      symbol_link_op_ratio; /* symlink 操作概率 */ 
        double      file_op_ratio; /* file 操作概率 */

        int         accuracy;
        int         accuracy_num;
        int         log_level;
        int         write_size;
        int         read_size;

        int         dir_max_depth;
        int         initial_dir_cnts;
        int         initial_file_cnts;
        int         initial_symlink_counts;
        int         total_op_group;

        char        test_dir_path[PATH_MAX];
        char        validation_dir_path[PATH_MAX];
        char        working_directory[PATH_MAX];

        int         working_threads;
        int         operations_waiting_list;
        int         operations_of_opened_file; 
        int         operations_of_unopened_file;
        int         operations_of_opened_dir;
        int         operations_of_unopened_dir; 
        int         operations_of_symlink;

        double      new_dir_ratio;
        double      open_dir_ratio;

        double      new_file_ratio;
        double      open_file_ratio;

        double      fsync_open_dir;
        double      fsetxattr_open_dir;
        double      fgetxattr_open_dir;
        double      fremovexattr_open_dir;
        double      readdir_open_dir;
        double      lk_open_dir;
        double      fchmod_open_dir;
        double      fchown_open_dir;
        double      futimes_open_dir;
        double      fstat_open_dir;

        double      access_unopen_dir;
        double      rename_unopen_dir;
        double      symlink_unopen_dir;
        double      setxattr_unopen_dir;
        double      getxattr_unopen_dir;
        double      removexattr_unopen_dir;
        double      chown_unopen_dir;
        double      chmod_unopen_dir;
        double      utimes_unopen_dir;
        double      rmdir_unopen_dir;
        double      stat_unopen_dir;
        double      statfs_unopen_dir;

        double      writev_open_file;
        double      readv_open_file;
        double      fallocate_open_file;
        double      ftruncate_open_file;
        double      fsync_open_file;
        double      fsetxattr_open_file;
        double      fgetxattr_open_file;
        double      fremovexattr_open_file;
        double      lk_open_file;
        double      fchown_open_file;
        double      fchmod_open_file;
        double      futimes_open_file;
        double      fstat_open_file;

        double      rename_unopen_file;
        double      symlink_unopen_file;
        double      link_unopen_file;
        double      truncate_unopen_file;
        double      setxattr_unopen_file;
        double      getxattr_unopen_file;
        double      removexattr_unopen_file;
        double      access_unopen_file;
        double      chown_unopen_file;
        double      chmod_unopen_file;
        double      utimes_unopen_file;
        double      stat_unopen_file;
        double      unlink_unopen_file;
        double      statfs_unopen_file;

        double      readlink_symbol_link;
        double      unlink_symbol_link;

}config_globle_var;
#endif

#define XATTR_KEY_STR           "user.xattr-key-XXXXXX"
#define XATTR_KEY_LEN           (21+1)
#define XATTR_VAL_STR           "xattr-val-XXXXXX"
#define XATTR_VAL_LEN           (16+1)
int total_ops_cnt;

typedef enum {
        OPENED_DIR_OP,
        NO_OPEN_DIR_OP,
        OPENED_FILE_OP,
        NO_OPEN_FILE_OP,
        SYMLINK_OP
}op_type_t;

typedef enum {
        O_UNKNOWN_DIR_OP,
        FSYNC_DIR_OP,/* ==1*/
        FSETXATTR_DIR_OP,
        FGETXATTR_DIR_OP,
        FREMOVEXATTR_DIR_OP,
        READDIR_DIR_OP,
        /*
        FSETATTR_DIR_OP,
        */
        FCHOWN_DIR_OP,
        FCHMOD_DIR_OP,
        FUTIMES_DIR_OP,
        FSTAT_DIR_OP,
        O_MAX_DIR_OP
} opendir_needed_ops_t;

typedef enum {
        NO_UNKNOWN_DIR_OP,
        ACCESS_DIR_OP,
        RENAME_DIR_OP,
        SYMLINK_DIR_OP,/* ==2 */
        SETXATTR_DIR_OP,
        GETXATTR_DIR_OP,
        REMOVEXATTR_DIR_OP,
        /*
        SETATTR_DIR_OP,
        */
        CHOWN_DIR_OP,
        CHMOD_DIR_OP,
        UTIMES_DIR_OP,
        RMDIR_DIR_OP,
        STAT_DIR_OP,
        STATFS_DIR_OP,
        NO_MAX_DIR_OP
} opendir_noneed_ops_t;

typedef enum {
        O_UNKNOWN_FILE_OP,
        WRITEV_FILE_OP,/* ==1 */
        READV_FILE_OP,
        FALLOCATE_FILE_OP,
        FTRUNCATE_FILE_OP,
        FSYNC_FILE_OP,
        FSETXATTR_FILE_OP,
        FGETXATTR_FILE_OP,
        FREMOVEXATTR_FILE_OP,
        LK_FILE_OP,
        /*
        FSETATTR_FILE_OP,
        */
        FCHOWN_FILE_OP,
        FCHMOD_FILE_OP,
        FUTIMES_FILE_OP,
        FSTAT_FILE_OP,
        O_MAX_FILE_OP
} open_needed_ops_t;

typedef enum {
        NO_UNKNOW_FILE_OP,
        RENAME_FILE_OP,/* == */
        SYMLINK_FILE_OP,
        LINK_FILE_OP,
        TRUNCATE_FILE_OP,
        SETXATTR_FILE_OP,
        GETXATTR_FILE_OP,
        REMOVEXATTR_FILE_OP,
        ACCESS_FILE_OP,
        /*
        SETATTR_FILE_OP,
        */
        CHOWN_FILE_OP,
        CHMOD_FILE_OP,
        UTIMES_FILE_OP,
        STAT_FILE_OP,
        UNLINK_FILE_OP,
        STATFS_FILE_OP,
        NO_MAX_FILE_OP
} open_noneed_ops_t;

typedef enum {
        UNKNOWN_SYMLINK_OP,
        READLINK_SYMLINK_OP,
        UNLINK_SYMLINK_OP,
        MAX_SYMLINK_OP
} symlink_ops_t;

typedef struct {
        struct list_head        oplist;

        int                     op;
        int                     op_index;
        
        union {
/*
                struct {
                        mode_t  mode;
                }mkdir;

                struct {
                        mode_t  mode;
                }creat;
*/
                struct {
                        mode_t  mode;
                        dev_t   dev;
                }mknod;
/*
                struct {
                        int     flags;
                }open;

                struct {
                        ;
                }opendir;
                struct {
                        ;
                }release;

                struct {
                        ;
                }releasedir;

*/
                struct {
                        off_t           offset;/* random */
                }pwritev;

                struct {
                        off_t           offset;/* random */
                }preadv;

                struct {
                        char    newpath[PATH_MAX];
                }rename;

                struct {
                        char    newpath[PATH_MAX];
                }symlink;

                struct {
                        char    newpath[PATH_MAX];
                }link;

                struct {
                        off_t   length;/* random */
                }truncate;

                struct {
                        off_t   length;/* random */
                }ftruncate;
/*
                struct {
                        ;
                }fsync;
*/
                struct {
                        char    name[XATTR_KEY_LEN];/* random */
                        char    value[XATTR_VAL_LEN];/* random */
                        int     flags;/* REPLACE */
                }setxattr;

                struct {
                        char    name[XATTR_KEY_LEN];
                        char    value[XATTR_VAL_LEN];
                        int     flags;
                }fsetxattr;

                struct {
                        char    name[XATTR_KEY_LEN];
                }getxattr;

                struct {
                        char    name[XATTR_KEY_LEN];
                }fgetxattr;

                struct {
                        char    name[XATTR_KEY_LEN];
                }removexattr;

                struct {
                        char    name[XATTR_KEY_LEN];
                }fremovexattr;
/*
                struct {
                        ;
                }readdir;
*/
                struct {
                        int     mode;
                }access;

                struct {
                        int     operation;
                }lk;
                
                struct {
                        uid_t           owner;
                        gid_t           group;
                }chown;

                struct {
                        mode_t          mode;
                }chmod;

                struct {
                        struct timeval  time[2];
                }utimes;
                
                struct {
                        uid_t           owner;
                        gid_t           group;
                }fchown;

                struct {
                        mode_t          mode;
                }fchmod;

                struct {
                        struct timeval  time[2];
                }futimes;
/*
                struct {
                        ;
                }rmdir;

                struct {
                        ;
                }readlink;

                struct {
                        ;
                }stat;

                struct {
                        ;
                }unlink;

                struct {
                        ;
                }statfs;

                struct {
                        ;
                }fstat;
*/
                struct {
                        int             mode;/* random */
                        off_t           offset;/* random */
                        off_t           len;/* random */
                }fallocate;
        }args;
}file_op_t;

typedef struct {
        pthread_mutex_t         lock;
        file_node_t             *file;
        struct list_head        op_group;/* chain to file_ops_group->opsgroup */

        struct list_head        ops;
        int                     op_cnt;
        int                     ops_index;/*  */

        int                     file_type;
        int                     is_new;
        int                     op_type;
}file_ops_t;

typedef struct {
        pthread_mutex_t         lock;
        
        struct list_head        opsgroup;
        struct list_head        group_list;
        int                     ops_cnt;
        int                     opsgroups_index;
}file_ops_group_t;

typedef struct {
        uint64_t                start;
        uint64_t                end;
        int                     key;/* op/op_type/file_type/offset */
        struct list_head        layout_list;
}layout_t;

typedef struct {
        struct list_head        layouts;
        int                     layout_cnt;
        int                     accuracy;             
}layout_table_t;

typedef enum {
        OPS_FILE_TYPE_DISTRIBUTION,/* ==0 */
        NEWDIR_DISTRIBUTION,/* 1 */
        OPENDIR_DISTRIBUTION,/* 2 */
        OPENDIR_OPS_DISTRIBUTION,/* 3 */
        NO_OPENDIR_OPS_DISTRIBUTION,/* 4 */
        NEWFILE_DISTRIBUTION,/* 5 */
        OPENFILE_DISTRIBUTION,/* 6 */
        OPENFILE_OPS_DISTRIBUTION,/* 7 */
        NO_OPENFILE_OPS_DISTRIBUTION,/* 8 */
        SYMLINK_OPS_DISTRIBUTION,/* 9 */
        LAYOUT_TYPE_MAX/* 10 */
} layout_type_t;

layout_table_t     *global_random_layout_table[LAYOUT_TYPE_MAX];

/* Functions table */
int
create_layout_table_list();

int
list_all_random();/* for unit test */

void
list_group_ops(file_ops_group_t *group);

int
file_ops_group_create();

file_ops_group_t *
file_ops_group_new (int opsgroups_index);

int
file_ops_group_destroy(file_ops_group_t *group);
/* End of function table */
#endif
