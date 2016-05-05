#ifndef __SHARK_POSIX_H
#define __SHARK_POSIX_H


#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <attr/xattr.h>
#define _GNU_SOURCE
#include <fcntl.h>

#include <memory.h>
#include <sys/uio.h>
#include <sys/statvfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/file.h>

#include "list.h"


#include "shark_file.h"
#include "shark_ops.h"
#include "analyze.h"



char *posix_copy_str(const char *path, int n);

int open_pwrite (int fd, off_t  offset,int sum,int *tmp_errno);
int open_preadv (int fd, off_t  offset, int sum, int *tmp_errno);
int open_fallocate(int fd,off_t offset,off_t len,int mode, int *tmp_errno);
int open_ftruncate(int fd,off_t file_size, int *tmp_errno);
int open_sync(int fd, int *tmp_errno);
int open_fgetxattr(int fd,const char *key, int *tmp_errno);
int open_fsetxattr(int fd, char *key, char *value, int flag, int *tmp_errno);
int open_fremovexattr(int fd, const char *key, int *tmp_errno);
int open_fchown(int fd, uid_t owner,gid_t group, int *tmp_errno);
int open_fchmod(int fd, mode_t  mode , int *tmp_errno);
int open_futimes(int fd, struct timeval  *time, int *tmp_errno);
int open_fstat(int fd, int *tmp_errno);

int need_open_file(void *args, const char *path, int *ret, int *tmp_errno);


int operation_rename(const char *name,const char *rename, int *tmp_errno);
int operation_symlink(const char *srcname, const char *newname, int  *tmp_errno);
int operation_link(const char *srcname, const char *newname, int *tmp_errno);
int operation_truncate(const char *path, off_t file_size, int *tmp_errno);
int operation_getxattr(const char *path, const char *key ,int *tmp_errno);
int operation_setxattr(const char *path, const char *key, const char *value, int flag, int *tmp_errno);
int operation_removexattr(const char *path, const char *key, int *tmp_errno);
int operation_access(const char *path, int mode, int *tmp_errno);
int operation_chown(const char *path, uid_t owner,gid_t group, int *tmp_errno);
int operation_futimes(const char *path, struct timeval  *time, int *tmp_errno);
int operation_stat(const char *path, int *tmp_errno);
int operation_statfs(const char *path, int *tmp_errno);
int operation_unlink(const char *path, int *tmp_errno);

int operation_file(void *args, const char *path, int *ret, int *tmp_errno);


int dirc_readdir(const char *path, int *tmp_errno);

int need_open_direc(void *args, const char *path, int *ret, int *tmp_errno);


int operation_direc(void *args, const char *path, int *ret, int *tmp_errno);


int operation_readlink(const char *path,int *tmp_errno);

int symlink_operation(void *op_list, const char *path, int *ret, int *tmp_errno);


int ceate_file(void *args, const char *path, int *ret, int *tmp_errno);


int mkdir_direc(void *args, const char *path,int *ret ,int *tmp_errno);
#endif
