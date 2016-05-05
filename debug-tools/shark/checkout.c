#include "checkout.h"
#include <openssl/md5.h>
#include "analyze.h"

#include "log.h"

const  char *remove_xattr_selinux = "security.selinux";

extern thread_pool     *check_pool;
extern int             check_flag;
extern error_list      *all_error_list;
extern int             digioceand_mount_size;
extern unsigned int    check_max_depth;
unsigned int           check_pool_percentage = 0;


void checkout_pool_destroy()
{

    int     i = 0;
    if(check_pool->destroy)
        return ;
    check_pool->destroy = 1;
    pthread_cond_broadcast(&check_pool->cond);
    for(i = 0; i < check_pool->thread_size; i++ ){

        pthread_join(check_pool->thread[i],NULL);
    }
    free(check_pool->thread);

    free_check_list(check_pool);
    pthread_mutex_destroy(&check_pool->mutex);
    pthread_cond_destroy(&check_pool->cond);
    free(check_pool);
    check_pool = NULL;
}

void check_pool_init(const char *mount_path, worker_type type)
{
    int                     i     = 0;
    int                     ret   = 0;

    if(!mount_path){

        working_journal_log("checkout pool", LOG_ERROR, 
                   "checkout mount is NULL");
        goto error;
    }

    check_pool = (thread_pool *)malloc(sizeof(thread_pool));
    if(!check_pool){

        working_journal_log("checkout pool", LOG_ERROR,
                    "checkout pool malloc is failer");
        goto error;
    }

    check_pool->thread_size = conf_thread_size;
    pthread_mutex_init(&check_pool->mutex,NULL);
    pthread_cond_init(&check_pool->cond,NULL);
    INIT_LIST_HEAD(&check_pool->worker_list);
    check_pool->worker_size = 0;
    check_pool->type = type;
    check_pool->group_read = 1;
    check_pool->perform_sum = 0;
    check_pool->destroy    = 0;
    check_pool->check_total_file = 0;
/* 
    check_pool->path_mount =(char *)malloc(sizeof(char)*(strlen(mount_path)+1));
 
    if(!check_pool->path_mount){
        working_journal_log("checkout pool", LOG_ERROR,
                      "check_pool->path_mount malloc failer");
        goto error;
    }
    if(!strcpy(check_pool->path_mount, mount_path)){
        working_journal_log("checkout pool", LOG_ERROR,
                      "check_pool->path_mount strcpy is failer");
        goto error;
    }
*/
    check_pool->thread = malloc(sizeof(pthread_t)*check_pool->thread_size);
    if(!check_pool->thread){
        working_journal_log("checkout pool", LOG_ERROR,
                      "check_pool->thread  malloc failer");
        goto error;
    }

    for(i = 0; i < check_pool->thread_size; i++){
        if(check_pool->type == check_type){

            working_journal_log("checkout pool", LOG_DEBUG,
                        "Starting a thread  %d",i);
            ret = pthread_create(&check_pool->thread[i], NULL,(void *)check_thread, NULL);
        }
        if(ret != 0){
            working_journal_log("checkout pool", LOG_ERROR,
                        "Starting a thread  %d  failer", i);
            goto failer_thread;
        }
   }

error:

   return ;
failer_thread:
   checkout_pool_destroy(check_pool);
   return ;
}

int free_check(checkout_date *date)
{

    xattr_link      *xattr      = NULL;
    xattr_link      *tmp_xattr  = NULL;

    if(!date)
        goto error;
    free(date->check_path);
    free(date->disk_path);

    list_for_each_entry_safe(xattr,tmp_xattr,&date->check_list,list){

        list_del_init(&xattr->list);
        free(xattr->key);
        free(xattr->value);
        free(xattr);
    }

    list_for_each_entry_safe(xattr,tmp_xattr,&date->disk_list,list){

        list_del_init(&xattr->list);
        free(xattr->key);
        free(xattr->value);
        free(xattr);
    }
    return 0;

error:
    working_journal_log("checkout pool", LOG_ERROR,
               "checkout_date  is NULL");
    return 0;
}

int free_check_worker(worker_list *worker)
{

    if (!worker){
        goto error;
    }

    if (worker->args) {
        free_check((checkout_date *)worker->args);
        free(worker->args);
    }
    free(worker);
    return 0;
error:
    working_journal_log("checkout pool", LOG_ERROR,
                "worker_list is NULL");
    return -1;
}

int free_check_list()
{

    worker_list     *worker     = NULL;
    worker_list     *worker_tmp = NULL;


    pthread_mutex_lock(&check_pool->mutex);
    list_for_each_entry_safe(worker, worker_tmp, &check_pool->worker_list, list){ 

        list_del_init(&worker->list);
        free_check_worker(worker);
    }
    pthread_mutex_unlock(&check_pool->mutex);

    return 0;
}

checkout_date *check_malloc(const char *check_path,const char *disk_path, checkout_state status)
{

    checkout_date                *date = NULL;


    if(!check_path || !disk_path){

        working_journal_log("checkout pool",LOG_ERROR,  
             "Path does not completely check_path->%s, disk_path->%s",
                check_path,disk_path);
        goto validation;
    }

    date = (checkout_date *)malloc(sizeof(checkout_date));
    if(!date){
        working_journal_log("checkout pool",LOG_ERROR,
                "checkout_date  malloc failer");
        goto error;
    }
    date->check_path = malloc(sizeof(char)*(strlen(check_path) + 1));
    date->disk_path  = malloc(sizeof(char)*(strlen(disk_path) + 1));

    if(!date->check_path || !date->disk_path){
        working_journal_log("checkout pool",LOG_ERROR,
                "malloc date path  mount  failer");
        goto error;
    }

    strcpy(date->check_path, check_path);
    strcpy(date->disk_path, disk_path);
    INIT_LIST_HEAD(&date->check_list);
    INIT_LIST_HEAD(&date->disk_list);
    INIT_LIST_HEAD(&date->list);
    date->type = status;

validation:
    return date;
error:

    if (date){

        if(date->check_path){
            free(date->check_path);
        }
        if(date->disk_path){
            free(date->disk_path);
        }
        free(date);
    }
    return NULL;
}

worker_list *check_worker_malloc(worker_type type, process pro, void *args)
{

    worker_list             *worker = NULL;

    if(!args){
        working_journal_log("checkout pool",LOG_ERROR,
                "will malloc worker_list but args is NULL");
        goto error;
    }

    worker = (worker_list *)malloc(sizeof(worker_list));
    if(!worker){
        working_journal_log("checkout pool",LOG_ERROR,
                "malloc worker is failer");
        goto error;
    }

    worker->type = type;
    INIT_LIST_HEAD(&worker->list);
    worker->fn = pro;
    worker->args = args;
    worker->ret = -1;
    worker->tmp_errno = 0;

    return worker;
error:
    return NULL;
}


worker_list *pool_check_worker()
{

    worker_list                     *worker = NULL;

    pthread_mutex_lock(&check_pool->mutex);
    if(1 == check_pool->destroy){
        goto unlock;
    }
    while(list_empty(&check_pool->worker_list)){

        if(1 == check_pool->destroy){
            working_journal_log("check_pool destroy", LOG_INFO, 
                            "check_pool will destroy");
            goto unlock;
        }

        if(!list_empty(&check_pool->worker_list))
            break;

        if(check_flag == 2){
            goto unlock;
        }
        pthread_mutex_unlock(&check_pool->mutex);
        sleep(2);
        pthread_mutex_lock(&check_pool->mutex);

    }

    worker = list_entry(check_pool->worker_list.next, worker_list, list);
    list_del(&worker->list);
    check_pool->worker_size--;
unlock:
    pthread_mutex_unlock(&check_pool->mutex);
    return worker;
}

int pool_check_change(worker_list *worker)
{

    error_path_list     *error_date = NULL;
    checkout_date       *date       = NULL;
    unsigned int        sum         = 0;
    int                 ret         = -1;

    if(!worker){

        working_journal_log("check_pool change", LOG_INFO,
            "worker is NULL" );
        ret = 0;
        goto error;
    }

    sum = check_pool->perform_sum;

    if(worker->ret == 0){

        if(!check_pool_percentage){
            ret = 0;
            goto error;
        }
        if(sum %check_pool_percentage == 0){
            int     i = sum/check_pool_percentage;
            i*=5;
            printf(".........%d%%\n",i);
        }
        ret = 0;
        goto error;
    }

    if(worker->ret == -1 ){


        date = (checkout_date *)worker->args;
        pthread_mutex_lock(&all_error_list->mutex);
        list_for_each_entry(error_date, &all_error_list->disk_list, list){

            if(strcmp(error_date->path, date->disk_path) == 0){
                working_journal_log("check_pool change", LOG_INFO,
                        "error list have the path->%s", error_date->path);
                ret = 0;
                goto unlock;
            }
        }
        list_for_each_entry(error_date, &all_error_list->digi_list, list){

            if(strcmp(error_date->path, date->check_path) == 0){

                working_journal_log("check_pool change", LOG_INFO,
                        "error list have the path->%s", error_date->path);
                ret = 0;
                goto unlock;
            }
        }
        ret = -1;
   }

unlock:
    pthread_mutex_unlock(&all_error_list->mutex);
error:
    return ret;
}

void check_thread(void)
{

    int             ret             = 0;
    worker_list     *worker         = NULL;
    char            buf[PATH_MAX]   = "\0";

    while(1){

        worker = pool_check_worker();
        if(!worker){
            if(check_flag == 2){
                working_journal_log("check_pool thread", LOG_INFO,
                    "Traversal is over, take a callback function NULL"
                    "so set check_flag = 1 and exit");
                check_flag = 1;
            }
            pthread_exit(NULL);
        }
        
        worker->fn(worker->args, NULL, &worker->ret, &worker->tmp_errno);

        working_journal_log("check_pool perform", LOG_DEBUG,
                " check_pool perform worker");

        if(1 == check_pool->destroy){
            working_journal_log("check_pool thread", LOG_INFO,
                "Thread will be destroyed so %u exit",
                (unsigned int )pthread_self());
            pthread_exit(NULL);
        }

        check_pool->perform_sum++;

        ret = pool_check_change(worker);
        if(ret == -1){
            check_flag = 1;
            sprintf(buf, "checkout error: path-->%s",((checkout_date *)worker->args)->check_path);
            check_handle_fn(buf);
            free_check_worker(worker);
            pthread_exit(NULL);
        }

        free_check_worker(worker);  

    }
}


int check_stat_compare(struct stat buf1, struct stat buf2)
{

    if(buf1.st_uid != buf2.st_uid){
        goto error;
    }
    if(buf1.st_gid != buf2.st_gid){
        goto error;
    }
    if(buf1.st_mode != buf2.st_mode){
        goto error;
    }

    return 0;
error:

    working_journal_log("check_pool thread", LOG_ERROR,
        "STAT is not equal");
    return -1;
}

xattr_link *xattr_malloc(size_t key_len, size_t value)
{

    xattr_link      *xattr = NULL;

    xattr = (xattr_link *)malloc(sizeof(xattr_link));
    if(!xattr){

        working_journal_log("check_pool xattr", LOG_ERROR,
                "malloc xattr failer");
        goto error;
    }
    xattr->key = malloc(sizeof(char)*key_len);
    if (!xattr->key){
        free(xattr);
        xattr = NULL;
        goto error;
    }
    xattr->value = malloc(sizeof(char)*value);
    if(!xattr->value){
        free(xattr->key);
        free(xattr);
        xattr = NULL;
        goto error;
    }
    INIT_LIST_HEAD(&xattr->list);

error:
    return xattr;
}

int xattr_get(checkout_date *date, int choose, int *sum)
{

    ssize_t     buflen, keylen, vallen;
    char        *buf, *key, *val;
    xattr_link  *check_date = NULL;
    char        *tmp_mount  = NULL;

    int         tmp_sum = 0;

    if(0 == choose)
        tmp_mount = date->check_path;
    else
        tmp_mount = date->disk_path;

    buflen = listxattr(tmp_mount, NULL, 0);
    if(-1 == buflen)
        goto posix_error;

    if(0 == buflen)
        return  tmp_sum;

    buf = malloc(buflen);
    if(!buf)
        goto posix_error;

    buflen = listxattr(tmp_mount, buf, buflen);
    if(-1 == buflen)
        goto posix_error;

    key = buf;
    while(buflen > 0){

        vallen = getxattr(tmp_mount, key, NULL, 0);
        if(vallen == -1)
            goto posix_error;

        if(vallen>0){
            val = malloc(vallen+1);
            if(val == NULL)
                goto posix_error;

            vallen = getxattr(tmp_mount, key, val, vallen);
            if(vallen == -1)
                goto posix_error;
            else
                val[vallen] = '\0';
            if(strcmp(key,remove_xattr_selinux) != 0){
                keylen = strlen(key) + 1;
                check_date = xattr_malloc(keylen, vallen+1);
                            strcpy(check_date->key, key);
                            strcpy(check_date->value, val);
                if(0 == choose)
                    list_add_tail(&check_date->list, &date->check_list);
                else
                    list_add_tail(&check_date->list, &date->disk_list);
            }

        }else if(vallen == 0){

            val = malloc(3);
            strcpy(val,"no");
        }
        keylen  = strlen(key) + 1;
        buflen -= keylen;
        key    += keylen;
        if(val)
            free(val);
        tmp_sum++;
    }
    *sum = tmp_sum;
    free(buf);
    return 0;
posix_error:
    return -1;
}

int check_xattr_get(checkout_date *date)
{
    int         ret = -1;
    int         sum1 = 0;
    int         sum2 = 0;

    ret = xattr_get(date,0,&sum1);
    if(ret != 0){
        goto error;
    }

    ret = xattr_get(date,1,&sum2);
    if(ret != 0){
        goto error;
    }

    if(sum1 != sum2){
        goto error;
    }

    ret = 0;
error:
    return ret;
}

int check_xattr_compare(checkout_date *date)
{

    int             ret  = 0;
    int             flag = 0;
    xattr_link      *src_link = NULL;
    xattr_link      *dst_link = NULL;

    if(!date){
        goto error;
    }


    list_for_each_entry(src_link, &date->disk_list, list){

        list_for_each_entry(dst_link, &date->check_list, list){

            if(0 == strcmp(src_link->key, dst_link->key)){
                if(0 == strcmp(src_link->value, dst_link->value)){
                    flag = 1;
                    break;
                }else{
                    ret = -1;
                    goto error;
                }
                    
           }
        }
        if(1 == flag){
            flag = 0;
            continue;
        }else{
            ret = -1;
            goto error;
        }
    }
error:
    return ret;
}

int check_op_direc(void *args,const char *path, int *ret, int *tmp_errno)
{

    checkout_date           *date = (checkout_date *)args;

    if(!date){

        working_journal_log("checkout check_op_direc", LOG_ERROR,
                            " Data types or path is empty ");
        return 1;
    }

    *ret = lstat(date->check_path, &date->stats[0]);
    if(*ret != 0){

        working_journal_log("checkout check_op_direc", LOG_ERROR,
                             "path:%s lstat failer", date->check_path);
        goto posix_error;
    }

    *ret = lstat(date->disk_path, &date->stats[1]);
    if(*ret != 0){

        working_journal_log("checkout check_op_direc", LOG_ERROR, 
                            "path:%s lstat failer", date->disk_path);
        goto posix_error;
    }

    *ret = check_stat_compare(date->stats[0], date->stats[1]);
    if(*ret != 0){

        working_journal_log("checkout check_op_direc", LOG_ERROR, 
                    "path->%s  stat  is  fail", date->check_path);
        goto check_error;
    }

    *ret = check_xattr_get(date);
    if(*ret != 0){

        working_journal_log("checkout check_op_direc", LOG_ERROR, 
                "path->%s  getxattr   is  fail",date->check_path);
        goto check_error;
    }

    *ret = check_xattr_compare(date);
    if(*ret !=0){

        working_journal_log("checkout check_op_direc", LOG_ERROR, 
              "path->%s  xattr Comparative errors   is  fail",date->check_path);
        goto check_error;
    }

    return *ret;

posix_error:
    *tmp_errno = errno;
    *ret = -1;
    return *ret;

check_error:

    *ret = -1;
    return *ret;
}

int check_op_symlink(void *args,const char *path, int *ret, int *tmp_errno)
{

    checkout_date       *date = (checkout_date *)args;

    if(!date){

        working_journal_log("checkout check_op_symlink", 
                LOG_ERROR, " Data types or path is empty ");
        return 1;
    }

    *ret = lstat(date->check_path, &date->stats[0]);
    if(*ret != 0){

        working_journal_log("checkout check_op_symlink", 
                LOG_ERROR, "path->%s   lstat  failer",
                                    date->check_path);

        goto posix_error;
    }

    *ret = lstat(date->disk_path, &date->stats[1]);
    if(*ret != 0){

        working_journal_log("checkout check_op_symlink", 
                    LOG_ERROR, "path->%s   lstat  failer",
                                        date->disk_path);
        goto posix_error;
    }

    *ret = check_stat_compare(date->stats[0], date->stats[1]);
    if(*ret != 0){

        working_journal_log("checkout check_op_symlink", 
                LOG_ERROR, "path->%s  stat  is  fail",
                                    date->check_path);
        goto check_error;
    }

    *ret = 0;
    return *ret;
posix_error:
    working_journal_log("checkout check_op_symlink", 
                LOG_ERROR, "Call posix error function  path->%s ", 
                                            date->check_path);
    *tmp_errno = errno;
    *ret = -1;
    return *ret;

check_error:
    *ret = -1;
    return *ret;
}

void check_file_md5(char *path, unsigned char   md5[16])
{

    int         fd      = -1;
    int         nread   = -1;
    MD5_CTX     ctx;
    char        data_buf[65536] = "\0";


    if(!path || path[0] != '/')
        return ;

    fd = open(path, O_RDONLY); 
    if(!fd)
        return ;

    MD5_Init(&ctx);
    while( (nread = read(fd, data_buf, sizeof(data_buf))) > 0 ){

        MD5_Update(&ctx, data_buf, nread); 
    }

    MD5_Final(md5, &ctx);
    close(fd);
}

int check_op_file(void *args,const char *path, int *ret, int *tmp_errno)
{

    checkout_date       *date = (checkout_date *)args;

    unsigned char       src_md5[16] = "\0";
    unsigned char       dst_md5[16] = "\0";

    int     i = 0;
    char    tmp_src_buf[4]  = "\0";
    char    src_buf[64]     = "\0" ;
    char    tmp_dst_buf[4]  = "\0";
    char    dst_buf[64]     = "\0";

    if(!date){

        working_journal_log("checkout check_op_file", LOG_ERROR,
                             " Data types or path is empty ");
        return -1;
    }

    *ret = lstat(date->check_path, &date->stats[0]);
    if(*ret != 0){

        working_journal_log("checkout check_op_file", LOG_ERROR, 
                    "path->%s  lstat  is  failer  ", date->check_path);
        goto posix_error;
    }

    *ret = lstat(date->disk_path, &date->stats[1]);
    if(*ret != 0){

        working_journal_log("checkout check_op_file", LOG_ERROR,
                      "path->%s  lstat  is  failer  ", date->disk_path);
        goto posix_error;
    }


    *ret = check_stat_compare(date->stats[0], date->stats[1]);
    if(*ret != 0){

        working_journal_log("checkout check_op_file", LOG_ERROR, 
                            "path->%s  stat  is  fail",date->disk_path);
        goto check_error;
    }

    *ret = check_xattr_get(date);
    if(*ret != 0){
        working_journal_log("checkout check_op_file", LOG_ERROR,
                        "path->%s  getxattr   is  fail",date->disk_path);
        goto check_error;
    }

    *ret = check_xattr_compare(date);

     
    if(*ret !=0){
        working_journal_log("checkout check_op_file", LOG_ERROR, 
                "path->%s  xattr Comparative errors   is  fail",
                                                date->disk_path);
        goto check_error;
    }

    check_file_md5(date->disk_path, src_md5);
    for(i=0 ;i<sizeof(src_md5);i++){

        sprintf(tmp_src_buf,"%02x",src_md5[i]);
        strcat(src_buf,tmp_src_buf);
    }

    check_file_md5(date->check_path, dst_md5);
    for(i=0 ;i<sizeof(dst_md5);i++){

        sprintf(tmp_dst_buf,"%02x",dst_md5[i]);
                strcat(dst_buf,tmp_dst_buf);
    }

    if(strcmp(src_buf, dst_buf) != 0){
        working_journal_log("checkout check_op_file", LOG_ERROR, 
                    "path->%s  ms5 Comparative errors   is  fail",
                                                date->check_path);
        goto check_error;
    }
    *ret = 0;
    return *ret;

posix_error:
    working_journal_log("checkout check_op_file", LOG_ERROR, 
                        "Call posix error function  path->%s ", 
                                                date->check_path);
    *tmp_errno = errno;
    *ret = -1;
    return *ret;

check_error:
    working_journal_log("checkout check_op_file", LOG_ERROR, 
                        "check_error   path->%s",date->check_path);
    *ret = -1;
    return *ret;
}



int traversal_mount(const char *path, int depth)
{

    int                ret = -1;
    DIR                *dir_dp = NULL;
    struct dirent      *entry  = NULL;
    struct stat        digistat;
    struct stat        diskstat;
    char               digi_buf[4096] = "\0";
    char               disk_buf[4096] = "\0";
    char               *tmp = NULL;
    checkout_date      *date = NULL;

    worker_list        *worker = NULL;
/*
        if(depth > check_max_depth)
        working_journal_log("checkout depth", LOG_ERROR, "Directory depth has exceeded the configuration file");
*/
    if((dir_dp = opendir(path)) == NULL){
        working_journal_log("checkout opendir",LOG_ERROR, 
                "Open the path to failure path->%s  errno-->%d ",
                                              path,errno);
        return -1;
    }

    while((entry = readdir(dir_dp)) != NULL){

        if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
            continue;

        strcpy(disk_buf,disk_mount);

        sprintf(digi_buf,"%s/%s",path,entry->d_name);

        ret = lstat(digi_buf,&digistat);
        if(ret != 0){

            working_journal_log("checkout  stat", LOG_ERROR, 
                            "lstat failure  path->%s errno->%d ",
                                                digi_buf,errno);
            continue;
        }

        /*Figure out whether the thread will be destroyed*/
        tmp = copy_str(digi_buf, digioceand_mount_size);
        if(!tmp){

            working_journal_log("checkout  stat", LOG_ERROR, 
                                        "copy_str  faile ");
        }
        strcat(disk_buf,tmp);
        free(tmp);

        if(S_ISDIR(digistat.st_mode)){

            ret = lstat(disk_buf, &diskstat);
            if(ret != 0){
                working_journal_log("checkout  stat", LOG_ERROR, 
                                    "stat failure  path->%s, %d",
                                                disk_buf,errno);
                continue;
            }

            if(!S_ISDIR(diskstat.st_mode)){

                working_journal_log("checkout dir", LOG_ERROR, 
                    "Do not match the file type  digi_buf->%s disk_buf->%s",
                                               digi_buf, disk_buf);
                continue;
            }

            date = check_malloc(digi_buf, disk_buf, check_is_direc);
            if(!date){

                working_journal_log("checkout checkout_date", LOG_ERROR , 
                                        "checkout_date malloc  failer ");
                continue;
            }

            worker = check_worker_malloc(check_type, check_op_direc, (void *)date);
            if(!worker){

                working_journal_log("checkout worker", LOG_ERROR , 
                                                "worker malloc failer ");
                continue;
            }

            pthread_mutex_lock(&check_pool->mutex);
            list_add_tail(&worker->list, &check_pool->worker_list);
            check_pool->worker_size++;
            check_pool->check_total_file++;
            pthread_mutex_unlock(&check_pool->mutex);

            if(check_pool->destroy){
                working_journal_log("checkout dir", LOG_ERROR,
                            "pool will destroy , path->%s",path);
                return 0;
            }

            depth++;
            ret = traversal_mount(digi_buf,depth);
            if(ret == -1){
                working_journal_log("checkout dir", LOG_ERROR, 
                        "traversal  failer  path->%s ",digi_buf);
                continue;
            }
        }else if(S_ISREG(digistat.st_mode)){

            ret = lstat(disk_buf,&diskstat);
            if(ret!=0){
                working_journal_log("checkout  stat", LOG_ERROR, 
                            "stat failure  path->%s errno->%d ",
                                                disk_buf,errno);
                continue;
            }

            if(!S_ISREG(digistat.st_mode)){
                working_journal_log("checkout file", LOG_ERROR, 
                    "Do not match the file type  digi_buf->%s disk_buf->%s",
                                            digi_buf, disk_buf);
                continue;
            }


            date = check_malloc(digi_buf,disk_buf, check_is_file);
            if(!date){

                working_journal_log("checkout checkout_date", LOG_ERROR , 
                                        "checkout_date malloc  failer ");
                continue;
            }

            worker = check_worker_malloc(check_type, check_op_file, (void *)date);
            if(!worker){

                working_journal_log("checkout worker", LOG_ERROR , 
                                            "worker malloc failer ");
                continue;
            }
 
            pthread_mutex_lock(&check_pool->mutex);
            list_add_tail(&worker->list, &check_pool->worker_list);
            check_pool->worker_size++;
            check_pool->check_total_file++;
            pthread_mutex_unlock(&check_pool->mutex);

            if(check_pool->destroy){
                return 0;
            }

        }else if (S_ISLNK(digistat.st_mode)){

            ret = lstat(disk_buf,&diskstat);
            if(ret!=0){
                working_journal_log("checkout  stat", LOG_ERROR, 
                                "stat failure  path->%s errno:%d",
                                                disk_buf, errno);
                continue;
            }

            if (!S_ISLNK(digistat.st_mode)){
                working_journal_log("checkout symlink", LOG_ERROR, 
                    "Do not match the file type  digi_buf->%s disk_buf->%s",
                                                        digi_buf, disk_buf);
                continue;
            }

            date = check_malloc(digi_buf,disk_buf, check_is_symlink);
            if(!date){
                working_journal_log("checkout checkout_date", LOG_ERROR , 
                                        "checkout_date malloc  failer ");
                continue;
            }

            worker = check_worker_malloc(check_type, check_op_symlink, (void *)date);
            if(!worker){
                working_journal_log("checkout worker", LOG_ERROR , 
                                            "worker malloc failer ");
                                continue;
            }

            pthread_mutex_lock(&check_pool->mutex);
            list_add_tail(&worker->list, &check_pool->worker_list);
            check_pool->worker_size++;
            check_pool->check_total_file++;
            pthread_mutex_unlock(&check_pool->mutex);
            if(check_pool->destroy){
                return 0;
            }
         
        }else{
            working_journal_log("checkout file type", LOG_ERROR , 
                   " Temporarily does not support the file type ");
        }
        worker = NULL;
        date   = NULL;
    }
    closedir(dir_dp);

    return 0;

}

void check_all(void)
{

    int                ret = -1;
    int                size_len = 0;
    char               buf[1024] = "\0";

    strcpy(buf,digioceand_mount);
    size_len = strlen(buf);
    buf[size_len-1] = '\0';
    working_journal_log("checkout traversal_mount", LOG_INFO , 
                    "traversal_mount  begin  path-->%s",buf );
    ret = traversal_mount(buf,0);
    if(ret == -1){

        working_journal_log("checkout traversal", LOG_ERROR, 
                                        "traversal faile");
    }
    check_pool_percentage =check_pool->check_total_file/20;
    check_flag = 2;
    working_journal_log("checkout traversal_mount", LOG_INFO ,
              "traversal  all file  %u", check_pool->check_total_file );
    printf("traversal done \n");
    printf("Began to perform checkout......\n");
    printf("total_file ----------->%u\n",check_pool->check_total_file );

}
