/*
 *
 */
#ifndef __SHARK_FILE_H
#define __SHARK_FILE_H

#include <pthread.h>
#include <stdint.h>
#include "list.h"
#include <limits.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "rb.h"

typedef enum file_type file_type_t;

enum file_type {
        UNKNOWN_TYPE,
        DIR_TYPE,/* == 1 */
        SYMLINK_TYPE,
        FILE_TYPE,
        MAX_TYPE
};

typedef struct file_node file_node_t;

struct file_node {
        /* TODO:
        struct list_head                *sub_node;
        struct list_head                sibling;
        */
        pthread_mutex_t                 lock;
        struct file_node                *parent;
        
        /* format: index-XXXXXX-file_type */
        char                            *file_name;


        /* dir or file or symlink */
        int                             file_type;

        /* Concurrent operation on file is not allowed */
        int                             usecnt;
};

typedef struct file_node_table_ file_node_table_t;

struct file_node_table_ {
        
        struct rb_table                 *idle_file_rbtree;
        struct rb_table                 *busy_file_rbtree;
        pthread_mutex_t                 file_lock;

        struct rb_table                 *idle_dir_rbtree;
        struct rb_table                 *busy_dir_rbtree;
        pthread_mutex_t                 dir_lock;

        struct rb_table                 *idle_symlink_rbtree;
        struct rb_table                 *busy_symlink_rbtree;
        pthread_mutex_t                 symlink_lock;

        uint64_t                        file_cnt;
        uint64_t                        dir_cnt;
        uint64_t                        symlink_cnt;
};

/* Functions table */
file_node_t *
file_delete_form_rbt(int file_type, uint64_t index, bool form_idle);

int
file_insert_to_rbt(int file_type, file_node_t *file_node, bool to_idle);

int
file_node_usecnt_add(file_node_t *file_node);

int
file_node_usecnt_decrease(file_node_t *file_node);

file_node_table_t *
file_node_table_initialize();

int
file_node_table_destroy (file_node_table_t *node_table);

file_node_t *
file_node_create (int file_type, int for_op);

void
file_node_destory(void *file_node, void *prama);

int
create_original_file (int ndir, int nfile, int nsymlink);

char *
get_path_for_file_node(file_node_t *file_node);
/* End of function table */
#endif
