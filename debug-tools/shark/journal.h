#ifndef __ANALYZE_CONFIG___
#define __ANALYZE_CONFIG___
#include <glib.h>
#include "analyze.h"

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
        
        int         first_open_for_log;
}config_globle_var ;

extern config_globle_var cgv;

#define GET_CONFIG_STRING(src,dst)                                \
        p = strtok (dst, tmp_delim);                              \
        memset (src, 0, sizeof (src));                            \
        strcpy (src, p);                                          \
        tmp_p = strrchr(src, '/');                                \
        tmp_len = strlen (tmp_p);                                 \
        if (tmp_len > 1)                                          \
                strcat (src, "/");                                \
        free (dst);


int
get_operation_percent_int (char *config_file_path,
                           const gchar *group_name, const gchar *key);
double
get_operation_percent (char *config_file_path,
                       const gchar *group_name, const gchar *key);
char *
get_operation_string (char *config_file_path,
                      const gchar *group_name, const gchar *key);

int is_dir_exsit (const char *pathname);
int
config_globle_var_init (config_globle_var *cgv,
                           char *config_file_path);
#endif
