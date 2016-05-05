#include "shark_posix.h"
#include <fcntl.h>
#include "log.h"
#include "shark_file.h"
#include <limits.h>
#include <inttypes.h>


char write_buf[1024] = {'1',};
char read_buf[1024] = {'1',};

/*add by  2016-01-29*/

const char    xattr_key[1024]     = "user.key";
const char    xattr_value[1024]   = "value";
/*add by  2016-01-29*/

extern char            digioceand_mount[1024] ;
extern char            disk_mount[1024] ;
extern int             digioceand_mount_size ;
extern int             disk_mount_size;


int mobile_list(const char *path, int type, int choose)
{

    if(!path)
        return -1;
    file_node_t  *file_node =  NULL;
    char         file[1024] = "\0";
    int          i = 0,j = 0;
    int          size_len = 0;
    char         char_index[1024] = "\0";
    uint64_t     index = 0;
    int          ret = -1;


    if (!path) {
        goto quit;
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

    file_node = file_delete_form_rbt(type,index, 0);
    
    if (choose) {
        if (file_node) { 
            file_node_usecnt_decrease(file_node);
            file_insert_to_rbt(type,file_node, 1);
        }
        ret = 1;
    }else{
        if(file_node){
            file_node_usecnt_decrease(file_node);
            file_node_destory(file_node,file_node);
        }
        ret = 0;
    }

quit:
    return ret;
}


char *posix_copy_str(const char *path, int n)
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



int open_pwrite(int fd, off_t  offset,int sum,int *tmp_errno)
{

    int     ret = -1;
    struct  iovec   iov;

    memset(write_buf,'2',1024);
    iov.iov_base = write_buf;
    iov.iov_len  = 1024;
    while(sum--){
        ret = pwritev(fd,&iov,1,offset);
        if(-1 == ret){
            *tmp_errno = errno;
            return -1;
        }
    }
    return 0;
}

int open_preadv(int fd, off_t  offset, int sum, int *tmp_errno)
{

    int             ret = -1;
    struct  iovec   iov;
    struct  stat    buf;

    iov.iov_base    = read_buf;
    iov.iov_len     = 1024;
    
    ret = fstat(fd,&buf);
    if(ret != 0)
        return -1;
    if(buf.st_size < offset){
        ret = preadv(fd,&iov,1,0);
        if(-1 == ret ){
            *tmp_errno = errno;
            return -1;
        }
        if(0 == ret)
            return 0;
    }
    while(sum--){
        ret = preadv(fd,&iov,1,offset);
        if(-1 == ret ){
            *tmp_errno = errno;
            return -1;
        }
    }
    return 0;
}

int open_fallocate(int fd,off_t offset,off_t len,int mode, int *tmp_errno)
{

    int                 ret = -1;
    struct  stat        buf;

    ret = fstat(fd,&buf);
    if(-1 == ret)
        goto error;


    ret = fallocate(fd,mode,offset,512*1024);
    if(-1 == ret)
        goto error;

    return 0;
error:
    *tmp_errno = errno;
    return -1;
}

/* man  ftruncate */
/* The file offset is not changed */
int open_ftruncate(int fd,off_t file_size, int *tmp_errno)
{

    int                     ret = -1;
    struct  stat            buf;

    ret = fstat(fd,&buf);
    if(-1 == ret)
        goto error;

    ret = ftruncate(fd,file_size);
    if(-1 == ret)
        goto error;

    if(0 == ret && 0 == file_size)
        ret=lseek(fd,0,SEEK_SET);

    if(ret == 0 && buf.st_size < file_size){
        
        ret = lseek(fd,0,SEEK_END);
        write(fd,"a",1);
    }
    return 0;
error:
    *tmp_errno = errno;
    return -1;
}

int open_sync(int fd, int *tmp_errno)
{

    int                     ret = -1;

    ret = fsync(fd);
    if(-1 == ret)
        goto error;

    return 0;
error:
    *tmp_errno = errno;
     return -1;
}

int open_fgetxattr(int fd,const char *key, int *tmp_errno)
{

    size_t          size_len = -1;
    char            buf[1024] = "\0";

    *tmp_errno = 0;
    size_len = fgetxattr(fd, xattr_key, buf, 1024);
    *tmp_errno = errno;
    if(size_len == -1 && *tmp_errno ==  ENOATTR){
        return 0;
    }else if(size_len >0){
        return 1;
    }else{
        ;
    }

    return -1;

}

int open_fsetxattr(int fd, char *key, char *value, int flag, int *tmp_errno)
{

    int                 ret = -1;

    ret = open_fgetxattr(fd, NULL, tmp_errno);
    if(ret == -1){
        goto error;
    }
    if(ret == 1 ){
        ret = fsetxattr(fd,xattr_key,xattr_value,1024,XATTR_REPLACE);
        goto error;
    }

    if(ret == 0){
        
        ret = fsetxattr(fd,xattr_key,xattr_value,1024,XATTR_CREATE);
        goto error;
    }
    return 0;

error :
    *tmp_errno = errno;
    return ret;
}

int open_fremovexattr(int fd, const char *key, int *tmp_errno)
{

    int                     ret = -1;

    ret  = open_fgetxattr(fd, NULL, tmp_errno);
    if(ret == 1 )
        ret = fremovexattr(fd,xattr_key);
    if(ret == -1)
        goto error;

    return 0;
error:
    *tmp_errno = errno;
    return -1;
    
}

int open_fchown(int fd, uid_t owner,gid_t group, int *tmp_errno)
{

    int         ret = -1;
    ret  = fchown(fd, owner, group);
    if(ret == -1)
        goto error;
    return 0;
error:
    *tmp_errno = errno;
    return -1;
}

int open_fchmod(int fd, mode_t  mode , int *tmp_errno)
{

    int                     ret = -1;
    ret = fchmod(fd,mode);
    if(ret == -1)
        goto error;
    return 0;
error:
    *tmp_errno = errno;
    return -1;
}

int open_futimes(int fd, struct timeval  *time, int *tmp_errno)
{
    int         ret = -1;
    ret = futimes(fd, time);
    if(ret == -1)
        goto error;
    return 0;
error:
    *tmp_errno = errno;
    return -1;
}

int open_fstat(int fd, int *tmp_errno)
{
    int                     ret = -1;
    struct  stat        buf;
    ret = fstat(fd,&buf);
    if(ret == -1)
        goto error;
    return 0;
error:
    *tmp_errno = errno;
    return -1;
}


int  need_open_file(void *args, const char *path, int *ret, int *tmp_errno)
{

    journal_file_op_list_t  *op_list = (journal_file_op_list_t *)args;
    struct  stat            stat_buf;
    int                     fd;
    file_op_t               *op = NULL;
    file_op_t               *op_tmp = NULL;

    int                     count   = -1;


    if(!op_list || !path){
        working_journal_log("need_open_file", LOG_ERROR,
            "op_list or path is  null");
        *tmp_errno = ENODATA;
        *ret       = -1;
        goto error;
    }
    *ret = stat(path,&stat_buf);
    if(*ret == -1){
        working_journal_log("need_open_file", LOG_ERROR,
            "path->%s stat failer ",path);
        *tmp_errno = errno;
        *ret       = -1;
        goto error;
    }
    if(!S_ISREG(stat_buf.st_mode)){
        working_journal_log("need_open_file", LOG_ERROR,
            "path->%s  is  not file",path);
        *tmp_errno = errno;
        *ret       = -1;
        goto error;
    }

    fd = open(path, O_RDWR);
    if(!fd){
        working_journal_log("need_open_file", LOG_ERROR,
            "path->%s  open is failer", path);
        *tmp_errno = errno;
        *ret       = -1;
        goto error;
    }

    list_for_each_entry_safe(op, op_tmp, &op_list->op_list_head, oplist){
        switch(op->op){

            case WRITEV_FILE_OP:  
                *ret = open_pwrite(fd, op->args.pwritev.offset, 512, tmp_errno);
                break;
            case READV_FILE_OP:
                *ret = open_preadv(fd, op->args.preadv.offset, 1, tmp_errno);
                break;
            case FALLOCATE_FILE_OP:
                *ret = open_fallocate(fd, op->args.fallocate.offset, op->args.fallocate.len, op->args.fallocate.mode, tmp_errno );
                break;
            case FTRUNCATE_FILE_OP:
                *ret = open_ftruncate(fd, op->args.ftruncate.length, tmp_errno);
                break;
            case FSYNC_FILE_OP:
                *ret = open_sync(fd, tmp_errno);
                break;
            case FGETXATTR_FILE_OP:
                *ret = open_fgetxattr(fd, NULL, tmp_errno);
                break;
            case FSETXATTR_FILE_OP:
                *ret = open_fsetxattr(fd, NULL, NULL, 0, tmp_errno);
                break;
            case FREMOVEXATTR_FILE_OP:
                *ret = open_fremovexattr(fd, NULL, tmp_errno);
                break;
            case FCHOWN_FILE_OP:
                *ret = open_fchown(fd, op->args.chown.owner,op->args.chown.group , tmp_errno);
                break;
            case FCHMOD_FILE_OP:
                *ret = open_fchmod(fd,  op->args.chmod.mode, tmp_errno);
                break;
            case FUTIMES_FILE_OP:
                *ret = open_futimes(fd, op->args.futimes.time, tmp_errno);
                break;
            case FSTAT_FILE_OP:
                *ret = open_fstat(fd, tmp_errno);
                break;
            case LK_FILE_OP:
                if((*ret = flock(fd,op->args.lk.operation)) != 0){
                    *tmp_errno = errno;
                    break;
                }
                else
                     *ret = flock(fd, LOCK_UN);
                break;  
            default: break ;
        }


        if(*ret < 0){
            working_journal_log("Perform the posix need_open_file", LOG_ERROR,
                "path->%s ret->%d errno->%d  op->%d",path,*ret,errno,op->op);
            break;
        }
        count++;
        *tmp_errno = 0;
    }
    close(fd);
    op_list->perform_op_index = count;
    return *ret;

error:
    return  -1;
}

int operation_rename(const char *name,const char *newname, int *tmp_errno)
{

    int                 ret;
    struct  stat        buf;

    if(!newname || newname[0]!='/')
        return -1;

    ret = stat(newname,&buf);
    if(ret == 0)
        return 0 ;

    if(ret == -1 && (*tmp_errno =errno) == ENOENT){

        ret = rename(name,newname);
        if(ret == -1){
            *tmp_errno = errno;
            return ret;
        }
        *tmp_errno = 0;
    }
    return ret;
}

int operation_symlink(const char *srcname, const char *newname, int  *tmp_errno)
{

    int         ret = -1;
    struct  stat        buf;

    if(!newname || newname[0]!='/')
        return -1;

    ret = lstat(newname,&buf);

    if(ret == 0)
        return 0;

    if(ret == -1 && (*tmp_errno = errno) == ENOENT){

        ret = symlink(srcname,newname);
        if(ret == -1)
        {
            *tmp_errno = errno;
            return ret;
        }
        *tmp_errno = 0;
    }
    return 0;
}


int operation_link(const char *srcname, const char *newname, int *tmp_errno)
{

    int                     ret = -1;
    struct  stat            buf;

    if(!newname || newname[0]!='/')
        return -1;

    ret = stat(newname,&buf);

    if(ret == 0)
        return 0;

    if(ret == -1 && (*tmp_errno =errno) == ENOENT){

        ret = link(srcname,newname);
        if (ret == -1 ) {
            printf("Error, num is %d, errstr is %s\n",errno,strerror(errno));
        }
        

        if(ret == -1)
        {
            *tmp_errno = errno;
            return ret ;
        }

        *tmp_errno = 0;
    }
    return 0;
}

int operation_truncate(const char *path, off_t file_size, int *tmp_errno)
{

    int         ret = -1;

    ret = truncate(path,file_size);
    if(ret == -1)
        goto error;
    return 0;

error:
    *tmp_errno = errno;
    return ret;
}

int operation_getxattr(const char *path, const char *key ,int *tmp_errno)
{

    size_t          size_len = -1;
    char            buf[1024] = "\0";

    *tmp_errno = 0;
    size_len = getxattr(path, xattr_key, buf, 1024);
    *tmp_errno = errno;
    if(size_len == -1 && *tmp_errno ==  ENOATTR){
        return 0;
    }else if(size_len >0){
        return 1;
    }else{
        ;
    }

    return -1;

}

int operation_setxattr(const char *path, const char *key, const char *value, int flag, int *tmp_errno)
{

    int                     ret = -1;

    ret = operation_getxattr(path,NULL,tmp_errno);
    if(ret == 1 ){
        ret = setxattr(path, xattr_key, xattr_value, 1024,XATTR_REPLACE);
        goto finish;
    }
    if(ret == 0 ){
        ret = setxattr(path, xattr_key, xattr_value, 1024, XATTR_CREATE);
        goto finish;
    }

finish:
    return ret;
}

int operation_removexattr(const char *path, const char *key, int *tmp_errno)
{

    int                     ret = -1;

    ret = operation_getxattr(path,key,tmp_errno);
    if(ret == 0 )
        return 0;
    if(ret == -1)
        goto error;
    ret = removexattr(path,xattr_key);
    if(ret == -1)
        goto error;
    return 0;
error:
    *tmp_errno = errno;
    return -1;
        
}

int operation_access(const char *path, int mode, int *tmp_errno)
{

    int         ret = -1;

    ret = access(path,mode);
    if(!ret)
        *tmp_errno = errno;

    return ret;
}

int operation_chown(const char *path, uid_t owner,gid_t group, int *tmp_errno)
{

    int                     ret = -1;
    ret  = chown(path, owner, group);
    if(ret == -1)
        goto error;
    return 0;

error:
    *tmp_errno = errno;
    return -1;
}

int operation_chmod(const char *path, mode_t  mode , int *tmp_errno)
{

    int                     ret = -1;
    ret = chmod(path,mode);
    if(ret == -1)
        goto error;
    return 0;
error:
    *tmp_errno = errno;
    return -1;
}

int operation_utimes(const char *path, struct timeval  *time, int *tmp_errno)
{
    int             ret = -1;

    if(!time)
        return -1;
    ret = utimes(path, time);
    if(ret == -1)
        goto error;
    return 0;
error:
    *tmp_errno = errno;
    return -1;
}

int operation_stat(const char *path, int *tmp_errno)
{

    int                     ret = -1;
    struct  stat            buf;
    ret = stat(path,&buf);
    if(ret != 0)
        goto error;
    return 0;
error:
    *tmp_errno = errno;
    return -1;
}

int operation_statfs(const char *path, int *tmp_errno)
{

    int                     ret = -1;
    struct statvfs          buf;

    ret = statvfs(path,&buf);
    if(ret != 0)
        goto error;
    return 0;
error:
    *tmp_errno = errno;
    return -1;
}

int operation_unlink(const char *path, int *tmp_errno)
{

    int                     ret = -1;
    ret = unlink(path);

    if(ret != 0)
        goto error;
    return 0;
error:
    *tmp_errno = errno;
    return -1;
}

int operation_file(void *args, const char *path, int *ret, int *tmp_errno)
{


    journal_file_op_list_t  *op_list    = (journal_file_op_list_t *)args;
    struct  stat            stat_buf;
    file_op_t               *op         = NULL;
    file_op_t               *op_tmp     = NULL;
    file_op_t               *unlink_op  = NULL;

    char                    newname[PATH_MAX]       = "\0";
    char                    op_newname[PATH_MAX]    = "\0";
    int                     unlink                  = 0;
    int                     unlink_count            = 0;

    int                     count                   = -1;
    int                     move_ret                = -1;

    if(!op_list || !path){
        working_journal_log("operation_file", LOG_ERROR, 
            "op_list or path is  null");
        *tmp_errno = ENODATA;
        *ret = -1;
        goto error;
    }

    strcpy(newname,path);

    *ret = stat(path,&stat_buf);
    if(*ret != 0){
        working_journal_log("operation_file", LOG_ERROR,
            "path->%s ret->%d errno->%d",
            path, ret, errno);
        *tmp_errno = errno;
        *ret       = -1;
        goto error;
    }

    if(!S_ISREG(stat_buf.st_mode)){
        working_journal_log("operation_file", LOG_ERROR,
            "path->%s is  not a file", path);
        *tmp_errno = errno;
        *ret       = -1;    
        goto error;
    }

    list_for_each_entry_safe(op, op_tmp, &op_list->op_list_head, oplist){
        switch(op->op){

            case RENAME_FILE_OP:
                if(op_list->op_list_type == disk_type){
                    memset(op_newname,0,PATH_MAX);
                    strcpy(op_newname, disk_mount);
                    strcat(op_newname, op->args.rename.newpath);
                }else{
                    memset(op_newname,0,PATH_MAX);
                    strcpy(op_newname, digioceand_mount);
                    strcat(op_newname, op->args.rename.newpath);
                }
                *ret = operation_rename(newname, op_newname, tmp_errno);
                move_ret = mobile_list(newname, 3, 0);
                if(*ret == 0)
                    strcpy(newname, op_newname);    
                break;

            case SYMLINK_FILE_OP:
                if(op_list->op_list_type == disk_type){
                    memset(op_newname,0,PATH_MAX);
                    strcpy(op_newname, disk_mount);
                    strcat(op_newname, op->args.symlink.newpath);
                }else{
                    memset(op_newname,0,PATH_MAX);
                    strcpy(op_newname, digioceand_mount);
                    strcat(op_newname, op->args.symlink.newpath);
                }

                *ret = operation_symlink(newname, op_newname, tmp_errno);
                break;
            case LINK_FILE_OP:
                if(op_list->op_list_type == disk_type){
                    memset(op_newname,0,PATH_MAX);
                    strcpy(op_newname, disk_mount);
                    strcat(op_newname, op->args.link.newpath);
                }else{
                    memset(op_newname,0,PATH_MAX);
                    strcpy(op_newname,digioceand_mount);
                    strcat(op_newname, op->args.link.newpath);
                }
                *ret = operation_link(newname, op_newname, tmp_errno);
                break;
            case TRUNCATE_FILE_OP:
                *ret = operation_truncate(newname, op->args.truncate.length, tmp_errno);
                break;
            case GETXATTR_FILE_OP:
                *ret = operation_getxattr(newname, op->args.getxattr.name, tmp_errno);
                break;
            case SETXATTR_FILE_OP:
                *ret = operation_setxattr(newname, op->args.setxattr.name, op->args.setxattr.value, op->args.setxattr.flags, tmp_errno);
                                break;
            case REMOVEXATTR_FILE_OP:
                *ret = operation_removexattr(newname, op->args.removexattr.name, tmp_errno);
                break;
            case ACCESS_FILE_OP:
                *ret = operation_access(newname,op->args.access.mode, tmp_errno); 
                break;
            case CHOWN_FILE_OP:
                *ret = operation_chown(newname, op->args.chown.owner,  op->args.chown.group, tmp_errno);
                break;
            case CHMOD_FILE_OP:
                *ret = operation_chmod(newname, op->args.chmod.mode, tmp_errno);
                break;
            case UTIMES_FILE_OP:
                *ret = operation_utimes(newname, op->args.utimes.time, tmp_errno);
                break;
            case STAT_FILE_OP:
                *ret = operation_stat(newname, tmp_errno);
                break;
            case UNLINK_FILE_OP:
                if(unlink == 0){
                    unlink_op = op;
                    unlink = 1;
                }
                unlink_count++;
                break;
            case STATFS_FILE_OP:
                *ret = operation_statfs(newname, tmp_errno);
                break;
            default:
                working_journal_log("operation_file", LOG_ERROR,
                "path->%s  op->%d Is not within the scope of");
                break;

        }

        if(*ret < 0){
            working_journal_log("Perform the posix error operation_file", LOG_ERROR,
                "path->%s errrno->%d  op->%d",newname,*tmp_errno ,op->op);
            break;
        }
        *tmp_errno = 0;
        count++;
    }
    if(unlink == 1){
        if(unlink_count > 1)
        *ret = operation_unlink(newname, tmp_errno);

        move_ret =  mobile_list(newname, 3, 0);
        working_journal_log("SHARK_OPS",LOG_DEBUG, "unlink mobile to %d  "
                    "name is %s",move_ret, newname);
        if(*ret == -1){
            op_list->perform_op_index = count;
            *tmp_errno = errno;
            goto error;
        }

    }

    op_list->perform_op_index = count;

    return *ret;
error:
    
    return  -1;
}


int dirc_readdir(const char *path, int *tmp_errno)
{

    DIR                 *dir;
    struct dirent       *ptr;
    int                 num = 0;
    

    dir = opendir(path);
    if(dir == NULL)
        goto error;

    while((ptr = readdir(dir)) != NULL){
        num++;
    }

    closedir(dir);
    if(num >= 2)
        return 0;
error:
    *tmp_errno=errno;
    return  -1;
}


int need_open_direc(void  *args, const char *path, int *ret, int *tmp_errno)
{

    journal_file_op_list_t  *op_list = (journal_file_op_list_t *)args;
    struct  stat            stat_buf;
    int                     fd;
    file_op_t               *op      = NULL;
    file_op_t               *op_tmp  = NULL;
    int                     count    = -1;


    if (!op_list || !path) {
        working_journal_log("open_direc", LOG_ERROR, 
            "op_list or path is  null");
        *tmp_errno = ENODATA;
        *ret       = -1;
        goto error;
    }

    *ret = stat(path,&stat_buf);
    if(*ret == -1){
        working_journal_log("open_direc", LOG_ERROR,
              "path->%s stat failer ret->%d,errno->%d,%s",
               path,ret,errno,strerror(errno));

        *tmp_errno = errno;
        *ret       = -1;
        goto error;
    }

    if(!S_ISDIR(stat_buf.st_mode)){
        working_journal_log("open_direc", LOG_ERROR,
            "path->%s is  Not a directory",path);
        *tmp_errno = errno;
        *ret       = -1;
        goto error;
    }

    fd = open(path, O_RDONLY);
    if(!fd){
        working_journal_log("open_direc", LOG_ERROR,
            "path->%s open(dir) failer ",path);
        *tmp_errno = errno;
        *ret       = -1;
        goto error;
    }

    list_for_each_entry_safe(op, op_tmp, &op_list->op_list_head, oplist){
        switch(op->op){

            case FSYNC_DIR_OP: 
                *ret = fsync(fd);
                *tmp_errno = errno;
                break;
            case FGETXATTR_DIR_OP:
                *ret = open_fgetxattr(fd, op->args.fgetxattr.name, tmp_errno);
                break;
            case FSETXATTR_DIR_OP:
                *ret = open_fsetxattr(fd, op->args.fsetxattr.name, op->args.fsetxattr.value, op->args.fsetxattr.flags, tmp_errno);
                break;
            case FREMOVEXATTR_DIR_OP:
                *ret = open_fremovexattr(fd, op->args.fremovexattr.name, tmp_errno);
                break;
            case READDIR_DIR_OP:
                *ret = dirc_readdir(path, tmp_errno);
                break;
            case FCHOWN_DIR_OP:
                *ret = open_fchown(fd, op->args.chown.owner,op->args.chown.group , tmp_errno);
                break;
            case FCHMOD_DIR_OP:
                *ret = open_fchmod(fd,  op->args.chmod.mode, tmp_errno);
                break;
            case FUTIMES_DIR_OP:
                *ret = open_futimes(fd, op->args.futimes.time, tmp_errno);
                break;
            case FSTAT_DIR_OP:
                *ret = open_fstat(fd, tmp_errno);
                break;
            default:
                working_journal_log("open_direc", LOG_ERROR, 
                    "path->%s  op->op is  %d",path,op->op);  
                break;
        }
        
                if(*ret < 0){
            working_journal_log("Perform the posix error open_direc", LOG_ERROR,
                "path->%s op->%d is  failer errno->%d op_index->%d ",
                path,op->op, errno,count);
            break;
        }
        count++;
    }
    close(fd);
    op_list->perform_op_index = count;
    return *ret;

error:
    return  -1;
}


int operation_direc(void *args, const char *path, int *ret, int *tmp_errno)
{

    journal_file_op_list_t  *op_list = (journal_file_op_list_t *)args;
    struct  stat            stat_buf;
    file_op_t               *op      = NULL;
    file_op_t               *op_tmp  = NULL;

    char                    op_newname[2048] = "\0";

    int                     count = -1;


    if(!op_list || !path){
        working_journal_log("operation_direc", LOG_ERROR,
            "op_list or path is  null");

        *tmp_errno = ENODATA;
        *ret       = -1;
        goto error;
    }

    *ret = stat(path,&stat_buf);
    if(*ret == -1){
        working_journal_log("operation_direc", LOG_ERROR,
            "path->%s  ret->%d errno->%d",path, ret, errno);
        *tmp_errno = errno;
        *ret       = -1;
        goto error;
    }

    if(!S_ISDIR(stat_buf.st_mode)){
        working_journal_log("operation_direc", LOG_ERROR,
            "path->%s is Not a directory",path);
        *tmp_errno = errno;
        *ret       = -1; 
        goto error;
    }


    list_for_each_entry_safe(op, op_tmp, &op_list->op_list_head, oplist){
        switch(op->op){
            case SYMLINK_DIR_OP:
                if(op_list->op_list_type == disk_type){
                    strcpy(op_newname, disk_mount);
                    strcat(op_newname, op->args.symlink.newpath);
                }else{
                    strcpy(op_newname, digioceand_mount);
                    strcat(op_newname, op->args.symlink.newpath);
                }
                *ret = operation_symlink(path, op_newname, tmp_errno);
                break;
            case SETXATTR_DIR_OP:
                *ret = operation_setxattr(path, op->args.setxattr.name, op->args.setxattr.value, op->args.setxattr.flags, tmp_errno);
                break;
            case GETXATTR_DIR_OP:
                *ret = operation_getxattr(path, op->args.getxattr.name, tmp_errno);
                break;
            case REMOVEXATTR_DIR_OP:
                *ret = operation_removexattr(path, op->args.removexattr.name, tmp_errno);
                break;
            case CHOWN_DIR_OP:
                *ret = operation_chown(path, op->args.chown.owner,  op->args.chown.group, tmp_errno);
                break;
            case CHMOD_DIR_OP:
                *ret = operation_chmod(path, op->args.chmod.mode, tmp_errno);
                break;
            case UTIMES_DIR_OP:
                *ret = operation_utimes(path, op->args.utimes.time, tmp_errno);
                break;
            case STAT_DIR_OP:
                *ret = operation_stat(path, tmp_errno);
                break;
            case STATFS_DIR_OP:
                break;
                *ret = operation_statfs(path, tmp_errno);
            case ACCESS_DIR_OP:
                *ret = operation_access(path,op->args.access.mode, tmp_errno);
                break;
            default: break;


        }
        if(*ret < 0){
            working_journal_log("Perform the posix error operation_direc", LOG_ERROR,
            "Execute the system call error path->%s  ret->%d errno->%d ,%s",
            path,ret,errno,strerror(errno));
            break;
        }
        count++;
    }
    op_list->perform_op_index = count;
    return *ret;
error:
        return  -1;
}
int operation_readlink(const char *path,int *tmp_errno)
{

    int     ret = -1;
    char        path_buf[2048];

    ret  = readlink(path, path_buf, 4086);
    if(ret == -1)
        goto error;
    return 0;
    
error:
    *tmp_errno = errno;
    return -1;
}

int symlink_operation(void *args, const char *path, int *ret, int *tmp_errno)
{

    journal_file_op_list_t  *op_list = (journal_file_op_list_t *)args;
    struct  stat        stat_buf;
    file_op_t           *op      = NULL;
    file_op_t           *op_tmp  = NULL;

    file_op_t           *unlink_op = NULL;
    int                 unlink = 0;
    int                 unlink_count =0;

    int                 count = -1;
    int                 move_ret = -1;

    if(!op_list || !path){
        working_journal_log("symlink operation", LOG_ERROR,
            "op_list or path is  null");
        *tmp_errno = ENODATA;
        *ret       = -1;
    }

    *ret = lstat(path,&stat_buf);
    if(*ret == -1){
        working_journal_log("symlink operation", LOG_ERROR,
            "path->%s  lstat",path);
        *tmp_errno = errno;
        *ret       = -1;
    }

    if(!S_ISLNK(stat_buf.st_mode)){
        working_journal_log("symlink operation", LOG_ERROR,
            "path->%s is no the symlink",path);

        *tmp_errno = errno;
        *ret       = -1;
    }

    list_for_each_entry_safe(op, op_tmp, &op_list->op_list_head, oplist){
        switch(op->op){
            case READLINK_SYMLINK_OP:
                *ret = operation_readlink(path, tmp_errno);
                break;
            case UNLINK_SYMLINK_OP:
                if(unlink == 0){
                    unlink_op = op;
                    unlink = 1;
                }
                unlink_count++;
                break;
            default:
                working_journal_log("symlink operation", LOG_ERROR,
                    "path->%s op->%d Is not within the scope of",
                    path, op->op); 
                break;
        }
        if(*ret < 0){
            working_journal_log("Perform the posix error symlink_operation", LOG_ERROR,
                "path->%s op->%d failer", path ,op);
            break;
        }
        count++;
    }
    
    if(unlink == 1){
        if(unlink_count > 1)
            *ret = operation_unlink(path, tmp_errno);
        move_ret =  mobile_list(path, 2, 0);
        working_journal_log("symlink_operation",LOG_DEBUG, "unlink symlink mobile to %d "
                    "name is %s",move_ret, path);
        if(*ret == -1){
            op_list->perform_op_index = count;
            goto error;
        }
    }
    op_list->perform_op_index = count ;
    return *ret;
error:

    return  -1;
}

int ceate_file(void *args, const char *path, int *ret, int *tmp_errno)
{

    journal_file_op_list_t  *op_list = (journal_file_op_list_t *)args;
    int                     fd;
    struct  stat            stat_buf;

    if(!op_list || !path){
        working_journal_log("ceate_file", LOG_ERROR,
            "op_list or path is  null");
        *tmp_errno = errno;
        *ret       = -1;
        goto error;
    }
    
    if(list_empty(&op_list->op_list_head)){

        working_journal_log("ceate_file", LOG_ERROR, 
                "Create a file has  no a number of op");
    }

    *ret = stat(path,&stat_buf);
    if(*ret == 0){

        working_journal_log("ceate_file", LOG_ERROR,
            "path ->%s  is  have  ", path);
        goto file_is_exist;
    }

    fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0777);
    if(!fd){
        working_journal_log("Perform the posix error ceate_file", LOG_ERROR,
        "path-->%s   file  failer",path);
        *ret        = -1;
        *tmp_errno  = errno;
        goto error;
    }

    close(fd);

file_is_exist:

    *ret = need_open_file(op_list, path, ret, tmp_errno);
    if(*ret == -1){
        working_journal_log("ceate_file", LOG_ERROR,
        "path-->%s  perform  failer",path);
    }
    return *ret;


error:
    return -1;
}

int mkdir_direc(void *args, const char *path,int *ret ,int *tmp_errno)
{

    journal_file_op_list_t  *op_list = (journal_file_op_list_t *)args;
    struct  stat            stat_buf;


    if(!op_list || !path){
        working_journal_log("mkdir_direc", LOG_ERROR, 
                "op_list or path is  null");

        *tmp_errno = ENODATA;
        *ret       = -1;
        goto error;
    }

    if(!list_empty(&op_list->op_list_head)){

        working_journal_log("mkdir_direc", LOG_ERROR, 
            "Create a directory has a number of op");
    }

    *ret = stat(path,&stat_buf);
    if(*ret == 0){
        working_journal_log("mkdir_direc", LOG_ERROR,
            "path ->%s  is  have  ", path);
        *tmp_errno = EEXIST;
        *ret       = 0;
        goto error;
    }

    *ret = mkdir(path, 0777);
    if(*ret == -1){
        working_journal_log("Perform the posix error mkdir_direc", LOG_ERROR,
        "path-->%s   mkdir  failer",path);

        *tmp_errno = errno;
        *ret       = -1;
        goto error;
    }

    return 0;
error:

    return *ret;
}

