#include "shark_posix.h"
#include "checkout.h"
#include "journal.h"
#include "shark_file.h"
#include "log.h"
#include <sys/statvfs.h>


thread_manager  *manager        = NULL;
error_list      *all_error_list = NULL;
thread_pool     *check_pool     = NULL;
int             check_flag      = 0;

unsigned int    groups_index ;
unsigned int    group_wait_list;
unsigned int    total_op_list;
int             conf_thread_size;
unsigned int    check_max_depth;

unsigned int    max_error_range;

int             pool_percentage;

int             min_worker_queue;


char        digioceand_mount[1024] ;
char        disk_mount[1024] ;
int         digioceand_mount_size ;
int         disk_mount_size;



uint64_t cut_str_to_int(char *path)
{


    char         file[1024] = "\0";
    int          i = 0,j = 0;
    int          size_len = 0;
    char         char_index[1024] = "\0";
    uint64_t     index = 0;

    if(!path){
        return -1;
    }


    size_len = strlen(path);
    for (i = size_len-1; i>0; i--) {
        if (path[i] == '/')
            break;
    }

    for (j = 0; i < size_len; j++) {
        i++;
        file[j] = path[i];
    }

    file[j+1] = '\0';

    sscanf (file, "%"PRId64"-%s",&index,char_index);


    working_journal_log("pool_thread", LOG_DEBUG, 
        "path->%s  int_index->%d",  path, index);

    return index;
}




int  worker_move_list(worker_list *worker)
{

    journal_file_op_list_t  *op_list    = (journal_file_op_list_t*) worker->args;
    file_op_t               *op         = NULL;
    int                     index       = 0;

    file_node_t             *file_node  = NULL;
    uint64_t                int_index   = 0;

    if(!worker){

        working_journal_log("pool_thread worker", LOG_ERROR,
            "worker  is  NULL");
        return 0;
    }

/*Don't open the file operations*/
    if(worker->op_type == NO_OPEN_FILE_OP){
        list_for_each_entry(op,&op_list->op_list_head, oplist){
 
            if(op->op == RENAME_FILE_OP){
                int_index = cut_str_to_int(op->args.rename.newpath);
                file_node= file_delete_form_rbt(worker->file_type,int_index, 0); 
 
            }
            if(op->op == LINK_FILE_OP){
                int_index = cut_str_to_int(op->args.link.newpath);
                file_node= file_delete_form_rbt(worker->file_type,int_index, 0);
            }
 
            if(op->op == SYMLINK_FILE_OP){
                int_index = cut_str_to_int(op->args.symlink.newpath);
                file_node= file_delete_form_rbt(worker->file_type,int_index, 0);
            }
            
            if(index <= worker->op_index){
 
                if(file_node){
                    working_journal_log("SHARK_OPS",LOG_DEBUG,"delete from busy,insert to idle,"
                                                        "name is %s",file_node->file_name);
                    file_node_usecnt_decrease(file_node);
                    file_insert_to_rbt(worker->file_type,file_node, 1);
                }
            }else{
 
                if(file_node){

                    working_journal_log("SHARK_OPS",LOG_DEBUG,"delete from busy,destroy file_node ,"
                                                                "path->%s", file_node->file_name);
                    file_node_usecnt_decrease(file_node);
                    file_node_destory(file_node,file_node);
                }
            }
            file_node = NULL;
            index++; 
        }


        int_index = cut_str_to_int(worker->abs_path);
        file_node = file_delete_form_rbt(worker->file_type,int_index, 0);
        if(file_node){

            working_journal_log("SHARK_OPS",LOG_DEBUG,"delete from busy,insert to idle",
                        "name is %s",file_node->file_name);

            if(worker->ret == 0){
                file_node_usecnt_decrease(file_node);
                file_insert_to_rbt(worker->file_type,file_node, 1);
            }else{
                file_node_usecnt_decrease(file_node); 
                file_node_destory(file_node,file_node);
            }
        }
        index = 0;
/*Don't open directory operations*/
    }else if(worker->op_type == NO_OPEN_DIR_OP){
 
        list_for_each_entry(op,&op_list->op_list_head, oplist){
 
            if(op->op == SYMLINK_DIR_OP){
 
                int_index = cut_str_to_int(op->args.symlink.newpath);
                file_node= file_delete_form_rbt(worker->file_type,int_index, 0);
            
        
                if(index <= worker->op_index){
 
                    if(file_node){
 
                        working_journal_log("SHARK_OPS",LOG_DEBUG,"delete from busy,insert to idle,"
                                "name is %s",file_node->file_name);
                        file_node_usecnt_decrease(file_node);
                        file_insert_to_rbt(worker->file_type,file_node, 1);
                    }
                }else{
                    if(file_node){
                        working_journal_log("SHARK_OPS",LOG_DEBUG,"delete from busy,destroy file_node ,"
                                    "path->%s", file_node->file_name);
                        file_node_usecnt_decrease(file_node);
                        file_node_destory(file_node,file_node);
                    }
                }
            }
            file_node = NULL;
            index++;
        }

        if(worker->ret == 0){
            working_journal_log("SHARK_OPS",LOG_DEBUG,"delete from busy,insert to idle",
                        "name is %s",worker->abs_path);
            int_index = cut_str_to_int(worker->abs_path);
            file_node = file_delete_form_rbt(worker->file_type,int_index, 0);
            if(file_node){
                file_node_usecnt_decrease(file_node);
                file_insert_to_rbt(worker->file_type,file_node, 1);
            }
        }else{
            working_journal_log("SHARK_OPS",LOG_DEBUG,"Do not need to open directory operations"
                                " have failed, remain in the list, name is %s",
                                worker->abs_path);
        }
/*Open directory operations*/
    }else if(worker->op_type == OPENED_DIR_OP){

        if(worker->ret == 0){

            int_index = cut_str_to_int(worker->abs_path);
            file_node = file_delete_form_rbt(worker->file_type,int_index, 0);
            if(file_node){

                working_journal_log("SHARK_OPS",LOG_DEBUG,"delete from busy,insert to idle,"
                            "name is %s",file_node->file_name);
                file_node_usecnt_decrease(file_node);
                file_insert_to_rbt(worker->file_type,file_node, 1);
            }
        }else{

            working_journal_log("SHARK_OPS",LOG_DEBUG,"Do not need to open directory operations"
                        " have failed, remain in the list, name is %s",
                        worker->abs_path);
        }
    }else{

        int_index = cut_str_to_int(worker->abs_path); 
        file_node = file_delete_form_rbt(worker->file_type,int_index, 0);
        if(file_node){

            if(worker->ret == 0){
                working_journal_log("SHARK_OPS",LOG_DEBUG,"delete from busy,insert to idle,"
                            "name is %s",file_node->file_name);

                file_node_usecnt_decrease(file_node);
                file_insert_to_rbt(worker->file_type,file_node, 1);
            }else{

                working_journal_log("SHARK_OPS",LOG_DEBUG,"delete from busy,destroy file_node ,"
                                                                "path->%s", file_node->file_name);
                file_node_usecnt_decrease(file_node);
                file_node_destory(file_node,file_node);
            }
        }
    }
 

    return 0;
}

void get_configuration()
{
    strcpy(digioceand_mount,cgv.test_dir_path);
    strcpy(disk_mount,cgv.validation_dir_path);
    digioceand_mount_size = strlen(digioceand_mount);
    disk_mount_size = strlen(disk_mount);
    

    total_op_list = cgv.total_op_group;
    group_wait_list = cgv.operations_waiting_list;
    groups_index = total_op_list/group_wait_list;

    check_max_depth = cgv.dir_max_depth;
    conf_thread_size = cgv.working_threads;

    max_error_range = 500;
    pool_percentage = total_op_list / 20;

    min_worker_queue = group_wait_list/2;
    printf("groups_index-->%d=====total_op_list  --->%d\n",groups_index, total_op_list);
}

char *copy_str(const char *path, int n)
{

    int                i = 0;
    int                size = 0;
    char               *copy = NULL;
    int                len = 0;

    if(!path ||path[0] != '/')
        return NULL;

    size = strlen(path);
    len = size - n;
    copy = malloc(len+1);
    for(i = 0; i < len; i++){
        copy[i] = path[n];
        n++;
    }
    copy[i] = '\0';
    return copy;
}

void error_init()
{

    all_error_list = malloc(sizeof(error_list));
    if(!all_error_list){
        working_journal_log("all_error_list", LOG_ERROR, "malloc  all_error_list Distribution of error");
                return ;
    }

    INIT_LIST_HEAD(&all_error_list->digi_list);
    INIT_LIST_HEAD(&all_error_list->disk_list);
    all_error_list->digisum = 0;
    all_error_list->disksum = 0;
    all_error_list->disk_have_destroy = 0;
    all_error_list->digi_have_destroy = 0;
    pthread_mutex_init(&all_error_list->mutex, NULL);

}

int free_error(error_path_list *date)
{
    free(date->path);
    free(date);
    return 0;
}

void free_error_list()
{
    error_path_list     *date     =  NULL;
    error_path_list     *tmp_date =  NULL;

    pthread_mutex_lock(&all_error_list->mutex);

    list_for_each_entry_safe(date, tmp_date, &all_error_list->digi_list, list){

        free_error(date);
    }

    list_for_each_entry_safe(date, tmp_date, &all_error_list->disk_list, list){

        free_error(date);
    }

    pthread_mutex_unlock(&all_error_list->mutex);
}

int free_oper_worker(worker_list *worker)
{
    file_op_t               *op      = NULL;
    file_op_t               *op_tmp  = NULL;
    journal_file_op_list_t  *op_list = (journal_file_op_list_t *) worker->args;


    list_for_each_entry_safe(op, op_tmp, &op_list->op_list_head, oplist) {

        free_journal_op_mem(op);
    }
    free(worker->abs_path);
    free_journal_op_list_mem(op_list);
    free(worker);
    return 0;
}

int free_oper_list(thread_pool *pool)
{

    worker_list                *worker = NULL;
    worker_list                *worker_tmp = NULL;


    pthread_mutex_lock(&pool->mutex);

    list_for_each_entry_safe(worker, worker_tmp, &pool->worker_list, list){
    
        list_del_init(&worker->list);   
        free_oper_worker(worker);
    }
    pthread_mutex_unlock(&pool->mutex);

    return 0;

}

int pool_destroy(thread_pool *pool)
{

    int     i = 0;
    if(pool->destroy)
        return 0;
    pool->destroy = 1;
    pthread_cond_broadcast(&pool->cond);
    for(i = 0; i < pool->thread_size; i++ ){

        pthread_join(pool->thread[i],NULL);
    }
    free(pool->thread);
    free_oper_list(pool);
    free(pool->path_mount);
    pthread_mutex_destroy(&pool->mutex); 
    pthread_cond_destroy(&pool->cond); 
    free(pool);                          
    return 0;
}        

thread_pool  *pool_init(const char *mount_path, worker_type type)
{
    thread_pool     *pool = NULL;
    int             i     = 0;
    int             ret   = 0;

    if(!mount_path)
        return NULL;

    pool = (thread_pool *)malloc(sizeof(thread_pool));
    if(!pool)
        return NULL;
    
    pool->thread_size = conf_thread_size;
    pthread_mutex_init(&pool->mutex,NULL);
    pthread_cond_init(&pool->cond,NULL);
    INIT_LIST_HEAD(&pool->worker_list);
    pool->worker_size = 0;
    pool->type = type;
    pool->group_read = 1;
    pool->perform_sum = 0;
    pool->building = 0;
    pool->group_index= 0;
    pool->read_index = 0;
    pool->op_list_index = 0;
    pool->destroy = 0;
    pool->path_mount =(char *)malloc(strlen(mount_path)+1);
    if(!pool->path_mount)
        goto error;
    strcpy(pool->path_mount, mount_path);

    pool->thread = malloc(sizeof(pthread_t)*pool->thread_size);
    if(!pool->thread)
        goto error;

    pool->error_type = limit_error;
    for(i = 0; i < pool->thread_size; i++){
        ret = pthread_create(&pool->thread[i], 0, oper_thread, pool);
        if(0 != ret)
            goto error;
    }


    return pool;

error:
    pool_destroy(pool);
    return NULL;
}


void manager_destroy()
{
    if(manager->destroy == 1)
        return ;
    manager->destroy = 1;
    if(manager->mask[0]=='0')
        pool_destroy(manager->disk_pool);
    if(manager->mask[1]=='0')
        pool_destroy(manager->digi_pool);

    free(manager->mask);
    free(manager->normal);
    pthread_mutex_unlock(&manager->mutex);
    pthread_mutex_destroy(&manager->mutex);
    free(manager);
    manager = NULL;
}

void manager_init()
{


    manager = (thread_manager *)malloc(sizeof(thread_manager));
    if(!manager){
        goto  error;
    }
    pthread_mutex_init(&manager->mutex, NULL);
    manager->pool_sum = 0;
    memset(manager->mask,'2',2);

    memset(manager->normal,'1',2);

    manager->group_index     = 0;
    manager->op_list_index   = 0;


/*Initialize the configuration file maximum*/
    manager->destroy_group   = groups_index;
    manager->destroy_op_list = total_op_list;

    manager->disk_pool =(thread_pool *)malloc(sizeof(thread_pool));
    manager->disk_pool = pool_init(disk_mount,disk_type);
    if(!manager->disk_pool)
        goto error;

    manager->pool_sum++;
    manager->mask[0]   = '0';
    manager->normal[0] = '0';

    manager->digi_pool =(thread_pool *)malloc(sizeof(thread_pool));
    manager->digi_pool = pool_init(digioceand_mount,digioceand_type);
    if(!manager->digi_pool)
        goto error;
    
    manager->pool_sum++;
    manager->mask[1]    = '0';
    manager->normal[1]  = '0';
    manager->thread_sum = conf_thread_size * 2;

    return ;

error:
    if(manager)
        manager_destroy();
    return ;
}

worker_list *oper_malloc(worker_type type, process pro, journal_file_op_list_t *args, thread_pool *pool,int  group_index)
{

    
    worker_list             *worker  = NULL;
    journal_file_op_list_t  *op_list = args;
    int                     size     = strlen(op_list->path);
    int                     size1    = strlen(pool->path_mount);

    worker = (worker_list *)malloc(sizeof(worker_list));
    if(!worker)
        return NULL;
    worker->type        = type;
    worker->file_type   = op_list->file_type;
    worker->op_type     = op_list->op_type;
    INIT_LIST_HEAD(&worker->list);
    worker->fn          = pro;
    worker->args        = (void *)args;
    worker->ret         = -1;
    worker->tmp_errno   = 0;

    worker->abs_path = malloc(sizeof(char)*(size1 + size + 1));
    strcpy(worker->abs_path, pool->path_mount);
    strcat(worker->abs_path, op_list->path);

    worker->group_index   = group_index;
    worker->op_list_index = op_list->ops_index;

    return worker;
}


int build_worker_queue(thread_pool *pool)
{
    
    journal_file_group_t    *group        = NULL;
    journal_file_op_list_t  *op_list      = NULL;
    journal_file_op_list_t  *tmp_op_list  = NULL;

    worker_list             *worker       = NULL;

    pthread_mutex_lock(&pool->mutex);
    if(1 == pool->group_read){

        group = analyze_string_read (pool->type);

        pool->group = group;
        if(group){
            pool->read_index++;
            pool->group_read = 0;
            working_journal_log("read group ", LOG_DEBUG, "read the group is  %d"
                        "pool->type = %d",
                        pool->read_index,pool->type);
        }else{
            if(pool->read_index >= groups_index ){
/*add by 2016/1/25 */
                if(pool->worker_size <= 0){
/*add by 2016/1/25 */
                    working_journal_log("pool  will destory",
                    LOG_DEBUG, "Read group end  type->%d",pool->type);
                                pthread_mutex_unlock(&pool->mutex);
                                return 1;
                }
            }
        }

        goto unlock;
    }else{
        group = pool->group;
    }
    
    if(!group){
        /*Here need to change back to the configuration file*/
        if(pool->read_index >= groups_index ){
/*add by 2016/1/25 */ 
            if(pool->worker_size <= 0){
/*add by 2016/1/25 */
                working_journal_log("Error reading group counts",
                LOG_ERROR, "Read group end pool->type = %d",
                pool->type);
                printf("Error reading group counts\n");
                pthread_mutex_unlock(&pool->mutex);
                return 1;
            }
            goto unlock;
        }
        else
            goto unlock;
    }

    
/*Build the work queue*/
    if(1 == pool->building)
        goto unlock;
    pool->group         = NULL;
    pool->group_read    = 1;
    pool->building      = 1;


    pthread_mutex_unlock(&pool->mutex);
    working_journal_log("pool building", LOG_DEBUG, "pool->type = %d ",
                pool->type);
    list_for_each_entry_safe(op_list, tmp_op_list, &group->group_head, op_group){

        list_del_init(&op_list->op_group);
        if(1 == op_list->is_new ){
            if(DIR_TYPE == op_list->file_type){

                worker = oper_malloc(pool->type, mkdir_direc, op_list, pool, group->opsgroups_index);                
            }
            else if(FILE_TYPE == op_list->file_type){
                
                worker = oper_malloc(pool->type, ceate_file, op_list, pool, group->opsgroups_index);
            }
            else{

                working_journal_log("worker", LOG_ERROR, "Cannot allocate worker, "
                                    "op_List values do not match op_list->path->%s ",
                                     op_list->path);
            }
        }else {
            if(DIR_TYPE == op_list->file_type){
                if(1 == op_list->is_need_open){

                    worker = oper_malloc(pool->type, need_open_direc, op_list, pool, group->opsgroups_index);
                }else{

                    worker = oper_malloc(pool->type, operation_direc, op_list, pool, group->opsgroups_index);
                }
                                
             }else if(FILE_TYPE == op_list->file_type){

                if(1 == op_list->is_need_open){
                    worker = oper_malloc(pool->type, need_open_file, op_list, pool, group->opsgroups_index);
                }else{

                    worker = oper_malloc(pool->type, operation_file, op_list, pool, group->opsgroups_index);
                }
             }else if(SYMLINK_TYPE == op_list->file_type){

                worker = oper_malloc(pool->type, symlink_operation, op_list, pool, group->opsgroups_index);

             }else{
                working_journal_log("worker", LOG_ERROR, "Cannot allocate worker, "
                                    "op_List values do not match op_list->path->%s ",
                                     op_list->path); 
            }
        }

        if(!worker){
            continue;
        }

/*add_worker_to_list:*/
        pthread_mutex_lock(&pool->mutex);
        if(1 == pool->destroy){
            free_oper_worker(worker);           
            goto unlock;
        }
        list_add_tail(&worker->list,&pool->worker_list);
        pool->worker_size++;
        pthread_mutex_unlock(&pool->mutex);
        worker = NULL;
    }
    pthread_mutex_lock(&pool->mutex);
    pool->building = 0;
    free_journal_group_mem(group);
unlock:
    pthread_mutex_unlock(&pool->mutex);
    return 0;
}
worker_list *pool_oper_worker(thread_pool *pool)
{

    int                     ret = 0;
    worker_list             *worker = NULL;

    pthread_mutex_lock(&pool->mutex);
    if(1 == pool->destroy)
        goto unlock;

    while(list_empty(&pool->worker_list)) {

        if(!list_empty(&pool->worker_list))
            break;

        if(1 == pool->destroy)
            goto unlock;

        pthread_mutex_unlock(&pool->mutex);
        ret = build_worker_queue(pool);
        if(1 == ret ) {
            pthread_mutex_lock(&manager->mutex);
            if(pool->type == disk_type)
                manager->mask[0] = '1';
            else
                manager->mask[1] = '1';
            working_journal_log("manager pool", LOG_ERROR, "mask[0]->%c,mask->%c,destroy_group->%u,destroy_op_list->%u",
                                 manager->mask[0], manager->mask[0] ,manager->destroy_group, manager->destroy_op_list);
            pthread_mutex_unlock(&manager->mutex);
            return NULL;
        }
        sleep(2);
        pthread_mutex_lock(&pool->mutex);
    }

    worker = list_entry(pool->worker_list.next, worker_list, list);
    list_del_init(&worker->list);
    pool->worker_size--;
unlock:
    pthread_mutex_unlock(&pool->mutex);
    return worker;
}

int update_disk_min_error(worker_list *worker, error_path_list *date, int error_num)
{

    int                             sum   = 0;
    if(!worker)
        return 0;

    pthread_mutex_lock(&all_error_list->mutex);
    list_add_tail(&date->list, &all_error_list->disk_list);
    sum = ++all_error_list->disksum;

    if(sum >= error_num){
        if(all_error_list->disk_have_destroy){
            if( date->group_index < all_error_list->disk_min_group ){

                all_error_list->disk_min_group = date->group_index;
                all_error_list->disk_min_op_list = date->op_list_index;
                all_error_list->disk_min_op = date->op_index;
             }else if(date->group_index == all_error_list->disk_min_group){

                if(date->op_list_index <  all_error_list->disk_min_op_list){

                    all_error_list->disk_min_group = date->group_index;
                    all_error_list->disk_min_op_list = date->op_list_index;
                    all_error_list->disk_min_op = date->op_index;
                }
            }
            pthread_mutex_unlock(&all_error_list->mutex);
            goto error;

        }else{
            all_error_list->disk_have_destroy = 1;
            all_error_list->disk_min_group = worker->group_index;
            all_error_list->disk_min_op_list = worker->op_list_index;
            all_error_list->disk_min_op = worker->op_index;
            pthread_mutex_unlock(&all_error_list->mutex);

            pthread_mutex_lock(&manager->mutex);
            manager->mask[0]   = '1';
            manager->normal[0] = '1';
            manager->destroy_group = all_error_list->disk_min_group;
            manager->destroy_op_list = all_error_list->disk_min_op_list;
            pthread_mutex_unlock(&manager->mutex);

            working_journal_log("manager disk to wrong number ", LOG_ERROR, 
                                    "mask[0]->%c,mask->%c,destroy_group->%u,destroy_op_list->%u",
                                 manager->mask[0], manager->mask[0] ,manager->destroy_group, manager->destroy_op_list);
                        goto error;
        }
    }

    pthread_mutex_unlock(&all_error_list->mutex);
    return 0;

error:
    return -1;
}

int update_digi_min_error(worker_list *worker, error_path_list *date, int error_sum)
{

    int                             sum   = 0;
    int                             ret   = 0;

    if(!worker){
        ret = 0;
        goto error;
    }

    pthread_mutex_lock(&all_error_list->mutex);
    list_add_tail(&date->list, &all_error_list->digi_list);
    sum = ++all_error_list->digisum;

    if(sum >= error_sum){
        if(all_error_list->digi_have_destroy){
            if( date->group_index < all_error_list->disk_min_group ){

                all_error_list->digi_min_group = date->group_index;
                all_error_list->digi_min_op_list = date->op_list_index;
                all_error_list->digi_min_op = date->op_index;
            }else if(date->group_index == all_error_list->disk_min_group){

                if(date->op_list_index <  all_error_list->disk_min_op_list){

                    all_error_list->digi_min_group = date->group_index;
                    all_error_list->digi_min_op_list = date->op_list_index;
                    all_error_list->digi_min_op = date->op_index;
                }
           }
           pthread_mutex_unlock(&all_error_list->mutex);
           ret = -1;
           goto error;

        }else{
            all_error_list->digi_have_destroy = 1;
            all_error_list->digi_min_group = worker->group_index;
            all_error_list->digi_min_op_list = worker->op_list_index;
            all_error_list->digi_min_op = worker->op_index;
            pthread_mutex_unlock(&all_error_list->mutex);

            pthread_mutex_lock(&manager->mutex);
            manager->destroy_group = all_error_list->disk_min_group;
            manager->destroy_op_list = all_error_list->disk_min_op_list;
            manager->mask[0] = '1';
            manager->normal[0] = '1';
            pthread_mutex_unlock(&manager->mutex);
            working_journal_log("manager digioceand to wrong number ", LOG_ERROR,
                                 "mask[0]->%c,mask->%c,destroy_group->%u,destroy_op_list->%u",
                                 manager->mask[0], manager->mask[0] ,manager->destroy_group, manager->destroy_op_list);
            ret = -1;
            goto error;
       }
    }

    pthread_mutex_unlock(&all_error_list->mutex);
    return ret;

error:
    return ret;
}

int file_add_error_list(worker_list *worker, int error_num)
{

    error_path_list *date       = NULL;
    char            buf[4096]   = "\0";

    date = (error_path_list *)malloc(sizeof(error_path_list));
    if(!date){
        working_journal_log("error_path_list malloc ", LOG_ERROR, 
                    "error_path_list malloc failure");
        goto error;
    }

    INIT_LIST_HEAD(&date->list);
    date->path = malloc(sizeof(char)*strlen(worker->abs_path)+1);
    if(!date->path){

        working_journal_log("error_path_list malloc ", LOG_ERROR, 
                    "error_path_list->path  malloc  failure");
        free(date);
        goto error;
    }
    strcpy(date->path,worker->abs_path);
    date->ret = worker->ret;
    date->tmp_errno = worker->tmp_errno;
    date->group_index = worker->group_index;
    date->op_list_index = worker->op_list_index;
/*Write error diary*/

    sprintf(buf,"path->%s,ret->%d,error->%d,group_index->%u,op_list_index->%u,op_index->%u",
        worker->abs_path, worker->ret,worker->tmp_errno,
        worker->group_index,worker->op_list_index,worker->op_index );
    result_handle_fn(buf);
/*Error handling*/

    if(worker->type == disk_type)
        return update_disk_min_error(worker, date, error_num);
    else
        return update_digi_min_error(worker, date, error_num);
    return 0;
error:
    return -1;

}

int  pool_oper_tolerate(thread_pool *pool, worker_list *worker)
{


    unsigned int        sum = pool->perform_sum;
    if(sum %pool_percentage == 0){
        int i = sum/pool_percentage;
        i*=5;
        printf(".........%d%%\n",i);
    }

    if(-1 == worker->ret && worker->tmp_errno != ENOENT){

        if(pool->error_type == all_stop)
            return -1;
        
        return  file_add_error_list(worker, max_error_range);

    }

    return 0;
}


int pool_oper_increase(thread_pool *pool, worker_list *worker)
{

    int                 ret   = -1;
    unsigned int        worker_size = 0;

/*  add_worker */
    pthread_mutex_lock(&pool->mutex);
    worker_size = pool->worker_size;
    pthread_mutex_unlock(&pool->mutex); 
    if(worker_size < min_worker_queue){

        ret = build_worker_queue(pool);
    }
/*  Read group end*/
    if(1 == ret){
        /* is executive group number in the configuration file*/
        pthread_mutex_lock(&manager->mutex);
        if(pool->type == disk_type)
            manager->mask[0] = '1';
        else
            manager->mask[1] = '1';
        pthread_mutex_unlock(&manager->mutex);
    }
    return 0;
}

int  file_choose_insert(worker_list  *worker)
{

    pthread_mutex_lock(&manager->mutex);
    if(worker->group_index > manager->group_index){                                                                                                                                      
        manager->group_index = worker->group_index;
        manager->op_list_index = worker->op_list_index;

        working_journal_log("manager max op_List ", LOG_DEBUG, 
                "manager->group_index = %u  manager->op_list_index = %u",
                manager->group_index, manager->op_list_index);

        pthread_mutex_unlock(&manager->mutex);
    }else if(worker->group_index == manager->group_index && worker->op_list_index >manager->op_list_index ){                                                                             
        manager->group_index = worker->group_index;
        manager->op_list_index = worker->op_list_index;

        working_journal_log("manager max op_List ", LOG_DEBUG, 
                            "manager->group_index = %u  manager->op_list_index = %u",                                                                    
                             manager->group_index, manager->op_list_index);

        pthread_mutex_unlock(&manager->mutex);
   }else{
        pthread_mutex_unlock(&manager->mutex);

            
        worker_move_list(worker);
   }

   return 1;
}


void *oper_thread(void  *date)
{
    int             ret     = 0;
    worker_list     *worker = NULL;
    thread_pool     *pool   = (thread_pool *)date;
    struct statvfs  buf;
    int             percent = 0;
    journal_file_op_list_t  *op_list = NULL;

    if(!pool){
        working_journal_log("pool is NULL", LOG_ERROR, 
                    "thread is  %u",(unsigned int)pthread_self());
        pthread_exit(NULL);
    }

    while(1){
        worker = pool_oper_worker(pool);
        if(!worker)
            pthread_exit(NULL);
        ret = worker->fn(worker->args, worker->abs_path, &worker->ret, &worker->tmp_errno);

        op_list = (journal_file_op_list_t *)worker->args;
        worker->op_index = op_list->perform_op_index;

        working_journal_log("pool_thread", LOG_DEBUG, "Perform the end path->%s "
                            " ret->%d  errno->%d pool->type->%d , worker->group_index->%u"
                            " worker->op_list_index->%u,  worker->op_type->%u", 
                            worker->abs_path, worker->ret, worker->tmp_errno,pool->type,
                            worker->group_index,worker->op_list_index,worker->op_type);

        pthread_mutex_lock(&pool->mutex);
        pool->perform_sum++;
        if(worker->group_index > pool->group_index){

            pool->group_index = worker->group_index;
            pool->op_list_index = worker->op_list_index;

            working_journal_log("pool_thread", LOG_DEBUG, "update thread pool  max num "
                        "pool->group_index->%u  pool->op_list_index->%u ",
                        pool->group_index,pool->op_list_index);
        }else if(worker->group_index == pool->group_index){

            pool->group_index = worker->group_index;
            pool->op_list_index = worker->op_list_index;

            working_journal_log("pool_thread", LOG_DEBUG, "update thread pool  max num "
                                                "pool->group_index->%u  pool->op_list_index->%u ",
                                                pool->group_index,pool->op_list_index);
        }else{
            ;
        }

        ret = statvfs(worker->abs_path,&buf);
        if(ret == 0){

            percent = (buf.f_blocks - buf.f_bfree) * 100 / (buf.f_blocks - buf.f_bfree + buf.f_bavail)+1;
            if( percent > 80){

                working_journal_log("Disk space", LOG_ERROR, "Insufficient disk space remaining  Have been using: %d%%",percent);
                printf("Insufficient disk space remaining  Have been using  %d%%\n",percent);
            }

        }

        pthread_mutex_unlock(&pool->mutex);

        file_choose_insert(worker);
        ret = pool_oper_tolerate(pool,worker);
        if(ret == -1){
            free_oper_worker(worker);
            pthread_exit(NULL);
        }

        pool_oper_increase(pool,worker);
        free_oper_worker(worker);

    }
}

/*Add class state machine for the manager*/

manager_state check_pool_state()
{

    if('0' == manager->mask[0]  && '0' == manager->mask[1]){
        return survive;
    }else{
        return destroy; 
    }
}

manager_state check_pool_destroy()
{

    if('2' != manager->mask[0]  && '2' != manager->mask[1]){
        return will_destroy;
    }else{
        return have_destroy;
    }
}

manager_state check_pool_will_destroy()
{

    int count = 0 ;
    
    if('1' == manager->mask[0]){
        pthread_mutex_unlock(&manager->mutex);
        pool_destroy(manager->disk_pool);
        pthread_mutex_lock(&manager->mutex);
        manager->mask[0] = '2';
        count++;
        if('2' == manager->mask[1])
            return all_destroy;
    }
    if('1' == manager->mask[1]){
        pthread_mutex_unlock(&manager->mutex);
        pool_destroy(manager->digi_pool);
        pthread_mutex_lock(&manager->mutex);
        manager->mask[1] = '2';
        count++;
        if('2' == manager->mask[0])
            return all_destroy;
    }
    if(2 == count )
        return all_destroy;
    return have_destroy;
}

manager_state check_pool_have_destroy(int  *which)
{
    if('2' == manager->mask[0]){
        *which = 0;
        return notice_other;
    }
    else{

        *which = 1;
        return notice_other;
    }
    
    
}

manager_state check_pool_notice_destroy(int which)
{

    thread_pool *tmp_pool = NULL;
    int     whos = -1;
    if(which == 0){

        if('1' == manager->mask[1]){

            pthread_mutex_unlock(&manager->mutex);
            pool_destroy(manager->digi_pool);
            pthread_mutex_lock(&manager->mutex);
            return all_destroy;
        }

        if('1' == manager->normal[0] ){

            tmp_pool = manager->digi_pool;
            whos = 1;
            pthread_mutex_lock(&tmp_pool->mutex);
            if(tmp_pool->group_index > manager->destroy_group){
                goto all_dest;
            }else if(tmp_pool->group_index == manager->destroy_group){
                if(tmp_pool->op_list_index >= manager->destroy_op_list){
                    goto all_dest;
                }
            }else
                goto will_cont;
        }else{
            return will_continue;
        }
    }else{

        if('1' == manager->mask[0]){

            pthread_mutex_unlock(&manager->mutex);
            pool_destroy(manager->disk_pool);
            pthread_mutex_lock(&manager->mutex);
            return all_destroy;
        }


        if('1' == manager->normal[1] ){
            tmp_pool = manager->disk_pool;
            whos = 0;
            pthread_mutex_lock(&tmp_pool->mutex);
            if(tmp_pool->group_index > manager->destroy_group){
                goto all_dest;
            }else if(tmp_pool->group_index == manager->destroy_group){
                if(tmp_pool->op_list_index >= manager->destroy_op_list){
                    goto all_dest;
                }
            }else{
                goto will_cont;
            }
        }else{
            return will_continue;
        }
    
    }
all_dest:
    pthread_mutex_unlock(&tmp_pool->mutex);
    pthread_mutex_unlock(&manager->mutex);
    pool_destroy(tmp_pool);
    pthread_mutex_lock(&manager->mutex);    
    if( 1 == whos)
        manager->mask[1] = '2';
    else if(0 == whos)
        manager->mask[0] = '2';
    else
        printf("******************************\n");
    return all_destroy;
will_cont:
    pthread_mutex_unlock(&tmp_pool->mutex);
    return will_continue;
}
/*Add class state machine for the manager*/

int  check_pool_all_destroy()
{

    if('0' == manager->normal[0] && '0' == manager->normal[1])
        return 1;
    return -1;
}

int get_manager_state(manager_state state)
{

    int     which = -1;
    while(1){
        switch(state){

            case survive:
                printf("in survive \n");
                return 0;
            case destroy:
                printf("in destroy \n");  
                state = check_pool_destroy();break;
            case will_destroy: 
                printf("will destroy \n");
                state = check_pool_will_destroy();break;
            case have_destroy: 
                printf("have destroy \n");
                state = check_pool_have_destroy(&which);break;
            case notice_other:
                printf("notice other \n");
                state = check_pool_notice_destroy(which);break;
            case all_destroy:  
                printf("all destroy \n");
                return check_pool_all_destroy();break;
            case will_continue: 
                printf("will continue \n");
                return 0;

            default :printf("state  error\n");return 0;
        }
    }
}


