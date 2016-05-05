/**********************************************
AUTHOR:zhd
TIME:Sat 26 Dec 2015 10:03:49 AM CST
FILENAME:shark_file.c
DESCRIPTION:.............
**********************************************/
#include "shark_file.h"
#include "shark_ops.h"
#include "journal.h"
#include "log.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

extern config_globle_var cgv;

#define SHARK_FILE_DEBUG 0
#if SHARK_FILE_DEBUG
#define shark_log(str)        {                                                       \
        do {                                                                    \
                printf("[%s %s:%d] %s\n",__FILE__,__func__,__LINE__,str);     \
        }while(0);                                                           \
}              
#else
#define shark_log(str) {do {;}while(0);}
#endif
/*
char                    *tmp_root = "/mnt/disk_sde/";
*/
file_node_table_t       *file_node_table = NULL;

char *
construction_file_name (uint64_t index, int file_type)
{
        char            *file_name = NULL;
        char            *tmp = NULL;
        
        file_name = (char *)calloc(1,PATH_MAX);
        if (!file_name) {
                goto out;
        }

        sprintf(file_name, "%"PRId64"-XXXXXX",index);
        tmp = mktemp(file_name);

        switch (file_type) {
                case DIR_TYPE:
                        return strcat(file_name, "-dir");
                case FILE_TYPE:
                        return strcat(file_name, "-file");
                case SYMLINK_TYPE:
                        return strcat(file_name, "-symlink");
                default:
                        free (file_name);
                        file_name = NULL;
                        goto out;
        }
        shark_log(file_name);
out:
        return file_name;
}

uint64_t
get_idle_index(int file_type)
{
        uint64_t                ret = 0;

        if (!file_node_table) {
                ret = -1;
                working_journal_log ("SHARK_FILE",LOG_ERROR,"Can't get global file node table");
                goto out;
        }

        switch (file_type) {
                case DIR_TYPE:
                        pthread_mutex_lock(&file_node_table->dir_lock);
                        ret = file_node_table->dir_cnt++;
                        pthread_mutex_unlock(&file_node_table->dir_lock);
                        break;
                case FILE_TYPE:
                        pthread_mutex_lock(&file_node_table->file_lock);
                        ret = file_node_table->file_cnt++;
                        pthread_mutex_unlock(&file_node_table->file_lock);
                        break;
                case SYMLINK_TYPE:
                        pthread_mutex_lock(&file_node_table->symlink_lock);
                        ret = file_node_table->symlink_cnt++;
                        pthread_mutex_unlock(&file_node_table->symlink_lock);
                        break;
                default:
                        ret = -1;
                        working_journal_log ("SHARK_FILE",LOG_ERROR,"Invalid file type");
                        break;
        }
out:
        return ret;
}

bool
check_path_depth(file_node_t *file_node)
{
        bool                    ret = true;
        file_node_t             *par = file_node->parent;
        int                     depth = 0;

        while (par) {
                depth++;
                par = par->parent;
        }

        if (depth >= 10) {
                working_journal_log ("SHARK_FILE",LOG_DEBUG,"Exceed the maximum directory depth");
                ret = false; 
        }

        return ret;
}

file_node_t *
file_delete_form_rbt(int file_type, uint64_t index, bool form_idle)
{
        file_node_t             *file_node = NULL;
        file_node_t             tmp;
        struct rb_table         *rbtable = NULL;
        char                    tmp_name[PATH_MAX] = {0};
        pthread_mutex_t         *lock = NULL;


        memset(&tmp,0,sizeof(file_node_t));
        working_journal_log ("SHARK_FILE",LOG_TRACE,"file_type is %d,index is %"PRId64",delete from %d",
                        file_type,index,form_idle);
        switch (file_type) {
                case DIR_TYPE:
                        if (form_idle) {
                                rbtable = file_node_table->idle_dir_rbtree;
                        } else {
                                rbtable = file_node_table->busy_dir_rbtree;
                        }
                        lock = &file_node_table->dir_lock;
                        break;
                case FILE_TYPE:
                        if (form_idle) {
                                rbtable = file_node_table->idle_file_rbtree;
                        } else {
                                rbtable = file_node_table->busy_file_rbtree;
                        }
                        lock = &file_node_table->file_lock;
                        break;
                case SYMLINK_TYPE:
                        if (form_idle) {
                                rbtable = file_node_table->idle_symlink_rbtree;
                        } else {
                                rbtable = file_node_table->busy_symlink_rbtree;
                        }
                        lock = &file_node_table->symlink_lock;
                        break;
                default:
                        file_node = NULL;
                        working_journal_log ("SHARK_FILE",LOG_ERROR,"Invalid file type");
                        goto out;
        }

        sprintf (tmp_name, "%"PRId64"-%s-%s",index,"forfind","filetype");
        tmp.file_name = strdup(tmp_name);
        pthread_mutex_init(&tmp.lock,NULL);

        pthread_mutex_lock (lock);
        file_node = rb_delete(rbtable, &tmp);
        pthread_mutex_unlock (lock);
        pthread_mutex_destroy(&tmp.lock);

out:
        if (tmp.file_name) {
                free(tmp.file_name);
        }
        return file_node;
}

int
file_insert_to_rbt (int file_type, file_node_t *file_node, bool to_idle)
{
        int                     ret = -1;
        file_node_t             *tmp = NULL;
        struct rb_table         *rbtable = NULL;
        pthread_mutex_t         *lock = NULL;

        if (!file_node) {
                goto out;
        }

        working_journal_log ("SHARK_FILE",LOG_TRACE,"file_type is %d,filename is %s,insert to %d",
                        file_type,file_node->file_name,to_idle);
        switch (file_type) {
                case DIR_TYPE:
                        if (to_idle) {
                                rbtable = file_node_table->idle_dir_rbtree;
                        } else {
                                rbtable = file_node_table->busy_dir_rbtree;
                        }
                        lock = &file_node_table->dir_lock;
                        break;
                case FILE_TYPE:
                        if (to_idle) {
                                rbtable = file_node_table->idle_file_rbtree;
                        } else {
                                rbtable = file_node_table->busy_file_rbtree;
                        }
                        lock = &file_node_table->file_lock;
                        break;
                case SYMLINK_TYPE:
                        if (to_idle) {
                                rbtable = file_node_table->idle_symlink_rbtree;
                        } else {
                                rbtable = file_node_table->busy_symlink_rbtree;
                        }
                        lock = &file_node_table->symlink_lock;
                        break;
                default:
                        working_journal_log ("SHARK_FILE",LOG_ERROR,"Invalid file type");
                        goto out;
        }

        pthread_mutex_lock (lock);
        tmp = rb_insert(rbtable, file_node);
        pthread_mutex_unlock (lock);
        
        ret = (!tmp)?0:-1;
out:
        return ret;
}

int
file_node_compare(const void *node1, const void *node2,void *param)
{
        file_node_t             *file1 , *file2;
        uint64_t                index1,index2;
        char                    tmpbuf[PATH_MAX] = {0};
        int                     ret = -1;

        if (!node1 || !node2 || !param) {
                goto out;
        }

        file1 = (file_node_t *)node1;
        file2 = (file_node_t *)node2;

        sscanf (file1->file_name, "%"PRId64"-%s",&index1,tmpbuf);
        sscanf (file2->file_name, "%"PRId64"-%s",&index2,tmpbuf);

        //printf ("node1 index is %" PRId64 ",node2 index is %" PRId64 "\n",index1,index2);

        if (index1 < index2) {
                ret = -1;
        } else if (index1 > index2){
                ret = 1;
        } else {
                ret = 0;
        }
out:
        return ret;
}

static int      param = 0;

file_node_table_t *
file_node_table_initialize()
{
        file_node_table_t       *node_table = NULL;

        node_table = (file_node_table_t *)calloc (1,sizeof(file_node_table_t));
        if (!node_table) {
                goto err;/* error */
        }

        node_table->idle_file_rbtree = rb_create(file_node_compare, &param, NULL);
        if (!node_table->idle_file_rbtree) {
                goto free_node_table;
        }
        node_table->busy_file_rbtree = rb_create(file_node_compare, &param, NULL);
        if (!node_table->busy_file_rbtree) {
                goto free_idle_file;
        }
        node_table->file_cnt = 0;
        pthread_mutex_init (&node_table->file_lock, NULL);

        node_table->idle_dir_rbtree = rb_create(file_node_compare, &param, NULL);
        if (!node_table->idle_dir_rbtree) {
                goto free_busy_file;
        }
        node_table->busy_dir_rbtree = rb_create(file_node_compare, &param, NULL);
        if (!node_table->busy_dir_rbtree) {
                goto free_idle_dir;
        }
        node_table->dir_cnt = 0;
        pthread_mutex_init (&node_table->dir_lock, NULL);
        
        node_table->idle_symlink_rbtree = rb_create(file_node_compare, &param, NULL);
        if (!node_table->idle_symlink_rbtree) {
                goto free_busy_dir;
        }
        node_table->busy_symlink_rbtree = rb_create(file_node_compare, &param, NULL);
        if (!node_table->busy_symlink_rbtree) {
                goto free_idle_sym;
        }
        node_table->symlink_cnt = 0;
        pthread_mutex_init (&node_table->symlink_lock, NULL);
        
        return node_table;

free_idle_sym:
        rb_destroy(node_table->idle_symlink_rbtree, file_node_destory);
free_busy_dir:
        rb_destroy(node_table->busy_dir_rbtree, file_node_destory);
free_idle_dir:
        rb_destroy(node_table->idle_dir_rbtree, file_node_destory);
free_busy_file:
        rb_destroy(node_table->busy_file_rbtree, file_node_destory);
free_idle_file:
        rb_destroy(node_table->idle_file_rbtree, file_node_destory);
free_node_table:
        free(node_table);
err:
        working_journal_log ("SHARK_FILE",LOG_ERROR,"Failed to allocate memory");
        return NULL;
}

int
file_node_table_destroy (file_node_table_t *node_table)
{
        if (!node_table) {
                goto out;;/* error */
        }
        rb_destroy (node_table->idle_file_rbtree, file_node_destory);
        rb_destroy (node_table->busy_file_rbtree, file_node_destory);
        rb_destroy (node_table->idle_dir_rbtree, file_node_destory);
        rb_destroy (node_table->busy_dir_rbtree, file_node_destory);
        rb_destroy (node_table->idle_symlink_rbtree, file_node_destory);
        rb_destroy (node_table->busy_symlink_rbtree, file_node_destory);
        pthread_mutex_destroy (&node_table->file_lock);
        pthread_mutex_destroy (&node_table->dir_lock);
        pthread_mutex_destroy (&node_table->symlink_lock);
        free (node_table);
        node_table = NULL;
out:
        return 0;
}

file_node_t *
file_node_create (int file_type, int for_op)
{
        uint64_t                par_index = 0;
        file_node_t             *par_node = NULL;
        uint64_t                dir_cnt = 0;
        uint64_t                curr_index = 0;
        file_node_t             *file_node = NULL;
        uint64_t                old_index = 0;
        file_node_t             *old_file = NULL;
        int                     err = 0;
        char                    compare_old_path[PATH_MAX] = {0};
        char                    target_old_path[PATH_MAX] = {0};

        char                    target_path[PATH_MAX] = {0};
        char                    compare_path[PATH_MAX] = {0};

        char                    *file_path = NULL;

        if ( !file_node_table ) {
                goto out;/* error */
        }

        curr_index = get_idle_index(file_type);
        working_journal_log ("SHARK_FILE", LOG_TRACE, "curr index is %"PRId64" file type is %d",
                        curr_index,file_type);
        while (1) {
                dir_cnt = file_node_table->dir_cnt;
                if (dir_cnt == 0) {
                        par_index = 0;
                } else {
                        par_index = random()%(dir_cnt);
                }
                working_journal_log("SHARK_FILE", LOG_TRACE, "curr_index is %" PRId64 ",select %" PRId64 " as par-index",
                                curr_index,par_index);
                if (par_index == 0) {
                        par_node = NULL;
                        goto create_on_root;
                }

                par_node = file_delete_form_rbt (DIR_TYPE, par_index, 1);
                if (par_node == NULL) {
                        shark_log("file_delete_form_rbt");
                        working_journal_log ("SHARK_FILE", LOG_TRACE, "Can't get a file node form "
                                        "idle dir rbtree,par_index is %"PRId64"",par_index);
                        usleep(10000);
                        continue;
                }

                if (!check_path_depth(par_node)) {
                        working_journal_log("SHARK_FILE",LOG_DEBUG,"Exceed the maximum directory depth");
                        err = file_insert_to_rbt(DIR_TYPE,par_node,1);
                        if (err == -1) {
                                working_journal_log("SHARK_FILE",LOG_ERROR,"insert dir to idle rbtree failed");
                        }
                        usleep(10000);
                        continue;
                }
create_on_root:
                file_node = (file_node_t *)calloc(1,sizeof(file_node_t));
                if (!file_node) {
                        working_journal_log ("SHARK_FILE",LOG_ERROR,"Failed to allocate memory");
                        if (par_node) {
                                err = file_insert_to_rbt(DIR_TYPE,par_node,1);
                                if (err == -1) {
                                        working_journal_log("SHARK_FILE",LOG_ERROR,"insert dir to idle rbtree failed");
                                }
                        }
                        break;/* error */
                }

                file_node->parent = par_node;
                /* TODO
                INIT_LIST_HEAD (&file_node->sibling);
                INIT_LIST_HEAD (&file_node->sub_node);
                */
                file_node->file_type = file_type;
                file_node->file_name = construction_file_name(curr_index+1,file_type);
                file_node->usecnt = 0;
                pthread_mutex_init(&file_node->lock,NULL);

                shark_log(file_node->file_name);


                /* TODO:
                list_add (&file_node->sibling, &par_node->sub_node);
                */
                memset(target_path,0,PATH_MAX);
                strncpy(target_path,cgv.test_dir_path,strlen(cgv.test_dir_path));
                memset(compare_path,0,PATH_MAX);
                strncpy(compare_path,cgv.validation_dir_path,strlen(cgv.validation_dir_path));
                
                file_path = get_path_for_file_node(file_node);
                strcat(target_path,file_path);
                strcat(compare_path,file_path);
                if (file_path) {
                        free(file_path);
                        file_path = NULL;
                }

                if (!for_op) {
                        if (file_type == DIR_TYPE) {
                                err = mkdir(target_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                                if (err) {
                                        working_journal_log ("SHARK_FILE", LOG_ERROR, "Create orifinal file failed");
                                        shark_log("mkdir");
                                        free(file_node->file_name);
                                        free(file_node);
                                        goto put_par_to_idle;
                                }
                                err = mkdir(compare_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                                if (err) {
                                        working_journal_log ("SHARK_FILE", LOG_ERROR, "Create orifinal file failed");
                                        shark_log("mkdir");
                                        rmdir(target_path);
                                        free(file_node->file_name);
                                        free(file_node);
                                        goto put_par_to_idle;
                                }

                                err = file_insert_to_rbt(DIR_TYPE,file_node,1);
                                if (err == -1) {
                                        working_journal_log("SHARK_FILE",LOG_ERROR,"insert dir to idle rbtree failed");
                                }
                                
                        } else if (file_type == FILE_TYPE) {
                                err = creat(target_path, 777);
                                if (err == -1) {
                                        working_journal_log ("SHARK_FILE", LOG_ERROR, "Create orifinal file failed");
                                        shark_log("creat");
                                        free(file_node->file_name);
                                        free(file_node);
                                        goto put_par_to_idle;
                                } else {
                                        close(err);
                                }
                                err = creat(compare_path, 777);
                                if (err == -1) {
                                        working_journal_log ("SHARK_FILE", LOG_ERROR, "Create orifinal file failed");
                                        shark_log("creat");
                                        unlink(target_path);
                                        free(file_node->file_name);
                                        free(file_node);
                                        goto put_par_to_idle;
                                } else {
                                        close(err);
                                }

                                err = file_insert_to_rbt(FILE_TYPE,file_node,1);
                                if (err == -1) {
                                        working_journal_log("SHARK_FILE",LOG_ERROR,"insert file to idle rbtree failed");
                                }

                        } else {
                                /**
                                 * Only create symlink for file,which means
                                 * there are files on disk and glusterfs,in
                                 * other words,file_node_table->file_cnt != 0
                                 **/
                                curr_index = file_node_table->file_cnt;
re_select:
                                if (curr_index == 0) {
                                        old_index = 0;
                                } else {
                                        if (curr_index == 0) {
                                                ;/* error ,there are no files,can't run here */
                                        } else if (curr_index == 1){
                                                old_index = 1;
                                        } else {
                                                do {
                                                        old_index = random()%(curr_index);
                                                }while(!old_index);
                                        }
                                }

                                if (old_index == 0) {
                                        goto re_select;
                                }

                                old_file = file_delete_form_rbt(FILE_TYPE,old_index,1);
                                if (!old_file) {
                                        goto re_select;
                                }
                                working_journal_log ("SHARK_FILE", LOG_TRACE, "Now we have the old file");

                                memset(target_old_path,0,PATH_MAX);
                                memset(compare_old_path,0,PATH_MAX);
                                strncpy(target_old_path,cgv.test_dir_path,strlen(cgv.test_dir_path));
                                strncpy(compare_old_path,cgv.validation_dir_path,strlen(cgv.validation_dir_path));
                                
                                file_path = get_path_for_file_node(old_file);
                                strcat(target_old_path,file_path);
                                strcat(compare_old_path,file_path);
                                if (file_path) {
                                        free(file_path);
                                        file_path = NULL;
                                }

                                err = symlink(target_old_path,target_path);
                                if (err == -1) {
                                        working_journal_log ("SHARK_FILE", LOG_ERROR, "Create orifinal file failed");
                                        shark_log("symlink");
                                        free(file_node->file_name);
                                        free(file_node);
                                        goto put_par_to_idle;
                                }
                                err = symlink(compare_old_path,compare_path);
                                if (err == -1) {
                                        working_journal_log ("SHARK_FILE", LOG_ERROR, "Create orifinal file failed");
                                        shark_log("symlink");
                                        unlink(target_path);
                                        free(file_node->file_name);
                                        free(file_node);
                                        goto put_par_to_idle;
                                }

                                err = file_insert_to_rbt(FILE_TYPE,old_file,1);
                                if (err == -1) {
                                        working_journal_log("SHARK_FILE",LOG_ERROR,"insert file to idle rbtree failed");
                                }
                                err = file_insert_to_rbt(SYMLINK_TYPE,file_node,1);
                                if (err == -1) {
                                        working_journal_log("SHARK_FILE",LOG_ERROR,"insert symlink to idle rbtree failed");
                                }
                        }
                        working_journal_log("SHARK_FILE",LOG_DEBUG,"create file_node for initialize ,"
                                        "name is %s",file_node->file_name);
                } else {
                        if (file_type == DIR_TYPE) {
                                err = file_insert_to_rbt(DIR_TYPE,file_node,0);
                                if (err == -1) {
                                        working_journal_log("SHARK_FILE",LOG_ERROR,"insert dir to busy rbtree failed");
                                }
                        } else if (file_type == FILE_TYPE) {
                                err = file_insert_to_rbt(FILE_TYPE,file_node,0);
                                if (err == -1) {
                                        working_journal_log("SHARK_FILE",LOG_ERROR,"insert file to busy rbtree failed");
                                }
                        } else {
                                err = file_insert_to_rbt(SYMLINK_TYPE,file_node,0);
                                if (err == -1) {
                                        working_journal_log("SHARK_FILE",LOG_ERROR,"insert symlink to busy rbtree failed");
                                }
                        }
                        file_node_usecnt_add(file_node);
                        working_journal_log("SHARK_FILE",LOG_DEBUG,"create file_node for op ,"
                                        "name is %s",file_node->file_name);
                }
put_par_to_idle:
                if (par_node) {
                        working_journal_log("SHARK_FILE",LOG_TRACE,"get dir_index %s as par_dir",par_node->file_name);
                        err = file_insert_to_rbt(DIR_TYPE,par_node,1);
                        if (err == -1) {
                                working_journal_log("SHARK_FILE",LOG_ERROR,"insert dir to idle rbtree failed");
                        }
                } else {
                        working_journal_log("SHARK_FILE",LOG_TRACE,"get dir_index 0 as par_dir");
                }
                break;
        }
out:
        return file_node;
}

void
file_node_destory(void *file_node, void *prama)
{
        file_node_t     *file;
        int             file_type = 0;

        if (!file_node || !prama) {
                goto out;
        }

        file = (file_node_t *)file_node;
        if (file->file_type == DIR_TYPE) {
#if 0
                if (file->usecnt != 0) {
                        /**
                         * TODO:Does not support removing dir operation
                         **/
                        working_journal_log ("SHARK_FILE", LOG_ERROR, "rm dir,file_name is %s",file->file_name);
                        goto out;
                }
#endif
                working_journal_log ("SHARK_FILE", LOG_ERROR, "rm dir,usecnt is %d,"
                                "file_name is %s",file->usecnt,file->file_name);
                goto out;
                pthread_mutex_lock(&file_node_table->dir_lock);
                file_type = DIR_TYPE;
        } else if (file->file_type == SYMLINK_TYPE) {
                pthread_mutex_lock(&file_node_table->symlink_lock);
                file_type = SYMLINK_TYPE;
        } else {
                pthread_mutex_lock(&file_node_table->file_lock);
                file_type = FILE_TYPE;
        }

        if (file->usecnt) {
                /**
                 * For dir,     usercnt == refcount
                 * For symlink, usercnt == 0
                 * For file,    usercnt == 0
                 * In other words,for symlink and file,never goto here.this
                 * branch is used for remove dir.
                 **/
                working_journal_log ("SHARK_FILE", LOG_INFO, "the file is in use,file_name is %s",file->file_name);
                pthread_mutex_unlock(&file->lock);
        //        file_node_usecnt_decrease (file);
                goto out;/* being use */
        }
        working_journal_log ("SHARK_FILE", LOG_INFO, "destroy file_node,name is %s",file->file_name);
        free(file->file_name);
        pthread_mutex_destroy(&file->lock);
        free(file);
        file = NULL;

        if (file_type == DIR_TYPE) {
                pthread_mutex_unlock(&file_node_table->dir_lock);
        } else if (file_type == SYMLINK_TYPE) {
                pthread_mutex_unlock(&file_node_table->symlink_lock);
        } else {
                pthread_mutex_unlock(&file_node_table->file_lock);
        }
out:
        return;
}

static int
file_node_usecnt_update (file_node_t *file_node, bool is_add)
{
        int             ret = -1;
        file_node_t     *par = NULL;
        uint64_t        init_index = 0;
        char            buf[PATH_MAX] = {0};


        if ( file_node ) {
                pthread_mutex_lock(&file_node->lock);
                par = file_node->parent;
                if (par) {
                        pthread_mutex_lock(&par->lock);
                }
                while (par) {
                        memset(buf,0,PATH_MAX);
                        sscanf(par->file_name,"%"PRId64"-%s",&init_index,buf);
                        if (init_index < cgv.initial_dir_cnts) {
                                working_journal_log("SHARK_FILE",LOG_DEBUG,"get par index is %"PRId64"",init_index);
                                pthread_mutex_unlock(&par->lock);
                                break;
                        }

                        if (is_add) {
                                working_journal_log("SHARK_FILE",LOG_DEBUG,"add usecnt:file_name is %s,usecnt is %d,"
                                                "file_type is %d",par->file_name,par->usecnt,par->file_type);
                                par->usecnt++;
                        } else {
                                working_journal_log("SHARK_FILE",LOG_DEBUG,"decreae usrcnt:file_name is %s,usecnt is %d,"
                                                "file_type is %d",par->file_name,par->usecnt,par->file_type);
                                par->usecnt--;
                        }
                        pthread_mutex_unlock(&par->lock);
                        par = par->parent;
                        if (par) {
                                pthread_mutex_lock(&par->lock);
                        }
                }
                pthread_mutex_unlock(&file_node->lock);
                ret = 0;
        }
        return ret;
}

int
file_node_usecnt_add (file_node_t *file_node)
{
        return file_node_usecnt_update (file_node, 1);
}

int
file_node_usecnt_decrease (file_node_t *file_node)
{
        return file_node_usecnt_update (file_node, 0);
}

int
create_original_file (int ndir, int nfile, int nsymlink)
{
        int             i;
        int             ret = 0;

        /**
         * create original file(dir,file,symlink).
         * first,create dir;
         * second,create file;
         * finally create symlink.
         **/
        for (i = 0;i < ndir; i++){
                file_node_create (DIR_TYPE, 0);
        }
        for (i = 0;i < nfile;i++) {
                file_node_create (FILE_TYPE, 0);
        }
        for (i = 0;i < nsymlink;i++){
                file_node_create (SYMLINK_TYPE, 0);
        }
        ret = i;
        return ret;
}

char *
get_path_for_file_node(file_node_t *file_node)
{
        char                    *buf = NULL;
        char                    tmp[PATH_MAX] = {0};
        file_node_t             *par = NULL;

        if (!file_node) {
                working_journal_log ("SHARK_FILE", LOG_ERROR, "file node is NULL");
                goto out;/* error */
        }

        buf = (char *)calloc(1,PATH_MAX);
        if (!buf) {
                working_journal_log ("SHARK_FILE",LOG_ERROR,"Failed to allocate memory");
                goto out;
        }

        pthread_mutex_lock(&file_node->lock);
        strncpy (buf,file_node->file_name, strlen(file_node->file_name));
        par = file_node->parent;
        if (par) {
                pthread_mutex_lock(&par->lock);
        }
        while (par != NULL) {
                sprintf(tmp,"%s/%s",par->file_name,buf);
                memset(buf,0,PATH_MAX);
                strncpy(buf,tmp,strlen(tmp));
                memset(tmp,0,PATH_MAX);
                pthread_mutex_unlock(&par->lock);
                par = par->parent;
                if (par) {
                        pthread_mutex_lock(&par->lock);
                }
        }
        pthread_mutex_unlock(&file_node->lock);
out:
        return buf;
}

