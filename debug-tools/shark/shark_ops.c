/**********************************************
AUTHOR:zhd
TIME:Sat 26 Dec 2015 10:03:49 AM CST
FILENAME:shark_ops.c
DESCRIPTION:.............
**********************************************/
#include "shark_ops.h"
#include "shark_file.h"
#include "journal.h"
#include "log.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <sys/time.h>
#include <sys/file.h>

#if 0
extern config_globle_var cgv = {
        .dir_op_ratio = 0.1700, /* dir 操作概率 */ 
        .symbol_link_op_ratio = 0.0205, /* symlink 操作概率 */ 
        .file_op_ratio = 0.8095, /* file 操作概率 */

        .accuracy = 10000,
        .write_size = 512,
        .read_size = 512,

        .dir_max_depth = 10,
        .initial_dir_cnts = 10000,
        .initial_file_cnts = 10000,
        .initial_symlink_counts = 10000,
        .total_op_group = 1000000,

        .test_dir_path = "/mnt/disk_sde",
        .validation_dir_path = "/mnt/disk_sdd",

        .operations_waiting_list = 500,
        .operations_of_opened_file = 500, 
        .operations_of_unopened_file = 3,
        .operations_of_opened_dir = 3,
        .operations_of_unopened_dir = 3, 
        .operations_of_symlink = 1,

        .new_dir_ratio = 0.0800,
        .open_dir_ratio = 0.8922,

        .new_file_ratio = 0.5000,
        .open_file_ratio = 0.9000,

        .fsync_open_dir = 0.1000,
        .fsetxattr_open_dir = 0.1000,
        .fgetxattr_open_dir = 0.1000,
        .fremovexattr_open_dir = 0.1000,
        .readdir_open_dir = 0.1000,
        .lk_open_dir = 0.1000,
        .fchmod_open_dir = 0.1000,
        .fchown_open_dir = 0.1000,
        .futimes_open_dir = 0.1000,
        .fstat_open_dir = 0.1000,

        .access_unopen_dir = 0.1000,
        .rename_unopen_dir = 0.0000,
        .symlink_unopen_dir = 0.1000,
        .setxattr_unopen_dir = 0.1000,
        .getxattr_unopen_dir = 0.1000,
        .removexattr_unopen_dir = 0.1000,
        .chown_unopen_dir = 0.1000,
        .chmod_unopen_dir = 0.1000,
        .utimes_unopen_dir = 0.1000,
        .rmdir_unopen_dir = 0.0000,
        .stat_unopen_dir = 0.1000,
        .statfs_unopen_dir = 0.1000,

        .writev_open_file = 0.1000,
        .readv_open_file = 0.1000,
        .fallocate_open_file = 0.1000,
        .ftruncate_open_file = 0.1000,
        .fsync_open_file = 0.1000,
        .fsetxattr_open_file = 0.1000,
        .fgetxattr_open_file = 0.1000,
        .fremovexattr_open_file = 0.0500,
        .lk_open_file = 0.0500,
        .fchown_open_file = 0.0500,
        .fchmod_open_file = 0.0500,
        .futimes_open_file = 0.0500,
        .fstat_open_file = 0.0500,

        .rename_unopen_file = 0.1000,
        .symlink_unopen_file = 0.1000,
        .link_unopen_file = 0.1000,
        .truncate_unopen_file = 0.1000,
        .setxattr_unopen_file = 0.1000,
        .getxattr_unopen_file = 0.1000,
        .removexattr_unopen_file = 0.1000,
        .access_unopen_file = 0.0500,
        .chown_unopen_file = 0.0500,
        .chmod_unopen_file = 0.0500,
        .utimes_unopen_file = 0.0500,
        .stat_unopen_file = 0.0500,
        .unlink_unopen_file = 0.0500,
        .statfs_unopen_file = 0.0500,

        .readlink_symbol_link = 0.5000,
        .unlink_symbol_link = 0.5000,
};
#endif

extern int                      total_ops_cnt;
extern file_node_table_t        *file_node_table;
extern config_globle_var        cgv;

int
list_all_random() {
        int             ret = -1;
        layout_table_t  *layout_table;
        layout_t        *layout = NULL;
        int             layout_type = 0;

        for (layout_type = 0;layout_type < LAYOUT_TYPE_MAX;layout_type++) {
                
                working_journal_log("SHARK_OPS",LOG_DEBUG,"*****Layout_type is %d*****",layout_type);

                layout_table = global_random_layout_table[layout_type]; 
                list_for_each_entry (layout, &layout_table->layouts, layout_list)
                {
                        working_journal_log("SHARK_OPS",LOG_DEBUG,"layout begin is %"PRId64",\tend is %"PRId64",\tit's key is %d",
                                        layout->start,layout->end,layout->key);
                }

        }
        return ret;
}

layout_table_t *
create_layout_table (int type)
{
        layout_table_t             *hbt = NULL;
        layout_t                   *hb = NULL;
        int                             i = 1;
        int                             last_end = 0;
        
        hbt = (layout_table_t *)calloc(1,sizeof(layout_table_t));
        if (!hbt) {
                working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                goto out;/* error */
        }
        hbt->accuracy = cgv.accuracy;
        INIT_LIST_HEAD (&hbt->layouts);

        switch (type) {
                case OPS_FILE_TYPE_DISTRIBUTION:
                        hbt->layout_cnt = 3;
                        for (i = 1;i <= hbt->layout_cnt ;i++){
                                hb = (layout_t *)calloc(1,sizeof(layout_t));
                                if (hb == NULL) {
                                        working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                                        goto out;/* error */
                                }
                                INIT_LIST_HEAD (&hb->layout_list);
                                hb->start = last_end;
                                
                                if ( i == 1) {/* dir */
                                        hb->end = (int)last_end + hbt->accuracy*(cgv.dir_op_ratio); /* dir ratio */
                                } else if (i == 2){ /* symlink */
                                        hb->end = last_end + hbt->accuracy*(cgv.symbol_link_op_ratio); /* symlink ratio */
                                } else { /* file */
                                        hb->end = last_end + hbt->accuracy*(cgv.file_op_ratio);/* file ratio */
                                }
                                last_end = hb->end;
                                hb->key = i;
                                list_add (&hb->layout_list, &hbt->layouts);
                        }
                        break;
                case NEWDIR_DISTRIBUTION:
                        hbt->layout_cnt = 2;
                        for (i = 1;i <= hbt->layout_cnt ;i++){
                                hb = (layout_t *)calloc(1,sizeof(layout_t));
                                if (hb == NULL) {
                                        working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                                        goto out;/* error */
                                }
                                INIT_LIST_HEAD (&hb->layout_list);
                                hb->start = last_end;
                                
                                if ( i == 1) {/* new dir */
                                        hb->key = 1;
                                        hb->end = last_end + hbt->accuracy*(cgv.new_dir_ratio); /* new dir ratio */
                                } else { /* use exist dir */
                                        hb->key = 0;
                                        hb->end = last_end + hbt->accuracy*(1 - cgv.new_dir_ratio);/* use exist ratio */
                                }
                                last_end = hb->end;
                                list_add (&hb->layout_list, &hbt->layouts);
                        }
                        break;
                case OPENDIR_DISTRIBUTION:
                        hbt->layout_cnt = 2;
                        for (i = 1;i <= hbt->layout_cnt ;i++){
                                hb = (layout_t *)calloc(1,sizeof(layout_t));
                                if (hb == NULL) {
                                        working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                                        goto out;/* error */
                                }
                                INIT_LIST_HEAD (&hb->layout_list);
                                hb->start = last_end;
                                
                                if ( i == 1) {/* opendir */
                                        hb->key = OPENED_DIR_OP;
                                        hb->end = last_end + hbt->accuracy*(cgv.open_dir_ratio); /* opendir ratio */
                                } else { /* no_opendir */
                                        hb->key = NO_OPEN_DIR_OP;
                                        hb->end = last_end + hbt->accuracy*(1 - cgv.open_dir_ratio);/* no_opendir ratio */
                                }
                                last_end = hb->end;
                                list_add (&hb->layout_list, &hbt->layouts);
                        }
                        break;
                case OPENDIR_OPS_DISTRIBUTION:
                        hbt->layout_cnt = O_MAX_DIR_OP - 1;
                        for (i = 1;i <= hbt->layout_cnt;i++) {
                                hb = (layout_t *)calloc(1,sizeof(layout_t));
                                if (hb == NULL) {
                                        working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                                        goto out;/* error */
                                }
                                INIT_LIST_HEAD (&hb->layout_list);
                                hb->start = last_end;
                                
                                switch (i) {
                                        case FSYNC_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fsync_open_dir);
                                                break;
                                        case FSETXATTR_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fsetxattr_open_dir);
                                                break;
                                        case FGETXATTR_DIR_OP: 
                                                hb->end = last_end + hbt->accuracy*(cgv.fgetxattr_open_dir);
                                                break;
                                        case FREMOVEXATTR_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fremovexattr_open_dir);
                                                break;
                                                /*
                                        case LK_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.readdir_open_dir);
                                                break;
                                                */
                                        case READDIR_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.readdir_open_dir);
                                                break;
                                        case FCHOWN_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fchown_open_dir);
                                                break;
                                        case FCHMOD_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fchmod_open_dir);
                                                break;
                                        case FUTIMES_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.futimes_open_dir);
                                                break;
                                        case FSTAT_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fstat_open_dir);
                                                break;
                                        default:
                                                hb->end = last_end;
                                                break;
                                }

                                last_end = hb->end;
                                hb->key = i;
                                list_add (&hb->layout_list, &hbt->layouts);
                        }
                        break;
                case NO_OPENDIR_OPS_DISTRIBUTION:
                        hbt->layout_cnt = NO_MAX_DIR_OP - 1;
                        for (i = 1;i<= hbt->layout_cnt;i++){
                                hb = (layout_t *)calloc(1,sizeof(layout_t));
                                if (hb == NULL) {
                                        working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                                        goto out;/* error */
                                }
                                INIT_LIST_HEAD (&hb->layout_list);
                                hb->start = last_end;
                                switch (i) {
                                        case SYMLINK_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.symlink_unopen_dir);
                                                break;
                                        case SETXATTR_DIR_OP: 
                                                hb->end = last_end + hbt->accuracy*(cgv.setxattr_unopen_dir);
                                                break;
                                        case RENAME_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.rename_unopen_dir);
                                                break;
                                        case ACCESS_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.access_unopen_dir);
                                                break;
                                        case GETXATTR_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.getxattr_unopen_dir);
                                                break;
                                        case REMOVEXATTR_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.removexattr_unopen_dir);
                                                break;
                                        case CHOWN_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.chown_unopen_dir);
                                                break;
                                        case CHMOD_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.chmod_unopen_dir);
                                                break;
                                        case UTIMES_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.utimes_unopen_dir);
                                                break;
                                        case RMDIR_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.rmdir_unopen_dir);
                                                break;
                                        case STAT_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.stat_unopen_dir);
                                                break;
                                        case STATFS_DIR_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.statfs_unopen_dir);
                                                break;
                                        default:
                                                hb->end = last_end;
                                                break;
                                }

                                last_end = hb->end;
                                hb->key = i;
                                list_add (&hb->layout_list, &hbt->layouts);
                        }
                        break;
                case NEWFILE_DISTRIBUTION:
                        hbt->layout_cnt = 2;
                        for (i = 1;i <= hbt->layout_cnt ;i++){
                                hb = (layout_t *)calloc(1,sizeof(layout_t));
                                if (hb == NULL) {
                                        working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                                        goto out;/* error */
                                }
                                INIT_LIST_HEAD (&hb->layout_list);
                                hb->start = last_end;
                                
                                if ( i == 1) {/* new file */
                                        hb->key = 1;
                                        hb->end = last_end + hbt->accuracy*(cgv.new_file_ratio); /* new file ratio */
                                } else { /* use exist file */
                                        hb->key = 0;
                                        hb->end = last_end + hbt->accuracy*(1 - cgv.new_file_ratio);/* use exist file ratio */
                                }
                                last_end = hb->end;
                                list_add (&hb->layout_list, &hbt->layouts);
                        }
                        break;
                case OPENFILE_DISTRIBUTION:
                        hbt->layout_cnt = 2;
                        for (i = 1;i <= hbt->layout_cnt ;i++){
                                hb = (layout_t *)calloc(1,sizeof(layout_t));
                                if (hb == NULL) {
                                        working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                                        goto out;/* error */
                                }
                                INIT_LIST_HEAD (&hb->layout_list);
                                hb->start = last_end;
                                
                                if ( i == 1) {/* open file  */
                                        hb->key = OPENED_FILE_OP;
                                        hb->end = last_end + hbt->accuracy*(cgv.open_file_ratio); /* open file ratio */
                                } else { /* no_openfile */
                                        hb->key = NO_OPEN_FILE_OP;
                                        hb->end = last_end + hbt->accuracy*(1 - cgv.open_file_ratio);/* no_openfile ratio */
                                }
                                last_end = hb->end;
                                list_add (&hb->layout_list, &hbt->layouts);
                        }
                        break;
                case OPENFILE_OPS_DISTRIBUTION:
                        hbt->layout_cnt = O_MAX_FILE_OP - 1;
                        for (i = 1;i <= hbt->layout_cnt;i++) {
                                hb = (layout_t *)calloc(1,sizeof(layout_t));
                                if (hb == NULL) {
                                        working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                                        goto out;/* error */
                                }
                                INIT_LIST_HEAD (&hb->layout_list);
                                hb->start = last_end;
                                switch (i) {
                                        case WRITEV_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.writev_open_file);
                                                break;
                                        case READV_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.readv_open_file);
                                                break;
                                        case FALLOCATE_FILE_OP: 
                                                hb->end = last_end + hbt->accuracy*(cgv.fallocate_open_file);
                                                break;
                                        case FTRUNCATE_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.ftruncate_open_file);
                                                break;
                                        case LK_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.lk_open_file);
                                                break;
                                        case FSYNC_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fsync_open_file);
                                                break;
                                        case FSETXATTR_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fsetxattr_open_file);
                                                break;
                                        case FGETXATTR_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fgetxattr_open_file);
                                                break;
                                        case FREMOVEXATTR_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fremovexattr_open_file);
                                                break;
                                        case FCHOWN_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fchown_open_file);
                                                break;
                                        case FCHMOD_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fchmod_open_file);
                                                break;
                                        case FUTIMES_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.futimes_open_file);
                                                break;
                                        case FSTAT_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.fstat_open_file);
                                                break;
                                        default:
                                                hb->end = last_end;
                                                break;
                                }

                                last_end = hb->end;
                                hb->key = i;
                                list_add (&hb->layout_list, &hbt->layouts);
                        }
                        break;
                case NO_OPENFILE_OPS_DISTRIBUTION:
                        hbt->layout_cnt = NO_MAX_FILE_OP - 1;
                        for (i = 1;i <= hbt->layout_cnt;i++) {
                                hb = (layout_t *)calloc(1,sizeof(layout_t));
                                if (hb == NULL) {
                                        working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                                        goto out;/* error */
                                }
                                INIT_LIST_HEAD (&hb->layout_list);
                                hb->start = last_end;
                                switch (i) {
                                        case RENAME_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.rename_unopen_file);
                                                break;
                                        case SYMLINK_FILE_OP: 
                                                hb->end = last_end + hbt->accuracy*(cgv.symlink_unopen_file);
                                                break;
                                        case LINK_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.link_unopen_file);
                                                break;
                                        case TRUNCATE_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.truncate_unopen_file);
                                                break;
                                        case SETXATTR_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.setxattr_unopen_file);
                                                break;
                                        case GETXATTR_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.getxattr_unopen_file);
                                                break;
                                        case REMOVEXATTR_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.removexattr_unopen_file);
                                                break;
                                        case ACCESS_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.access_unopen_file);
                                                break;
                                        case CHOWN_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.chown_unopen_file);
                                                break;
                                        case CHMOD_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.chmod_unopen_file);
                                                break;
                                        case UTIMES_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.utimes_unopen_file);
                                                break;
                                        case STAT_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.stat_unopen_file);
                                                break;
                                        case UNLINK_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.unlink_unopen_file);
                                                break;
                                        case STATFS_FILE_OP:
                                                hb->end = last_end + hbt->accuracy*(cgv.statfs_unopen_file);
                                                break;
                                        default:
                                                hb->end = last_end;
                                                break;
                                }

                                last_end = hb->end;
                                hb->key = i;
                                list_add (&hb->layout_list, &hbt->layouts);
                        }
                        break;
                case SYMLINK_OPS_DISTRIBUTION:
                        hbt->layout_cnt = 2;
                        for (i = 1;i <= hbt->layout_cnt ;i++){
                                hb = (layout_t *)calloc(1,sizeof(layout_t));
                                if (hb == NULL) {
                                        working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                                        goto out;/* error */
                                }
                                INIT_LIST_HEAD (&hb->layout_list);
                                hb->start = last_end;
                                
                                if ( i == 1) {/* readlink  */
                                        hb->end = last_end + hbt->accuracy*(cgv.readlink_symbol_link); /* open file ratio */
                                } else { /* rm symlink */
                                        hb->end = last_end + hbt->accuracy*(cgv.unlink_symbol_link);/* no_openfile ratio */
                                }
                                last_end = hb->end;
                                hb->key = i;
                                list_add (&hb->layout_list, &hbt->layouts);
                        }
                        break;
                default:
                        break;
        }
out:
        return hbt;
}

int
create_layout_table_list()
{
        int             table_index = 0;
        int             ret = -1;

        for (; table_index < LAYOUT_TYPE_MAX;table_index++){
                global_random_layout_table[table_index] = create_layout_table(table_index);
                if (global_random_layout_table[table_index] == NULL) {
                        working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                        goto out;
                        /* error */
                }
        }
        ret = 0;
out:
        return ret;
}

int
random_handler(int layout_type)
{
        int             ret = -1;
        int             hash_val = 0;
        layout_table_t  *layout_table;
        layout_t        *layout = NULL;
        int             range = 0;

        layout_table = global_random_layout_table[layout_type]; 
        list_for_each_entry (layout, &layout_table->layouts, layout_list)
        {
                range = (range > layout->end) ? range : layout->end;
        }
        if (layout_table->accuracy == 0) {
                working_journal_log ("SHARK_OPS",LOG_ERROR,"accuracy is 0,can't as remainder");
                goto out;
        }
        hash_val = random()%(layout_table->accuracy);

        list_for_each_entry (layout, &layout_table->layouts, layout_list)
        {
                if (layout->start <= hash_val && hash_val < layout->end) {
                        working_journal_log("SHARK_OPS",LOG_TRACE,"val is %d,start is %d,end is %d,key is %d\n",
                                        hash_val,layout->start,layout->end,layout->key);
                        ret = layout->key;
                        break;
                }
        }
out:
        return ret;
}

static int
random_xattr_val_str(char *str,int *len)
{
        int             ret = -1;
        char            *tmp = NULL;

        if (str && len) {
                memset(str,0,XATTR_VAL_LEN);
                strncpy(str,XATTR_VAL_STR,XATTR_VAL_LEN-1);
                tmp = mktemp(str);
                if (!tmp) {
                        working_journal_log ("SHARK_OPS",LOG_ERROR,"failed to generate a random string");
                        goto out;
                }
                *len = XATTR_KEY_LEN;
                ret = 0;
       }
out:
        return ret;
}

static int
random_xattr_key_str(char *str,int *len)
{
        int             ret = -1;
        char            *tmp = NULL;

        if (str && len) {
                memset(str,0,XATTR_KEY_LEN);
                strncpy(str,XATTR_KEY_STR,XATTR_KEY_LEN-1);
                tmp = mktemp(str);
                if (!tmp) {
                        working_journal_log ("SHARK_OPS",LOG_ERROR,"failed to generate a random string");
                        goto out;
                }
                *len = XATTR_KEY_LEN;
                ret = 0;
        }
out:
        return ret;
}

file_op_t *
file_op_new (file_ops_t* fops, int op, int op_index)
{
        file_op_t               *file_op;
        int                     file_type;
        int                     op_type;
        file_node_t             *file_tmp = NULL;

        char                    xattr_key_buf[XATTR_KEY_LEN] = {0};
        char                    xattr_val_buf[XATTR_VAL_LEN] = {0};
        int                     key_len = 0;
        int                     val_len = 0;
        char                    *newpath = NULL;
        off_t                   pseudo_off = 1024*1024;/**/

        working_journal_log ("SHARK_OPS",LOG_DEBUG,"op index is %d",op_index);

        file_op = (file_op_t *)calloc(1,sizeof(file_op_t));
        if (!file_op) {
                /* error */
                working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                goto out;
        }
        INIT_LIST_HEAD (&file_op->oplist);
        file_op->op = op;
        file_op->op_index = op_index;
        file_type = fops->file_type;
        op_type = fops->op_type;

        switch (op_type) {
                case OPENED_DIR_OP:
                        switch (op) {
                                case FSYNC_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FSYNC_DIR_OP");
                                        break;

                                case FSETXATTR_DIR_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                               strncpy(file_op->args.fsetxattr.name,xattr_key_buf,key_len);
                                        }
                                        if (!random_xattr_val_str(xattr_val_buf,&val_len)) {
                                               strncpy(file_op->args.fsetxattr.value,xattr_val_buf,val_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FSETXATTR_DIR_OP,key is %s,val is %s",xattr_key_buf,xattr_val_buf);
                                        file_op->args.fsetxattr.flags = XATTR_REPLACE;
                                        break;

                                case FGETXATTR_DIR_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                                strncpy(file_op->args.fgetxattr.name,xattr_key_buf,key_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"Get FGETXATTR_DIR_OP,key is %s",xattr_key_buf);
                                        break;

                                case FREMOVEXATTR_DIR_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                                strncpy(file_op->args.fremovexattr.name,xattr_key_buf,key_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"Get REMOVEXATTR_DIR_OP,key is %s",xattr_key_buf);
                                        break;
                                
                                case READDIR_DIR_OP: 
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get READDIR_DIR_OP");
                                        break;

                                case FCHOWN_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FCHOWN_DIR_OP");
                                        file_op->args.chown.owner = 0;
                                        file_op->args.chown.group = 0;
                                        break;

                                case FCHMOD_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FCHMOD_DIR_OP");
                                        file_op->args.chmod.mode = 667;
                                        break;

                                case FUTIMES_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FUTIMES_DIR_OP");
                                        gettimeofday(&file_op->args.utimes.time[0], NULL);
                                        gettimeofday(&file_op->args.utimes.time[1], NULL);
                                        break;

                                case FSTAT_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FSTAT_DIR_OP");
                                        break;
                                default:
                                        break;
                        }
                        break;
                case NO_OPEN_DIR_OP:
                        switch (op) {
                                case ACCESS_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get ACCESS_DIR_OP");
                                        file_op->args.access.mode = R_OK|W_OK|X_OK|F_OK;
                                        break;

                                case RENAME_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get RENAME_DIR_OP");
                                        file_op->op = O_UNKNOWN_DIR_OP;
                                        break;

                                case SYMLINK_DIR_OP:/* create new file_node */
                                        file_tmp = file_node_create(SYMLINK_TYPE,1);
                                        if (file_tmp) {
                                                newpath = get_path_for_file_node(file_tmp);
                                                if (newpath) {
                                                        strncpy(file_op->args.symlink.newpath,newpath,strlen(newpath));
                                                }
                                                free(newpath);
                                                newpath = NULL;
                                        } else {
                                                ;/* error */
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get SYMLINK_DIR_OP,newpath is %s",file_op->args.symlink.newpath);

                                        break;

                                case SETXATTR_DIR_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                               strncpy(file_op->args.setxattr.name,xattr_key_buf,key_len);
                                        }
                                        if (!random_xattr_val_str(xattr_val_buf,&val_len)) {
                                               strncpy(file_op->args.setxattr.value,xattr_val_buf,val_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"Get SETXATTR_DIR_OP,key is %s,val is %s",xattr_key_buf,xattr_val_buf);
                                        file_op->args.setxattr.flags = XATTR_REPLACE;
                                        break;

                                 case GETXATTR_DIR_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                                strncpy(file_op->args.getxattr.name,xattr_key_buf,key_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"Get GETXATTR_DIR_OP,key is %s",xattr_key_buf);
                                        break;

                                 case REMOVEXATTR_DIR_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                                strncpy(file_op->args.removexattr.name,xattr_key_buf,key_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"Get REMOVEXATTR_DIR_OP,key is %s",xattr_key_buf);
                                        break;

                                 case CHOWN_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get CHOWN_DIR_OP");
                                        file_op->args.chown.owner = 0;
                                        file_op->args.chown.group = 0;
                                        break;

                                 case CHMOD_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get CHMOD_DIR_OP");
                                        file_op->args.chmod.mode = 667;
                                        break;

                                 case UTIMES_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get UTIMES_DIR_OP");
                                        gettimeofday(&file_op->args.utimes.time[0], NULL);
                                        gettimeofday(&file_op->args.utimes.time[1], NULL);
                                        break;

                                 case RMDIR_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get RMDIR_DIR_OP");
                                        file_op->op = O_UNKNOWN_DIR_OP;
                                        break;

                                 case STAT_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get STAT_DIR_OP");
                                        break;
                                 case STATFS_DIR_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get STATFS_DIR_OP");
                                        break;
                                 default:
                                        break;
                         }
                         break;
                case OPENED_FILE_OP:
                        switch (op) {
                                case WRITEV_FILE_OP:
                                        file_op->args.pwritev.offset = random()%(pseudo_off);
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get WRITEV_FILE_OP,offset is %llu",
                                                        file_op->args.pwritev.offset);
                                        break;
                                case READV_FILE_OP:
                                        file_op->args.preadv.offset = random()%(pseudo_off);
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get READV_FILE_OP,offset is %llu",
                                                        file_op->args.preadv.offset);
                                        break;

                                case FALLOCATE_FILE_OP:
                                        file_op->args.fallocate.mode = 0;
                                        file_op->args.fallocate.offset = random()%(pseudo_off);
                                        file_op->args.fallocate.len = 512;
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FALLOCATE_FILE_OP,offset is %llu,",
                                                        "len is %d, mode is %d",
                                                        file_op->args.fallocate.offset,file_op->args.fallocate.len,
                                                        file_op->args.fallocate.mode);
                                        break;

                                case FTRUNCATE_FILE_OP:
                                        file_op->args.ftruncate.length = random()%(pseudo_off);
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FTRUNCATE_FILE_OP,length is %llu",
                                                        file_op->args.ftruncate.length);
                                        break;

                                case FSYNC_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FSYNC_FILE_OP");
                                        break;

                                case FSETXATTR_FILE_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                               strncpy(file_op->args.fsetxattr.name,xattr_key_buf,key_len);
                                        }
                                        if (!random_xattr_val_str(xattr_val_buf,&val_len)) {
                                               strncpy(file_op->args.fsetxattr.value,xattr_val_buf,val_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FSETXATTR_FILE_OP,key is %s,val is %s",xattr_key_buf,xattr_val_buf);
                                        file_op->args.setxattr.flags = XATTR_REPLACE;
                                        break;
                                case FGETXATTR_FILE_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                                strncpy(file_op->args.fgetxattr.name,xattr_key_buf,key_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FGETXATTR_FILE_OP,key is %s",xattr_key_buf);
                                        break;
                                case FREMOVEXATTR_FILE_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                                strncpy(file_op->args.fremovexattr.name,xattr_key_buf,key_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get REMOVEXATTR_FILE_OP,key is %s",xattr_key_buf);
                                        break;
                                case LK_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get LK_FILE_OP");
                                        file_op->args.lk.operation = LOCK_EX;
                                        break;

                                case FCHOWN_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FCHOWN_FILE_OP");
                                        file_op->args.chown.owner = 0;
                                        file_op->args.chown.group = 0;
                                        break;
                                 case FCHMOD_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FCHMOD_FILE_OP");
                                        file_op->args.chmod.mode = 667;
                                        break;
                                 case FUTIMES_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FUTIMES_FILE_OP");
                                        gettimeofday(&file_op->args.utimes.time[0], NULL);
                                        gettimeofday(&file_op->args.utimes.time[1], NULL);
                                         break;

                                 case FSTAT_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get FSTAT_FILE_OP");
                                        break;
                                 default:
                                        break;
                         }
                         break;
                case NO_OPEN_FILE_OP:
                         switch (op){
                                 case RENAME_FILE_OP:
                                 case LINK_FILE_OP:/* need new file_node  */
                                        file_tmp = file_node_create(FILE_TYPE,1);
                                        if (file_tmp) {
                                                newpath = get_path_for_file_node(file_tmp);
                                                if (newpath) {
                                                        if (op == RENAME_FILE_OP) {
                                                                strncpy(file_op->args.rename.newpath,newpath,strlen(newpath));
                                                                working_journal_log("SHARK_OPS",LOG_DEBUG,"get RENAME_FILE_OP,newpath is %s",newpath);
                                                        } else {
                                                                working_journal_log("SHARK_OPS",LOG_DEBUG,"get LINK_FILE_OP,newpath is %s",newpath);
                                                                strncpy(file_op->args.link.newpath,newpath,strlen(newpath));
                                                        }
                                                }
                                                free(newpath);
                                                newpath = NULL;
                                        } else {
                                                ;/* error */
                                        }
                                        break;
                                 case SYMLINK_FILE_OP:
                                        file_tmp = file_node_create(SYMLINK_TYPE,1);
                                        if (file_tmp) {
                                                newpath = get_path_for_file_node(file_tmp);
                                                if (newpath) {
                                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get SYMLINK_FILE_OP,newpath is %s",newpath);
                                                        strncpy(file_op->args.symlink.newpath,newpath,strlen(newpath));
                                                }
                                                free(newpath);
                                                newpath = NULL;
                                        } else {
                                                ;/* error */
                                        }
                                        break;

                                 case TRUNCATE_FILE_OP:
                                        file_op->args.truncate.length = random()%(pseudo_off);
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get TRUNCATE_FILE_OP,length is %llu",
                                                        file_op->args.truncate.length);
                                        break;

                                 case SETXATTR_FILE_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                               strncpy(file_op->args.setxattr.name,xattr_key_buf,key_len);
                                        }
                                        if (!random_xattr_val_str(xattr_val_buf,&val_len)) {
                                               strncpy(file_op->args.setxattr.value,xattr_val_buf,val_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get SETXATTR_FILE_OP,key is %s,val is %s",xattr_key_buf,xattr_val_buf);
                                        file_op->args.setxattr.flags = XATTR_REPLACE;
                                        break;
                                 case GETXATTR_FILE_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                                strncpy(file_op->args.getxattr.name,xattr_key_buf,key_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get GETXATTR_FILE_OP,key is %s",xattr_key_buf);
                                        break;
                                 case REMOVEXATTR_FILE_OP:
                                        if (!random_xattr_key_str(xattr_key_buf,&key_len)) {
                                                strncpy(file_op->args.removexattr.name,xattr_key_buf,key_len);
                                        }
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get REMOVEXATTR_FILE_OP,key is %s",xattr_key_buf);
                                        break;
                                 case ACCESS_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get ACCESS_FILE_OP");
                                        file_op->args.access.mode = R_OK|W_OK|X_OK|F_OK;
                                        break;
                                 case CHOWN_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get CHOWN_FILE_OP");
                                        file_op->args.chown.owner = 0;
                                        file_op->args.chown.group = 0;
                                        break;
                                 case CHMOD_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get CHMOD_FILE_OP");
                                        file_op->args.chmod.mode = 667;
                                        break;
                                 case UTIMES_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get UTIMES_FILE_OP");
                                        gettimeofday(&file_op->args.utimes.time[0], NULL);
                                        gettimeofday(&file_op->args.utimes.time[1], NULL);
                                        break;

                                 case STAT_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get STAT_FILE_OP");
                                        break;
                                 case UNLINK_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get UNLINK_FILE_OP");
                                        break;
                                 case STATFS_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get STATFS_FILE_OP");
                                        break;
                                 default:
                                        break;
                         }
                         break;
                case SYMLINK_OP:
                        switch (op) {
                                case READLINK_SYMLINK_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get READLINK_SYMLINK_OP");
                                        break;
                                case UNLINK_FILE_OP:
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"get READLINK_SYMLINK_OP");
                                        break;
                                default:
                                        break;
                        }
                        break;
                default:
                        break;
        }
out:
        return file_op;
}

int
file_op_destroy(file_op_t *file_op)
{
        /* remove file_node from busy_file_list */
        if (file_op) {
                free(file_op);
                file_op = NULL;
        }
        return 0;
}

file_ops_t *
file_oplist_create(int index)
{
        file_ops_t      *foplist;
        int             type;
        int             op;
        int             op_index;
        file_op_t       *fop;
        int             i;
        file_node_t     *file = NULL;
        uint64_t        curr_index = 0;
        uint64_t        file_index = 0;
        int             ret = -1;

        foplist = (file_ops_t *)calloc(1,sizeof (file_ops_t));
        if (!foplist) {
                working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                goto out;
        }
        INIT_LIST_HEAD (&foplist->op_group);
        INIT_LIST_HEAD (&foplist->ops);
        foplist->ops_index = index;


        working_journal_log("SHARK_OPS",LOG_DEBUG,"Oplist index is %d",index);
        type = random_handler(OPS_FILE_TYPE_DISTRIBUTION);
        //type = FILE_TYPE;
        foplist->file_type = type;
        if (type == DIR_TYPE) {/* dir */
                type = random_handler (NEWDIR_DISTRIBUTION);
                foplist->is_new = type;
                if (type == 1) {/* new dir */
                        foplist->op_type = NO_OPEN_DIR_OP;
                        file = file_node_create(DIR_TYPE, 1);
                        working_journal_log("SHARK_OPS",LOG_DEBUG,"create dir,dir name is %s",file->file_name);
                        if (file) {
                                foplist->file = file;
                        } else {
                                /* error */
                                working_journal_log("SHARK_OPS",LOG_ERROR,"create file node failure,file type is DIR");
                                goto free_oplist;
                        }
                        op_index = 1;
                } else { /* exist dir */
                        do {/* select a idle dir for operation */
                                curr_index = file_node_table->dir_cnt;
                                if (curr_index == 0) {
                                        printf ("There is no dirs\n");/*error*/
                                } else if (curr_index == 1) {
                                        file_index = 1;
                                } else {
                                        file_index = random()%(curr_index);
                                }

                                //printf ("curr_index is %llu,file_index is %llu\n",curr_index,file_index);
                                
                                file = file_delete_form_rbt(DIR_TYPE,file_index,1);
                                if (!file) {
                                        working_journal_log ("SHARK_OPS", LOG_TRACE, "Can't get a file node form "
                                                        "idle dir rbtree,dir_index is %"PRId64"",file_index);
                                        usleep(10000);
                                }
                        }while (!file);

                        foplist->file = file;

                        working_journal_log("SHARK_OPS",LOG_DEBUG,"op on exist dir,dir name is %s",file->file_name);
                        type = random_handler (OPENDIR_DISTRIBUTION);
                        if (type == 1) {
                                foplist->op_type = OPENED_DIR_OP;
                        } else {
                                foplist->op_type = NO_OPEN_DIR_OP;
                        }
                        if (type == 1) {/* need open */
                                working_journal_log("SHARK_OPS",LOG_DEBUG,"open dir");
                                for (i = 0;i < cgv.operations_of_opened_dir;i++) {
                                        op_index = i;
                                        op = random_handler (OPENDIR_OPS_DISTRIBUTION);
                                        fop = file_op_new (foplist, op, op_index );
                                        if (fop) {
                                                list_add_tail (&fop->oplist, &foplist->ops);
                                        }
                                }
                        } else {/* no open */
                                /**
                                 * TODO: mkdir is not available
                                 **/
                                working_journal_log("SHARK_OPS",LOG_DEBUG,"unopen dir");
                                for (i = 0;i < cgv.operations_of_unopened_dir;i++) {
                                        op_index = i;
                                        op = random_handler (NO_OPENDIR_OPS_DISTRIBUTION);
                                        fop = file_op_new (foplist, op, op_index );
                                        if (fop) {
                                                list_add_tail (&fop->oplist, &foplist->ops);
                                        }
                                }
                        }
                        ret = file_insert_to_rbt(DIR_TYPE,file,0);
                        if (ret == -1) {
                                printf("in set failed,file_name is %s\n",file->file_name);
                        }
                        file_node_usecnt_add(file);
                }
        }else if (type == SYMLINK_TYPE) { /**/ 
                foplist->is_new = 0; 
                foplist->op_type = SYMLINK_OP;
                do { /* select a symlink for operation */
                        curr_index = file_node_table->symlink_cnt;
                        if (curr_index == 0) {
                                ;/* error */
                        } else if (curr_index == 1) {
                                file_index = 1;
                        } else {
                                do {
                                        file_index = random()%(curr_index);
                                }while(!file_index);
                                //printf ("curr_index is %llu,file_index is %llu\n",curr_index,file_index);
                        }

                        file = file_delete_form_rbt(SYMLINK_TYPE,file_index,1);
                        if (!file) {
                                working_journal_log ("SHARK_OPS", LOG_TRACE, "Can't get a file node form idle "
                                                "symlink rbtree,symlink_index is %"PRId64"",file_index);
                                usleep(10000);
                        }
                }while(!file);

                working_journal_log("SHARK_OPS",LOG_DEBUG,"operation symlink,name is %s",file->file_name);
                foplist->file = file;

                for (i = 0;i < cgv.operations_of_symlink;i++) { 
                        op_index = i;
                        op = random_handler (SYMLINK_OPS_DISTRIBUTION);
                        fop = file_op_new (foplist, op, op_index);
                        if (fop) {
                                list_add_tail (&fop->oplist, &foplist->ops);
                        }
                }
                ret = file_insert_to_rbt(SYMLINK_TYPE,file,0);
                if (ret == -1) {
                        printf("in set failed,file_name is %s\n",file->file_name);
                }
                file_node_usecnt_add(file);
        } else { /* file */ 
                //printf ("Get file type is FILE,");
                type = random_handler (NEWFILE_DISTRIBUTION);
                foplist->is_new = type;
                if (type == 1){ /* new */
                        file = file_node_create (FILE_TYPE, 1);
                        foplist->op_type = OPENED_FILE_OP;
                        working_journal_log("SHARK_OPS",LOG_DEBUG,"create file,name is %s",file->file_name);
                        goto fileopen_op;
                } else { /* exist*/ 
                        do {/* select a file for operation */
                                curr_index = file_node_table->file_cnt;
                                if (curr_index == 0) {
                                        ;/* error */
                                } else if (curr_index == 1) {
                                        file_index = 1;
                                } else {
                                        do {
                                                file_index = random()%(curr_index);
                                        }while(!file_index);
                                        //printf ("curr_index is %llu,file_index is %llu\n",curr_index,file_index);
                                }
                                file = file_delete_form_rbt(FILE_TYPE,file_index,1);
                                if (!file) {
                                        working_journal_log ("SHARK_OPS", LOG_TRACE, "Can't get a file node form "
                                                        "idle file rbtree,file_index is %"PRId64"",file_index);
                                        usleep(10000);
                                }
                        }while (!file);

                        working_journal_log("SHARK_OPS",LOG_DEBUG,"operation exist file,"
                                                "name is %s",file->file_name);
                        type = random_handler (OPENFILE_DISTRIBUTION);
                        if (type == 1) {
                                foplist->op_type = OPENED_FILE_OP;
                        } else {
                                foplist->op_type = NO_OPEN_FILE_OP;
                        }

                        if (type == 1) {/* open */
                                working_journal_log("SHARK_OPS",LOG_TRACE,"open file");
fileopen_op:
                                for (i = 0;i < cgv.operations_of_opened_file;i++) {
                                        op_index = i;
                                        op = random_handler (OPENFILE_OPS_DISTRIBUTION);
                                        fop = file_op_new (foplist, op, op_index);
                                        if (fop) {
                                                list_add_tail (&fop->oplist, &foplist->ops);
                                        }
                                }
                        } else {
                                working_journal_log("SHARK_OPS",LOG_TRACE,"unopen file");
                                for (i = 0;i < cgv.operations_of_unopened_file;i++) {
                                        op = random_handler (NO_OPENFILE_OPS_DISTRIBUTION);
                                        fop = file_op_new (foplist, op, i);
                                        if (fop) {
                                                list_add_tail (&fop->oplist, &foplist->ops);
                                        }
                                }
                        }
                        foplist->file = file;
                        if (foplist->is_new == 0) {
                                ret = file_insert_to_rbt(FILE_TYPE,file,0);
                                if (ret == -1) {
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"in set failed,file_"
                                                        "name is %s\n",file->file_name);
                                }
                                file_node_usecnt_add(file);
                        }
                }
        }
        return foplist;

free_oplist:
        free(foplist);
        foplist = NULL;
out:
        return foplist;
}

file_ops_group_t *
file_ops_group_new (int opsgroups_index)
{
        file_ops_group_t        *pfog = NULL;
        file_ops_t              *foplist = NULL;
        int                     i = 0;

        working_journal_log("SHARK_OPS",LOG_DEBUG,"Group index is %d",opsgroups_index);
        pfog = (file_ops_group_t *)calloc(1,sizeof (file_ops_group_t));
        if (!pfog) {
                working_journal_log ("SHARK_OPS",LOG_ERROR,"Failed to allocate memory");
                goto out;/* error */
        }

        pfog->opsgroups_index = opsgroups_index;
        INIT_LIST_HEAD (&pfog->opsgroup);
        INIT_LIST_HEAD (&pfog->group_list);
        pthread_mutex_init (&pfog->lock,NULL);

        for (i = 0;i < cgv.operations_waiting_list;i++) {
                working_journal_log ("SHARK_OPS",LOG_DEBUG,"========================"
                                "==================total_ops_cnt is %d",total_ops_cnt);
                if (0 >= total_ops_cnt--) {
                        if (i == 0) {
                                free (pfog);
                                pfog = NULL;
                        }
                        goto out;
                }
                foplist = file_oplist_create (i);
                list_add_tail (&foplist->op_group, &pfog->opsgroup);
                //sleep(1);
        }
        pfog->ops_cnt = i;
out:
        return pfog;
}

int
file_ops_group_destroy(file_ops_group_t *group)
{
        int             ret = -1;

        file_ops_t      *oplist = NULL;
        file_ops_t      *tmp_list = NULL;
        file_op_t       *op = NULL;
        file_op_t       *tmp_op = NULL;
        
        if (!group) {
                goto out;
        }
        list_for_each_entry_safe (oplist,tmp_list,&group->opsgroup,op_group) {
                if(oplist) {
                        list_del_init (&oplist->op_group);
                        list_for_each_entry_safe (op,tmp_op,&oplist->ops,oplist) {
                                if (op) {
                                        list_del_init (&op->oplist);
                                        free(op);
                                        op = NULL;
                                }
                        }
                        pthread_mutex_destroy(&oplist->lock);
                        free(oplist);
                        oplist = NULL;
                }
        }
        pthread_mutex_destroy(&group->lock);
        free(group);
        group = NULL;
        ret = 0;
out:
        return ret;
}

void
list_group_ops(file_ops_group_t *group)
{
        file_ops_t              *oplist = NULL;
        file_op_t               *op = NULL;
        char                    *path = NULL;
        if (group) {
                list_for_each_entry (oplist, &group->opsgroup,op_group) {
                        if (oplist) {
                                path = get_path_for_file_node(oplist->file);
                                working_journal_log("SHARK_OPS",LOG_DEBUG,"=========================="
                                                "=======================================");
                                working_journal_log("SHARK_OPS",LOG_DEBUG,"oplist: is_new = %d, file_type = %d,"
                                                "op_type is %d",oplist->is_new,oplist->file_type,oplist->op_type);
                                working_journal_log("SHARK_OPS",LOG_DEBUG,"oplist: path is %s",path);
#if 0
                                printf("=================================================================");
                                printf("oplist: is_new = %d, file_type = %d,"
                                                "op_type is %d",oplist->is_new,oplist->file_type,oplist->op_type);
                                printf("oplist: path is %s",path);
#endif                                
                                free (path);
                                list_for_each_entry (op, &oplist->ops, oplist) {
                                        working_journal_log("SHARK_OPS",LOG_DEBUG,"op is %d",op->op);
                                        //printf("op is %d\n",op->op);
                                }
                        }
                }
        }
}

void *
group_routine(void *args)
{
        int                     group_index = 0;
        file_ops_group_t        *group;
        
        total_ops_cnt = cgv.total_op_group;
        //printf("==================================Start create ops group===================================\n");
        while(1) {
                group = file_ops_group_new(group_index);
                if (!group) {
                        break;
                }
                group_index++;
                //printf("Now operation the group,loging in some file\n");
                working_journal_log("SHARK_OPS",LOG_DEBUG,"Now operation the group,loging in some file");
                handle_groups(group);
        }
        working_journal_log("SHARK_OPS",LOG_DEBUG,"group_routine thread is ending");
        return NULL;
}

int
file_ops_group_create()
{
        int             ret = -1;
        pthread_t       tid;

        ret = pthread_create(&tid,NULL,group_routine,NULL);
        if (ret == -1) {
                printf("Create group thread failed\n");
        }
        return ret;
}


