#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include "log.h"
#include "analyze.h"
#include "journal.h"
#include "shark_ops.h"

global_journal_t gjournal;

int analyze_string_write (file_ops_group_t *group,
                            file_ops_t *op_list, file_op_t *op)
{
        FILE *fp = NULL;
        int cls = 0;
        int op_num = 0;
        int grp_index = 0;
        int cls_index = 0;
        int op_index = 0;
        char buf[2048] = {0,};

        grp_index = group->opsgroups_index;
        cls_index = op_list->ops_index;
        op_index = op->op_index;

        cls = op_list->op_type;
        op_num = op->op;
        fp = gjournal.execute_journal_fd;

        if (cls == OPENED_DIR_OP) {
                switch (op_num) {
                case O_UNKNOWN_DIR_OP:
                case O_MAX_DIR_OP:
                        working_journal_log("OPENED_DIR_OP", LOG_INFO,
                                        "Couldn't analyze the op. op_info=(%d,%d,%d)"
                                        ,grp_index, cls_index, op_index);
                        break;
                case FSYNC_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fsync op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FSETXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fsetxattr name=%s value=%s flags=%d op_info=(%d,%d,%d)\n",
                                                                              op->args.fsetxattr.name,
                                                                              op->args.fsetxattr.value,
                                                                              op->args.fsetxattr.flags,
                                                                              grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FGETXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fgetxattr name=%s op_info=(%d,%d,%d)\n",op->args.fgetxattr.name,
                                                            grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FREMOVEXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fremovexattr name=%s op_info=(%d,%d,%d)\n",op->args.fremovexattr.name,
                                                                                  grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case READDIR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "readdir op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FCHOWN_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fchown owner=%u group=%u op_info=(%d,%d,%d)\n",op->args.fchown.owner,
                                                                                     op->args.fchown.group,
                                                                                     grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FCHMOD_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fchmod mode=%u op_info=(%d,%d,%d)\n",op->args.fchmod.mode,
                                                                            grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FUTIMES_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "futimes tv_sec=%ld tv_usec=%ld op_info=(%d,%d,%d)\n",op->args.futimes.time->tv_sec,
                                                                                            op->args.futimes.time->tv_usec,
                                                                                            grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FSTAT_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fstat op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                }
        }

        if (cls == NO_OPEN_DIR_OP) {
                switch (op_num) {
                case NO_UNKNOWN_DIR_OP:
                case NO_MAX_DIR_OP:
                        working_journal_log("NO_OPEN_DIR_OP", LOG_INFO,
                                        "Couldn't analyze the op. op_info=(%d,%d,%d)"
                                        ,grp_index, cls_index, op_index);
                        break;
                case ACCESS_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "access mode=%d op_info=(%d,%d,%d)\n",op->args.access.mode,
                                                                            grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case RENAME_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "rename newpath=%s op_info=(%d,%d,%d)\n",op->args.rename.newpath
                                                                              ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case SYMLINK_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "symlink newpath=%s op_info=(%d,%d,%d)\n",op->args.symlink.newpath,
                                                                                grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case SETXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "setxattr name=%s value=%s flags=%d op_info=(%d,%d,%d)\n"
                                                                            ,op->args.setxattr.name
                                                                            ,op->args.setxattr.value
                                                                            ,op->args.setxattr.flags
                                                                            ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case GETXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "getxattr name=%s op_info=(%d,%d,%d)\n",op->args.getxattr.name,
                                                                              grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case REMOVEXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "removexattr name=%s op_info=(%d,%d,%d)\n",op->args.removexattr.name
                                                                                ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case CHOWN_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "chown owner=%u group=%u op_info=(%d,%d,%d)\n",op->args.chown.owner
                                                                                    ,op->args.chown.group
                                                                                    ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case CHMOD_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "chmod mode=%u op_info=(%d,%d,%d)\n",op->args.fchmod.mode
                                                                          ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case UTIMES_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "utimes tv_sec=%ld tv_usec=%ld op_info=(%d,%d,%d)\n",op->args.utimes.time->tv_sec,
                                                                                           op->args.utimes.time->tv_usec,
                                                                                           grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case RMDIR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "rmdir op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case STAT_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "stat op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case STATFS_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "statfs op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                }
        }

        if (cls == OPENED_FILE_OP) {
                switch (op_num) {
                case O_UNKNOWN_FILE_OP:
                case O_MAX_FILE_OP:
                        working_journal_log("OPEN_FILE_OP", LOG_INFO,
                                        "Couldn't analyze the op. op_info=(%d,%d,%d)"
                                        ,grp_index, cls_index, op_index);
                        break;
                case WRITEV_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "pwritev offset=%ld op_info=(%d,%d,%d)\n",op->args.pwritev.offset,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case READV_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "preadv offset=%ld op_info=(%d,%d,%d)\n",op->args.preadv.offset,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FALLOCATE_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fallocate mode=%d offset=%ld len=%ld op_info=(%d,%d,%d)\n",op->args.fallocate.mode
                                                                                                 ,op->args.fallocate.offset
                                                                                                 ,op->args.fallocate.len
                                                                                                 ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FTRUNCATE_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "ftruncate length=%ld op_info=(%d,%d,%d)\n",op->args.ftruncate.length
                                                                                 ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FSYNC_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fsync op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FSETXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fsetxattr name=%s value=%s flags=%d op_info=(%d,%d,%d)\n",
                                                                              op->args.fsetxattr.name,
                                                                              op->args.fsetxattr.value,
                                                                              op->args.fsetxattr.flags,
                                                                              grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FGETXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fgetxattr name=%s op_info=(%d,%d,%d)\n",op->args.fgetxattr.name
                                                                               ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FREMOVEXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fremovexattr name=%s op_info=(%d,%d,%d)\n",op->args.fremovexattr.name
                                                                                 ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case LK_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "lk operation=%d op_info=(%d,%d,%d)\n",op->args.lk.operation
                                                                            ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FCHOWN_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fchown owner=%u group=%u op_info=(%d,%d,%d)\n",op->args.fchown.owner,
                                                                                      op->args.fchown.group
                                                                                      ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FCHMOD_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fchmod mode=%u op_info=(%d,%d,%d)\n",op->args.fchmod.mode
                                                                           ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FUTIMES_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "futimes tv_sec=%ld tv_usec=%ld op_info=(%d,%d,%d)\n",op->args.futimes.time->tv_sec
                                                                                           ,op->args.futimes.time->tv_usec
                                                                                           ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FSTAT_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fstat op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                }
        }

        if (cls == NO_OPEN_FILE_OP) {
                switch (op_num) {
                case NO_UNKNOWN_DIR_OP:
                case NO_MAX_FILE_OP:
                        working_journal_log("NO_OPEN_FILE_OP", LOG_INFO,
                                        "Couldn't analyze the op. op_info=(%d,%d,%d)"
                                        ,grp_index, cls_index, op_index);
                        break;
                case RENAME_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "rename newpath=%s op_info=(%d,%d,%d)\n",op->args.rename.newpath
                                                                              ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case SYMLINK_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "symlink newpath=%s op_info=(%d,%d,%d)\n",op->args.symlink.newpath
                                                                               ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case LINK_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "link newpath=%s op_info=(%d,%d,%d)\n",op->args.link.newpath
                                                                            ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case TRUNCATE_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "truncate length=%ld op_info=(%d,%d,%d)\n",op->args.truncate.length
                                                                                ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case SETXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "setxattr name=%s value=%s flags=%d op_info=(%d,%d,%d)\n"
                                                                                               ,op->args.setxattr.name
                                                                                               ,op->args.setxattr.value
                                                                                               ,op->args.setxattr.flags
                                                                                               ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case GETXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "getxattr name=%s op_info=(%d,%d,%d)\n",op->args.getxattr.name
                                                                                     ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case REMOVEXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "removexattr name=%s op_info=(%d,%d,%d)\n",op->args.removexattr.name
                                                                                ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case ACCESS_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "access mode=%d op_info=(%d,%d,%d)\n",op->args.access.mode
                                                                                       ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case CHOWN_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "chown owner=%u group=%u op_info=(%d,%d,%d)\n",op->args.chown.owner,
                                                                                    op->args.chown.group
                                                                                    ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case CHMOD_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "chmod mode=%u op_info=(%d,%d,%d)\n",op->args.fchmod.mode
                                                                          ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case UTIMES_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "utimes tv_sec=%ld tv_usec=%ld op_info=(%d,%d,%d)\n",op->args.utimes.time->tv_sec
                                                                                          ,op->args.utimes.time->tv_usec
                                                                                          ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case STAT_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "stat op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case UNLINK_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "unlink op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case STATFS_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "statfs op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                }
        }

        if (cls == SYMLINK_OP) {
                switch(op_num) {
                case UNKNOWN_SYMLINK_OP:
                case MAX_SYMLINK_OP:
                        working_journal_log("SYMLINK_OP", LOG_INFO,
                                        "Couldn't analyze the op. op_info=(%d,%d,%d)"
                                        ,grp_index, cls_index, op_index);
                        break;
                case READLINK_SYMLINK_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "readlink op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case UNLINK_SYMLINK_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "unlink op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                }
        }

        return 0;
}

#if 0
int analyze_string_write_2 (journal_file_group_t *group,
                            journal_file_op_list_t *op_list, file_op_t *op,
                            FILE *fp)
{
        int cls = 0;
        int op_num = 0;
        int grp_index = 0;
        int cls_index = 0;
        int op_index = 0;
        char buf[2048] = {0,};

        grp_index = group->opsgroups_index;
        cls_index = op_list->ops_index;
        op_index = op->op_index;

        cls = op_list->op_type;
        op_num = op->op;

        if (cls == OPENED_DIR_OP) {
                switch (op_num) {
                case O_UNKNOWN_DIR_OP:
                case O_MAX_DIR_OP:
                        working_journal_log("OPENED_DIR_OP", LOG_INFO,
                                        "Couldn't analyze the op. op_info=(%d,%d,%d)"
                                        ,grp_index, cls_index, op_index);
                        break;
                case FSYNC_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fsync op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FSETXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fsetxattr name=%s value=%s flags=%d op_info=(%d,%d,%d)\n",
                                                                              op->args.fsetxattr.name,
                                                                              op->args.fsetxattr.value,
                                                                              op->args.fsetxattr.flags,
                                                                              grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FGETXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fgetxattr name=%s op_info=(%d,%d,%d)\n",op->args.fgetxattr.name,
                                                            grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FREMOVEXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fremovexattr name=%s op_info=(%d,%d,%d)\n",op->args.fremovexattr.name,
                                                                                  grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case READDIR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "readdir op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FCHOWN_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fchown owner=%u group=%u op_info=(%d,%d,%d)\n",op->args.fchown.owner,
                                                                                     op->args.fchown.group,
                                                                                     grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FCHMOD_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fchmod mode=%u op_info=(%d,%d,%d)\n",op->args.fchmod.mode,
                                                                            grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FUTIMES_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "futimes tv_sec=%ld tv_usec=%ld op_info=(%d,%d,%d)\n",op->args.futimes.time->tv_sec,
                                                                                            op->args.futimes.time->tv_usec,
                                                                                            grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FSTAT_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fstat op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                }
        }

        if (cls == NO_OPEN_DIR_OP) {
                switch (op_num) {
                case NO_UNKNOWN_DIR_OP:
                case NO_MAX_DIR_OP:
                        working_journal_log("NO_OPEN_DIR_OP", LOG_INFO,
                                        "Couldn't analyze the op. op_info=(%d,%d,%d)"
                                        ,grp_index, cls_index, op_index);
                        break;
                case ACCESS_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "access mode=%d op_info=(%d,%d,%d)\n",op->args.access.mode,
                                                                            grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case RENAME_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "rename newpath=%s op_info=(%d,%d,%d)\n",op->args.rename.newpath
                                                                              ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case SYMLINK_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "symlink newpath=%s op_info=(%d,%d,%d)\n",op->args.symlink.newpath,
                                                                                grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case SETXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "setxattr name=%s value=%s flags=%d op_info=(%d,%d,%d)\n"
                                                                            ,op->args.setxattr.name
                                                                            ,op->args.setxattr.value
                                                                            ,op->args.setxattr.flags
                                                                            ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case GETXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "getxattr name=%s op_info=(%d,%d,%d)\n",op->args.getxattr.name,
                                                                              grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case REMOVEXATTR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "removexattr name=%s op_info=(%d,%d,%d)\n",op->args.removexattr.name
                                                                                ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case CHOWN_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "chown owner=%u group=%u op_info=(%d,%d,%d)\n",op->args.chown.owner
                                                                                    ,op->args.chown.group
                                                                                    ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case CHMOD_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "chmod mode=%u op_info=(%d,%d,%d)\n",op->args.fchmod.mode
                                                                          ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case UTIMES_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "utimes tv_sec=%ld tv_usec=%ld op_info=(%d,%d,%d)\n",op->args.utimes.time->tv_sec,
                                                                                           op->args.utimes.time->tv_usec,
                                                                                           grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case RMDIR_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "rmdir op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case STAT_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "stat op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case STATFS_DIR_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "statfs op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                }
        }

        if (cls == OPENED_FILE_OP) {
                switch (op_num) {
                case O_UNKNOWN_FILE_OP:
                case O_MAX_FILE_OP:
                        working_journal_log("OPEN_FILE_OP", LOG_INFO,
                                        "Couldn't analyze the op. op_info=(%d,%d,%d)"
                                        ,grp_index, cls_index, op_index);
                        break;
                case WRITEV_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "pwritev offset=%ld op_info=(%d,%d,%d)\n",op->args.pwritev.offset,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case READV_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "preadv offset=%ld op_info=(%d,%d,%d)\n",op->args.preadv.offset,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FALLOCATE_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fallocate mode=%d offset=%ld len=%ld op_info=(%d,%d,%d)\n",op->args.fallocate.mode
                                                                                                 ,op->args.fallocate.offset
                                                                                                 ,op->args.fallocate.len
                                                                                                 ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FTRUNCATE_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "ftruncate length=%ld op_info=(%d,%d,%d)\n",op->args.ftruncate.length
                                                                                 ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FSYNC_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fsync op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FSETXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fsetxattr name=%s value=%s flags=%d op_info=(%d,%d,%d)\n",
                                                                              op->args.fsetxattr.name,
                                                                              op->args.fsetxattr.value,
                                                                              op->args.fsetxattr.flags,
                                                                              grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FGETXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fgetxattr name=%s op_info=(%d,%d,%d)\n",op->args.fgetxattr.name
                                                                               ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FREMOVEXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fremovexattr name=%s op_info=(%d,%d,%d)\n",op->args.fremovexattr.name
                                                                                 ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case LK_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "lk operation=%d op_info=(%d,%d,%d)\n",op->args.lk.operation
                                                                            ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FCHOWN_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fchown owner=%u group=%u op_info=(%d,%d,%d)\n",op->args.fchown.owner,
                                                                                      op->args.fchown.group
                                                                                      ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FCHMOD_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fchmod mode=%u op_info=(%d,%d,%d)\n",op->args.fchmod.mode
                                                                           ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FUTIMES_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "futimes tv_sec=%ld tv_usec=%ld op_info=(%d,%d,%d)\n",op->args.futimes.time->tv_sec
                                                                                           ,op->args.futimes.time->tv_usec
                                                                                           ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case FSTAT_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "fstat op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                }
        }

        if (cls == NO_OPEN_FILE_OP) {
                switch (op_num) {
                case NO_UNKNOWN_DIR_OP:
                case NO_MAX_FILE_OP:
                        working_journal_log("NO_OPEN_FILE_OP", LOG_INFO,
                                        "Couldn't analyze the op. op_info=(%d,%d,%d)"
                                        ,grp_index, cls_index, op_index);
                        break;
                case RENAME_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "rename newpath=%s op_info=(%d,%d,%d)\n",op->args.rename.newpath
                                                                              ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case SYMLINK_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "symlink newpath=%s op_info=(%d,%d,%d)\n",op->args.symlink.newpath
                                                                               ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case LINK_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "link newpath=%s op_info=(%d,%d,%d)\n",op->args.link.newpath
                                                                            ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case TRUNCATE_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "truncate length=%ld op_info=(%d,%d,%d)\n",op->args.truncate.length
                                                                                ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case SETXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "setxattr name=%s value=%s flags=%d op_info=(%d,%d,%d)\n"
                                                                                               ,op->args.setxattr.name
                                                                                               ,op->args.setxattr.value
                                                                                               ,op->args.setxattr.flags
                                                                                               ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case GETXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "getxattr name=%s op_info=(%d,%d,%d)\n",op->args.getxattr.name
                                                                                     ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case REMOVEXATTR_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "removexattr name=%s op_info=(%d,%d,%d)\n",op->args.removexattr.name
                                                                                ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case ACCESS_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "access mode=%d op_info=(%d,%d,%d)\n",op->args.access.mode
                                                                                       ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case CHOWN_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "chown owner=%u group=%u op_info=(%d,%d,%d)\n",op->args.chown.owner,
                                                                                    op->args.chown.group
                                                                                    ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case CHMOD_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "chmod mode=%u op_info=(%d,%d,%d)\n",op->args.fchmod.mode
                                                                          ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case UTIMES_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "utimes tv_sec=%ld tv_usec=%ld op_info=(%d,%d,%d)\n",op->args.utimes.time->tv_sec
                                                                                          ,op->args.utimes.time->tv_usec
                                                                                          ,grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case STAT_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "stat op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case UNLINK_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "unlink op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case STATFS_FILE_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "statfs op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                }
        }

        if (cls == SYMLINK_OP) {
                switch(op_num) {
                case UNKNOWN_SYMLINK_OP:
                case MAX_SYMLINK_OP:
                        working_journal_log("SYMLINK_OP", LOG_INFO,
                                        "Couldn't analyze the op. op_info=(%d,%d,%d)"
                                        ,grp_index, cls_index, op_index);
                        break;
                case READLINK_SYMLINK_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "readlink op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                case UNLINK_SYMLINK_OP:
                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, "unlink op_info=(%d,%d,%d)\n",grp_index, cls_index, op_index);
                        fputs (buf, fp);
                        break;
                }
        }

        return 0;
}
#endif

int cut_string_operation (file_op_t *op, char *line, int op_type)
{
        int i = 0;
        char *fn_name = NULL;
        char *tmp = NULL;
        char *p = NULL;
        char *tmp_ptr = NULL;
        char *delim = " ";
        char string[PATH_MAX] = {0,};

        strcpy (string, line);
        fn_name = strtok_r (string, delim, &tmp_ptr);

        switch (op_type) {
        case OPENED_DIR_OP:
                if (!strcmp ("fsync", fn_name))
                        op->op = FSYNC_DIR_OP;
                else if (!strcmp ("fsetxattr", fn_name)) {
                        op->op = FSETXATTR_DIR_OP;

                        for (i = 0;i < 3;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      strcpy (op->args.fsetxattr.name, p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      strcpy (op->args.fsetxattr.value, p);
                              }
                              else if (i == 2) {
                                      p = p + 1;
                                      op->args.fsetxattr.flags = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("fgetxattr", fn_name)) {
                        op->op = FGETXATTR_DIR_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.fgetxattr.name, p);
                }
                else if (!strcmp ("fremovexattr", fn_name)) {
                        op->op = FREMOVEXATTR_DIR_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.removexattr.name, p);
                }
                else if (!strcmp ("readdir", fn_name)) {
                        op->op = READDIR_DIR_OP;
                }
                else if (!strcmp ("fchown", fn_name)) {
                        op->op = FCHOWN_DIR_OP;

                        for (i = 0;i < 2;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      op->args.fchown.owner = atoi (p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      op->args.fchown.group = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("fchmod", fn_name)) {
                        op->op = FCHMOD_DIR_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');
                        p = p + 1;
                        op->args.fchmod.mode = atoi (p);
                }
                else if (!strcmp ("futimes", fn_name)) {
                        op->op = FUTIMES_DIR_OP;

                        for (i = 0;i < 2;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      op->args.futimes.time->tv_sec = atoi (p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      op->args.futimes.time->tv_usec = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("fstat", fn_name)) {
                        op->op = FSTAT_DIR_OP;
                }
                break;
        case NO_OPEN_DIR_OP:
                if (!strcmp ("access", fn_name)) {
                        op->op = ACCESS_DIR_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        op->args.access.mode = atoi (p);
                }
                else if (!strcmp ("rename", fn_name)) {
                        op->op = RENAME_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.rename.newpath, p);
                }
                else if (!strcmp ("symlink", fn_name)) {
                        op->op = SYMLINK_DIR_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.symlink.newpath, p);
                }
                else if (!strcmp ("setxattr", fn_name)) {
                        op->op = SETXATTR_DIR_OP;

                        for (i = 0;i < 3;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      strcpy (op->args.setxattr.name, p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      strcpy (op->args.setxattr.value, p);
                              }
                              else if (i == 2) {
                                      p = p + 1;
                                      op->args.setxattr.flags = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("getxattr", fn_name)) {
                        op->op = GETXATTR_DIR_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.fgetxattr.name, p);
                }
                else if (!strcmp ("removexattr", fn_name)) {
                        op->op = REMOVEXATTR_DIR_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);  
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.removexattr.name, p);
                }
                else if (!strcmp ("chown", fn_name)) {
                        op->op = CHOWN_DIR_OP;

                        for (i = 0;i < 2;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      op->args.fchown.owner = atoi (p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      op->args.fchown.group = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("chmod", fn_name)) {
                        op->op = CHMOD_DIR_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');
                        p = p + 1;
                        op->args.fchmod.mode = atoi (p);
                }
                else if (!strcmp ("utimes", fn_name)) {
                        op->op = UTIMES_DIR_OP;

                        for (i = 0;i < 2;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      op->args.futimes.time->tv_sec = atoi (p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      op->args.futimes.time->tv_usec = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("rmdir", fn_name)) {
                        op->op = RMDIR_DIR_OP;
                }
                else if (!strcmp ("stat", fn_name)) {
                        op->op = STAT_DIR_OP;
                }
                else if (!strcmp ("statfs", fn_name)) {
                        op->op = STATFS_DIR_OP;
                }
                break;
        case OPENED_FILE_OP:
                if (!strcmp ("pwritev", fn_name)) {
                        op->op = WRITEV_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        op->args.pwritev.offset = atoi (p);
                }
                else if (!strcmp ("preadv", fn_name)) {
                        op->op = READV_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        op->args.preadv.offset = atoi (p);
                }
                else if (!strcmp ("fallocate", fn_name)) {
                        op->op = FALLOCATE_FILE_OP;

                        for (i = 0;i < 3;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      op->args.fallocate.mode = atoi (p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      op->args.fallocate.offset = atoi (p);
                              }
                              else if (i == 2) {
                                    p = p + 1;
                                    op->args.fallocate.len = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("ftruncate", fn_name)) {
                        op->op = FTRUNCATE_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');
                        p = p + 1;
                        op->args.ftruncate.length = atoi (p);
                }
                else if (!strcmp ("fsync", fn_name)) {
                        op->op = FSYNC_FILE_OP;
                }
                else if (!strcmp ("fsetxattr", fn_name)) {
                        op->op = FSETXATTR_FILE_OP;

                        for (i = 0;i < 3;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      strcpy (op->args.fsetxattr.name, p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      strcpy (op->args.fsetxattr.value, p);
                              }
                              else if (i == 2) {
                                      p = p + 1;
                                      op->args.fsetxattr.flags = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("fgetxattr", fn_name)) {
                        op->op = FGETXATTR_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.fgetxattr.name, p);
                }
                else if (!strcmp ("fremovexattr", fn_name)) {
                        op->op = FREMOVEXATTR_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.removexattr.name, p);
                }
                else if (!strcmp ("lk", fn_name)) {
                        op->op = LK_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        op->args.lk.operation = atoi (p);
                }
                else if (!strcmp ("fchown", fn_name)) {
                        op->op = FCHOWN_FILE_OP;

                        for (i = 0;i < 2;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      op->args.fchown.owner = atoi (p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      op->args.fchown.group = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("fchmod", fn_name)) {
                        op->op = FCHMOD_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');
                        p = p + 1;
                        op->args.fchmod.mode = atoi (p);
                }
                else if (!strcmp ("futimes", fn_name)) {
                        op->op = FUTIMES_FILE_OP;

                        for (i = 0;i < 2;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      op->args.futimes.time->tv_sec = atoi (p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      op->args.futimes.time->tv_usec = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("fstat", fn_name)) {
                        op->op = FSTAT_FILE_OP;
                }
                break;
        case NO_OPEN_FILE_OP:
                if (!strcmp ("rename", fn_name)) {
                        op->op = RENAME_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.rename.newpath, p);
                }
                else if (!strcmp ("symlink", fn_name)) {
                        op->op = SYMLINK_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.symlink.newpath, p);
                }
                else if (!strcmp ("link", fn_name)) {
                        op->op = LINK_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.link.newpath, p);
                }
                else if (!strcmp ("truncate", fn_name)) {
                        op->op = TRUNCATE_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);  
                        p = strchr (tmp, '=');

                        p = p + 1;
                        op->args.truncate.length = atoi (p);
                }
                else if (!strcmp ("setxattr", fn_name)) {
                        op->op = SETXATTR_FILE_OP;

                        for (i = 0;i < 3;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      strcpy (op->args.setxattr.name, p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      strcpy (op->args.setxattr.value, p);
                              }
                              else if (i == 2) {
                                      p = p + 1;
                                      op->args.setxattr.flags = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("getxattr", fn_name)) {
                        op->op = GETXATTR_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.getxattr.name, p);
                }
                else if (!strcmp ("removexattr", fn_name)) {
                        op->op = REMOVEXATTR_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);  
                        p = strchr (tmp, '=');

                        p = p + 1;
                        strcpy (op->args.removexattr.name, p);
                }
                else if (!strcmp ("access", fn_name)) {
                        op->op = ACCESS_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');

                        p = p + 1;
                        op->args.access.mode = atoi (p);
                }
                else if (!strcmp ("chown", fn_name)) {
                        op->op = CHOWN_FILE_OP;

                        for (i = 0;i < 2;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      op->args.chown.owner = atoi (p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      op->args.chown.group = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("chmod", fn_name)) {
                        op->op = CHMOD_FILE_OP;

                        tmp = strtok_r (NULL, delim, &tmp_ptr);
                        p = strchr (tmp, '=');
                        p = p + 1;
                        op->args.chmod.mode = atoi (p);
                }
                else if (!strcmp ("utimes", fn_name)) {
                        op->op = UTIMES_FILE_OP;

                        for (i = 0;i < 2;i++) {
                              tmp = strtok_r (NULL, delim, &tmp_ptr);
                              p = strchr (tmp, '=');
                              if (i == 0) {
                                      p = p + 1;
                                      op->args.utimes.time->tv_sec = atoi (p);
                              }
                              else if (i == 1) {
                                      p = p + 1;
                                      op->args.utimes.time->tv_usec = atoi (p);
                              }
                        }
                }
                else if (!strcmp ("stat", fn_name)) {
                        op->op = STAT_FILE_OP;
                }
                else if (!strcmp ("unlink", fn_name)) {
                        op->op = UNLINK_FILE_OP;
                }
                else if (!strcmp ("statfs", fn_name)) {
                        op->op = STATFS_FILE_OP;
                }
                break;
        case SYMLINK_OP:
                if (!strcmp ("readlink", fn_name)) {
                        op->op = READLINK_SYMLINK_OP;
                }
                else if (!strcmp ("unlink", fn_name)) {
                        op->op = UNLINK_SYMLINK_OP;
                }
                break;
        }
        return 0;
}

journal_file_group_t *analyze_string_read (worker_type type)
{
        int ret = 0;
        int i = 0;
        size_t len = 0;
        int op_num = 0;
        int op_list_num = 0;
        char *p = NULL;
        char *delim = " ";
        char *tmp_ptr = NULL;
        char *line = NULL;
        char *path = NULL;
        FILE *fp = NULL;
        char string[PATH_MAX] = {0,};
        file_op_t *op = NULL;
        journal_file_op_list_t *op_list = NULL;
        journal_file_group_t *group = NULL;
#if 0
        FILE *fp2 = NULL;
        file_op_t *op2 = NULL;
        journal_file_op_list_t *op_list2 = NULL;
#endif
        memset (string, 0, sizeof (string));
        sprintf (string, "%sexecute_journal.log",cgv.working_directory);

        if ((fp = fopen (string, "r")) == NULL)
                HANDLE_ERROR ("fopen");

        group = (journal_file_group_t *) malloc (sizeof(journal_file_group_t));
        memset (group, 0, sizeof(journal_file_group_t));
        INIT_LIST_HEAD(&group->group_head);

        if (type == disk_type) {
                fseek (fp, gjournal.disk_cur_offset, SEEK_SET);
                group->opsgroups_index = gjournal.ops_group_disk_num;
                gjournal.ops_group_disk_num ++;
        } else if (type == digioceand_type) {
                fseek (fp, gjournal.cluster_cur_offset, SEEK_SET);
                group->opsgroups_index = gjournal.ops_group_cluster_num;
                gjournal.ops_group_cluster_num ++;
        }

        LOCK(&gjournal.read_lock);

        ret = getline (&line, &len, fp);
        if (ret == -1) {
                working_journal_log("READ", LOG_INFO, "read the EOF!");
                goto out;
        } else {
                if (type == disk_type) {
                        gjournal.disk_cur_offset += ret;
                } else if (type == digioceand_type) {
                        gjournal.cluster_cur_offset += ret;
                }
        }

up:
        i = 0;
        path = strtok_r (line, delim, &tmp_ptr);
        p = strchr (path, '=');
        p = p + 1;

        op_list = (journal_file_op_list_t *) malloc (sizeof(journal_file_op_list_t));
        memset (op_list, 0, sizeof(journal_file_op_list_t));
        INIT_LIST_HEAD(&op_list->op_list_head);
        INIT_LIST_HEAD(&op_list->op_group);
        op_list->ops_index = op_list_num;

        strcpy (op_list->path, p);
        while ((p = strtok_r (NULL, delim, &tmp_ptr))) {
                p = strchr (p, '=');
                p = p + 1;
                if (i == 0)
                        op_list->is_need_open = atoi (p);
                else if (i == 1)
                        op_list->file_type = atoi (p);
                else if (i == 2)
                        op_list->is_new = atoi (p);
                else if (i == 3)
                        op_list->op_type = atoi (p);
                i++;
        }
        FREE (line);

        if (type == disk_type) {
                op_list->op_list_type = disk_type;
        } else if (type == digioceand_type) {
                op_list->op_list_type = digioceand_type;
        }

        while (1) {
                ret = getline (&line, &len, fp);
                if (ret == -1) {
                        working_journal_log("READ", LOG_INFO, "read the EOF!");
                        goto unlock;
                } else {
                        if (type == disk_type) {
                                gjournal.disk_cur_offset += ret;
                        } else if (type == digioceand_type) {
                                gjournal.cluster_cur_offset += ret;
                        }
                }

                if (!strncmp (line, "**********",10)) {
                       list_add_tail(&op_list->op_group, &group->group_head);
                       op_num = 0;
                       FREE (line);
                       ret = getline (&line, &len, fp);
                       if (ret == -1) {
                               working_journal_log("READ", LOG_INFO, "read the EOF!");
                               goto unlock;
                       } else {
                               if (type == disk_type) {
                                       gjournal.disk_cur_offset += ret;
                               } else if (type == digioceand_type) {
                                       gjournal.cluster_cur_offset += ret;
                               }
                       }
                       if (!strncmp (line, "##########",10)) {
                               FREE (line);
                               break;
                       } else {
                               op_list_num ++;
                               goto up;
                       }
                } else {
                        op = (file_op_t *) malloc (sizeof(file_op_t));
                        memset (op, 0, sizeof(file_op_t));
                        INIT_LIST_HEAD(&op->oplist);

                        cut_string_operation (op, line, op_list->op_type);
                        op->op_index = op_num;
                        op_num ++;
                        list_add_tail(&op->oplist, &op_list->op_list_head);

                        FREE (line);
                }
        }
unlock:
        UNLOCK(&gjournal.read_lock);

        fclose(fp);

#if 0
        if ((fp2 = fopen ("1.txt", "w+")) == NULL)
                HANDLE_ERROR ("fopen");

        list_for_each_entry(op_list2, &group->group_head, op_group) {
                sprintf (string,"op_list2->path = %s "
                                " op_list2->file_type = %d  "
                                " op_list2->is_new = %d   "
                                " op_list2->op_type = %d   "
                                " op_list2->ops_index = %d\n"
                                ,op_list2->path,
                                op_list2->file_type,
                                op_list2->is_new,
                                op_list2->op_type,
                                op_list2->ops_index);
                fputs (string, fp2);
                list_for_each_entry(op2, &op_list2->op_list_head, oplist) {
                        printf ("ops->op = %d, op2->op_index = %d\n"
                                        ,op2->op, op2->op_index);
                        analyze_string_write_2 (group, op_list2, op2, fp2); 
                }
        }
        fclose(fp2);
#endif
        return group;
out:
        UNLOCK(&gjournal.read_lock);

        if (type == disk_type) {
                gjournal.ops_group_disk_num --;
        } else if (type == digioceand_type) {
                gjournal.ops_group_cluster_num --;
        }

        FREE(group);
        FREE(line);
        fclose(fp);

        return NULL;
}

/*
void free_journal_group_mem (journal_file_group_t *group)
{
        file_op_t *op = NULL;
        file_op_t *tmp_op = NULL;
        journal_file_op_list_t *op_list = NULL;
        journal_file_op_list_t *tmp_op_list = NULL;

        list_for_each_entry_safe(op_list, tmp_op_list,
                                 &group->group_head, op_group){
                list_del(&op_list->op_group);
                list_for_each_entry_safe(op, tmp_op,
                                         &op_list->op_list_head, oplist) {
                        list_del(&op->oplist);
                        FREE(op);
                }
                FREE(op_list);
        }

}
*/
void free_journal_group_mem (journal_file_group_t *group)
{
        FREE(group);
}

void free_journal_op_list_mem (journal_file_op_list_t *op_list)
{
        list_del(&op_list->op_group);
        FREE(op_list);
}

void free_journal_op_mem (file_op_t *op)
{
        list_del(&op->oplist);
        FREE(op);
}

void *thread_writev (void *arg)
{
        file_op_t *op = NULL;
        file_ops_t *op_list = NULL;
        file_ops_group_t *group = NULL;
        file_ops_group_t *tmp = NULL;
        char buf[128] = {0,};
        char *path = NULL;
        char string[PATH_MAX] = {0,};
        int need_open = 0;
        int ret = 0;

        while(1) {
                LOCK(&gjournal.write_lock);
                if (list_empty(&gjournal.group_queue))
                        COND_WAIT(&gjournal.write_cond_lock,&gjournal.write_lock);

                list_for_each_entry_safe(group, tmp, &gjournal.group_queue, group_list){
                        list_del(&group->group_list);
                        UNLOCK(&gjournal.write_lock);

                        LOCK(&gjournal.read_lock);
                        list_for_each_entry(op_list, &group->opsgroup, op_group){
                                if ((path = get_path_for_file_node (op_list->file)) == NULL ) {
                                        working_journal_log("PATH", LOG_INFO, "failed to get file path!");
                                        SHARK_ASSERT(0);
                                } else {
                                        memset (string, 0, sizeof (string));
                                        if (op_list->op_type == OPENED_DIR_OP ||
                                                        op_list->op_type == OPENED_FILE_OP)
                                                need_open = 1;

                                        sprintf (string, "path=%s need_open=%d file_type=%d is_new=%d op_type=%d\n",
                                                        path, need_open, op_list->file_type, op_list->is_new,
                                                        op_list->op_type);
                                        FREE(path);
                                        fputs (string, gjournal.execute_journal_fd);

                                        need_open = 0;
                                }

                                list_for_each_entry(op, &op_list->ops, oplist) {
                                       analyze_string_write (group, op_list, op);
                                }
                                memset (buf, 0, sizeof (buf));
                                sprintf (buf, LIST_STRING,op_list->ops_index);
                                fputs (buf, gjournal.execute_journal_fd);
                        }

                        memset (buf, 0, sizeof (buf));
                        sprintf (buf, GROUP_STRING,group->opsgroups_index);

                        ret = file_ops_group_destroy (group);
                        if (ret != 0) {
                                working_journal_log("GROUP", LOG_INFO, "failed to free group mem");
                                SHARK_ASSERT(0);
                        }

                        fputs (buf, gjournal.execute_journal_fd);
                        fflush (gjournal.execute_journal_fd);

                        UNLOCK(&gjournal.read_lock);
                        LOCK (&gjournal.write_lock);
                }
                UNLOCK (&gjournal.write_lock);

                if (gjournal.thread_shutdown == 1){
                        pthread_exit (NULL);
                }
        }
}

int handle_groups (file_ops_group_t *group)
{
        LOCK(&gjournal.write_lock);

        list_add_tail(&group->group_list, &gjournal.group_queue);

        COND_SIGNAL(&gjournal.write_cond_lock);

        UNLOCK(&gjournal.write_lock);

        usleep (100);

        return 0;
}

void gjournal_procedure_destroy ()
{
        gjournal.thread_shutdown = 1;

        COND_SIGNAL(&gjournal.write_cond_lock);

        pthread_join (gjournal.thread_num, NULL);

        if (gjournal.working_journal_fd != NULL) {
                fclose (gjournal.working_journal_fd);
                gjournal.working_journal_fd = NULL;
        }

        if (gjournal.execute_journal_fd != NULL) {
                fclose (gjournal.execute_journal_fd);
                gjournal.execute_journal_fd = NULL;
        }

        if (gjournal.execute_error_journal_fd != NULL) {
                fclose (gjournal.execute_error_journal_fd);
                gjournal.execute_error_journal_fd = NULL;
        }

        if (gjournal.check_journal_fd != NULL) {
                fclose (gjournal.check_journal_fd);
                gjournal.check_journal_fd = NULL;
        }

        LOCK_DESTROY(&gjournal.write_lock);
        LOCK_DESTROY(&gjournal.read_lock);
        LOCK_DESTROY(&gjournal.log_lock);

        COND_LOCK_DESTROY(&gjournal.write_cond_lock);
}

void gjournal_init (global_journal_t *gjournal)
{
        int ret = 0;

        LOCK_INIT(&gjournal->write_lock);
        LOCK_INIT(&gjournal->read_lock);
        LOCK_INIT(&gjournal->log_lock);

        COND_LOCK_INIT(&gjournal->write_cond_lock);

        INIT_LIST_HEAD(&gjournal->group_queue);

        gjournal->thread_shutdown = 0;
        gjournal->disk_cur_offset = 0;
        gjournal->cluster_cur_offset = 0;
        gjournal->thread_num = 0;
        gjournal->ops_group_disk_num = 0;
        gjournal->ops_group_cluster_num = 0;

        gjournal->working_journal_fd = NULL;
        gjournal->execute_journal_fd = NULL;
        gjournal->execute_error_journal_fd = NULL;
        gjournal->check_journal_fd = NULL;

        ret = pthread_create (&(gjournal->thread_num), NULL,
                        thread_writev, NULL);
        if (ret != 0)
                HANDLE_ERROR_EN(ret, "pthread_create");
}

int result_handle_fn (char *buf)
{
        time_t now = {0,};
        struct tm *tm_now = NULL;
        char string[PATH_MAX] = {0,};
        char path[PATH_MAX] = {0,};

        now = time (NULL);
        if (now == ((time_t) -1))
                HANDLE_ERROR("time");

        tm_now = localtime (&now);
        if (tm_now == NULL)
                HANDLE_ERROR("localtime");

        sprintf (string, "[%d-%02d-%02d %02d:%02d:%02d] %s",tm_now->tm_year+1900
                                                           ,tm_now->tm_mon+1
                                                           ,tm_now->tm_mday
                                                           ,tm_now->tm_hour
                                                           ,tm_now->tm_min
                                                           ,tm_now->tm_sec
                                                           ,buf);
        if (gjournal.execute_error_journal_fd) {
                fprintf (gjournal.execute_error_journal_fd, "%s\n", string);
                fflush (gjournal.execute_error_journal_fd);
        } else {
                sprintf (path, "%sexecute_error_journal.log",cgv.working_directory);
                if ((gjournal.execute_error_journal_fd = fopen (path, "a+")) == NULL) {
                        working_journal_log("RESULT", LOG_INFO,
                                        "reopen path = %s failed!",path);
                        fprintf (stderr, "%s\n", string);
                        fflush (stderr);
                } else {
                        fprintf (gjournal.execute_error_journal_fd, "%s\n", string);
                        fflush (gjournal.execute_error_journal_fd);
                }

        }

        return 0;
}

int check_handle_fn (char *buf)
{
        time_t now = {0,};
        struct tm *tm_now = NULL;
        char string[PATH_MAX] = {0,};
        char path[PATH_MAX] = {0,};

        now = time (NULL);
        if (now == ((time_t) -1))
                HANDLE_ERROR("time");

        tm_now = localtime (&now);
        if (tm_now == NULL)
                HANDLE_ERROR("localtime");

        sprintf (string, "[%d-%02d-%02d %02d:%02d:%02d] %s",tm_now->tm_year+1900
                                                           ,tm_now->tm_mon+1
                                                           ,tm_now->tm_mday
                                                           ,tm_now->tm_hour
                                                           ,tm_now->tm_min
                                                           ,tm_now->tm_sec
                                                           ,buf);
        if (gjournal.check_journal_fd) {
                fprintf (gjournal.check_journal_fd, "%s\n", string);
                fflush (gjournal.check_journal_fd);
        } else {
                sprintf (path, "%scheck_journal.log",cgv.working_directory);
                if ((gjournal.check_journal_fd = fopen (path, "a+")) == NULL) {
                        working_journal_log("CHECK", LOG_INFO,
                                        "reopen path = %s failed!",path);
                        fprintf (stderr, "%s\n", string);
                        fflush (stderr);
                } else {
                        fprintf (gjournal.check_journal_fd, "%s\n", string);
                        fflush (gjournal.check_journal_fd);
                }
        }

        return 0;
}
