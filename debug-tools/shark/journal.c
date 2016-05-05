#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "log.h"
#include "analyze.h"
#include "journal.h"

config_globle_var cgv;

/* function:Integer percentage of cluster configuration file.
 * If the file could not get the int val then @error is set
 * to either a #GFileError or #GKeyFileError.to
 * @key_file: a #GKeyFile
 * @group_name: a group name such as GLOBAL
 * @key: a key such as DIRECTORY
 * @error:return location for a #GError, or %NULL
 *
 * return: successfule return percent val
 *         failed return -1
 * */
int
get_operation_percent_int (char *config_file_path,
                           const gchar *group_name, const gchar *key)
{
        int ret = -1;
        char *p = NULL;
        char *delim = "% ";
        GError *error = NULL;
        GKeyFile *key_file = NULL;
        gchar *int_percent = NULL;

        key_file = g_key_file_new();
        ret = g_key_file_load_from_file(key_file, config_file_path,
                        G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error);

        if (ret == 0) {
                working_journal_log("CONFIG", LOG_INFO, "Couldn't load configure file! "
                                    "config_file_path = %s,error = %s",config_file_path,
                                                                       error->message);
                exit (EXIT_FAILURE);
                goto out;
        }

        int_percent = g_key_file_get_value (key_file, group_name, key, &error);
        if (int_percent == NULL) {
                working_journal_log("CONFIG", LOG_INFO, "get operation percent int failed! "
                                "group_name = %s,key = %s error = %s"
                                ,group_name, key, error->message);
                exit (EXIT_FAILURE);
                goto out;
        }
        p = strtok (int_percent, delim);

        ret = atoi(p);
out:
        g_key_file_free(key_file);
        free (int_percent);
        return ret;
}

double
get_operation_percent (char *config_file_path,
                       const gchar *group_name, const gchar *key)
{
        int ret = 0;
        int len = 0;
        double ret2 = -1.0;
        char *p = NULL;
        char *q = NULL;
        char *delim = "% ";
        GError *error = NULL;
        GKeyFile *key_file = NULL;
        gchar *int_percent = NULL;

        key_file = g_key_file_new();
        ret = g_key_file_load_from_file(key_file, config_file_path,
                        G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error);

        if (ret == 0) {
                working_journal_log("CONFIG", LOG_INFO, "Couldn't load configure file! "
                                    "config_file_path = %s,error = %s",config_file_path,
                                                                       error->message);
                exit (EXIT_FAILURE);
                goto out;
        }

        int_percent = g_key_file_get_value (key_file, group_name, key, &error);
        if (int_percent == NULL) {
                working_journal_log("CONFIG", LOG_INFO, "get operation percent double failed! "
                                "group_name = %s,key = %s error = %s"
                                ,group_name, key, error->message);
                exit (EXIT_FAILURE);
                goto out;
        }

        p = strtok (int_percent, delim);

        q = strrchr (p, '.');
        q = q + 1;
        len = strlen (q);
        if (len != cgv.accuracy_num) {
                working_journal_log("CONFIG", LOG_INFO, "Accuracy mismatch! "
                                "group_name = %s,key = %s",
                                group_name, key);
                goto out;
        }

        ret2 = atof(p)/100;
out:
        g_key_file_free(key_file);
        free (int_percent);
        return ret2;
}

/* Gets the string in the cluster configuration file.
 * If the file could not get the string then @error is
 * set to either a #GFileError or #GKeyFileError.
 * @key_file: a #GKeyFile.
 * @group_name: a group name. such as GLOBAL
 * @key: a key. such as TEST_DIRECTORY
 * @error:return location for a #GError, or %NULL
 *
 * return: successfule return percent val
 *         failed return -1.0
 * */

char *
get_operation_string (char *config_file_path,
                      const gchar *group_name, const gchar *key)
{
        int ret = 0;
        GError *error = NULL;
        gchar *string = NULL;
        GKeyFile *key_file = NULL;

        key_file = g_key_file_new();
        ret = g_key_file_load_from_file(key_file, config_file_path,
                        G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error);

        if (ret == 0) {
                if (!strcmp (key, "WORKING_DIRECTORY")) {
                        printf ("Couldn't load configure file!"
                                        "config_file_path = %s,error = %s",
                                        config_file_path, error->message);
                } else {
                        working_journal_log("CONFIG", LOG_INFO, "Couldn't load configure file! "
                                        "config_file_path = %s,error = %s",config_file_path,
                                                                           error->message);
                }

                exit (EXIT_FAILURE);
        }

        string = g_key_file_get_value (key_file, group_name, key, &error);
        if (string == NULL) {
                if (!strcmp (key, "WORKING_DIRECTORY")) {
                        printf ("get operation percent string failed!"
                                        "group_name = %s,key = %s error = %s"
                                        ,group_name, key, error->message);
                } else {
                        working_journal_log("CONFIG", LOG_INFO, "get operation percent string failed! "
                                                                "group_name = %s,key = %s error = %s"
                                                                     ,group_name, key, error->message);
                }

                exit (EXIT_FAILURE);
        }

        g_key_file_free(key_file);
        return string;
}

int is_dir_exsit (const char *pathname)
{
        DIR *dirp = NULL;

        if (pathname == NULL) {
                working_journal_log("PATHNAME", LOG_INFO, "pathname is NULL!");
                return -1;
        }
        if ((dirp = opendir (pathname)) == NULL && errno == ENOENT)
                return 0;

        closedir (dirp);

        return 1;
}

int
config_globle_var_init (config_globle_var *cgv,
                           char *config_file_path)
{
        int tmp_int = 0;
        int ret = 0;
        DIR *dirp = NULL;
        int num = 0;
        char *tmp_delim = " ";
        char *p = NULL;
        int tmp_len = 0;
        char *tmp_p = NULL;
        char *tmp_char = NULL;
        double tmp_double = 0.0;
        time_t now = {0,};
        struct tm *tm_now = NULL;
        char timestr[PATH_MAX] = {0,};
        char string[PATH_MAX] = {0,};
        char tar_string[PATH_MAX] = {0,};

        cgv->first_open_for_log = 1;
        memset (cgv->test_dir_path, 0, PATH_MAX);
        memset (cgv->validation_dir_path, 0, PATH_MAX);
        memset (cgv->working_directory, 0, PATH_MAX);

        tmp_char = get_operation_string (config_file_path, "GLOBAL", "WORKING_DIRECTORY");
        GET_CONFIG_STRING(cgv->working_directory, tmp_char);

        if (!is_dir_exsit (cgv->working_directory)) {
                ret = mkdir (cgv->working_directory,
                                S_IREAD | S_IWRITE | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

                if (ret == -1) {
                        working_journal_log("MKDIR", LOG_INFO,
                                        "mkdir failed,procedure will exit! pathname = %s"
                                        ,cgv->working_directory);
                        HANDLE_ERROR("mkdir");
                }

        }

        if ((dirp = opendir(cgv->working_directory)) == NULL) {
                HANDLE_ERROR("opendir");
        } else {
                while (readdir(dirp))
                        num++;

                closedir (dirp);
        }

        if (num > 2) {
                now = time (NULL);
                if (now == ((time_t) -1))
                        HANDLE_ERROR("time");

                tm_now = localtime (&now);
                if (tm_now == NULL)
                        HANDLE_ERROR("localtime");

                sprintf (string, "working_journal.log execute_journal.log execute_error_journal.log "
                                " check_journal.log");

                sprintf (timestr, "%d%02d%02d-%02d:%02d:%02d.tar.gz ",tm_now->tm_year + 1900,
                                                                tm_now->tm_mon + 1, tm_now->tm_mday,
                                                                tm_now->tm_hour, tm_now->tm_min,
                                                                tm_now->tm_sec);

                sprintf (tar_string, "cd %s && tar cvzf %s  %s --force-local > /dev/null",cgv->working_directory, timestr, string);
                system (tar_string);
        }

        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "LOG_LEVEL");
        cgv->log_level = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init log_level val is (%d)!",
                                                                         cgv->log_level);

        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init working_directory val is (%s)!",
                                                                           cgv->working_directory);

        memset (string, 0, sizeof (string));
        sprintf (string, "%sexecute_journal.log",cgv->working_directory);
        if ((gjournal.execute_journal_fd = fopen (string, "w+")) == NULL)
                HANDLE_ERROR("fopen");

        memset (string, 0, sizeof (string));
        sprintf (string, "%sexecute_error_journal.log",cgv->working_directory);
        if ((gjournal.execute_error_journal_fd = fopen (string, "w+")) == NULL)
                HANDLE_ERROR("fopen");

        memset (string, 0, sizeof (string));
        sprintf (string, "%scheck_journal.log",cgv->working_directory);
        if ((gjournal.check_journal_fd = fopen (string, "w+")) == NULL)
                HANDLE_ERROR("fopen");

        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "ACCURACY");
        cgv->accuracy = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init accuracy val is (%d)!",
                                                                         cgv->accuracy);

        tmp_int = tmp_int / 100;
        while(tmp_int / 10) {
                tmp_int /= 10;
                cgv->accuracy_num ++;
        }

        tmp_double = get_operation_percent (config_file_path,
                                            "GLOBAL", "DIRECTORY");
        cgv->dir_op_ratio = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init dir_op_ratio val is (%lf)!",
                                                                          cgv->dir_op_ratio);
        tmp_double = get_operation_percent (config_file_path,
                                            "GLOBAL", "SYMBOL_LINK");
        cgv->symbol_link_op_ratio = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init symbol_link_op_ratio val is (%lf)!",
                                                                          cgv->symbol_link_op_ratio);

        tmp_double = get_operation_percent (config_file_path,
                                            "GLOBAL", "FILE");
        cgv->file_op_ratio = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init file_op_ratio val is (%lf)!",
                                                                          cgv->file_op_ratio);

        tmp_double = cgv->file_op_ratio + cgv->symbol_link_op_ratio + cgv->dir_op_ratio;
        if (fabs(tmp_double - 1.0) >= 1E-6) {
                working_journal_log("CONFIGURE_INIT", LOG_INFO, "dir_op_ratio + symbol_link_op_ratio"
                                                                " + file_op_ratio != 100\% procedure"
                                                                " will exit!");
                exit (EXIT_FAILURE);
        }

        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "WRITE_SIZE");
        cgv->write_size = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init write_size val is (%d)!",
                                                                         cgv->write_size);

        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "READ_SIZE");
        cgv->read_size = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init read_size val is (%d)!",
                                                                         cgv->read_size);
        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "DIRECTORY_MAX_DEPTH");
        cgv->dir_max_depth = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init dir_max_depth val is (%d)!",
                                                                         cgv->dir_max_depth);
        tmp_int = get_operation_percent_int (config_file_path,
                                            "GLOBAL", "INITIAL_DIRECTORY_COUNTS");
        cgv->initial_dir_cnts = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init initial_dir_cnts val is (%d)!",
                                                                          cgv->initial_dir_cnts);
        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "INITIAL_FILE_COUNTS");
        cgv->initial_file_cnts = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init initial_file_cnts val is (%d)!",
                                                                         cgv->initial_file_cnts);

        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "INITIAL_SYMLINK_COUNTS");
        cgv->initial_symlink_counts = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init initial_symlink_counts val is (%d)!",
                                                                         cgv->initial_symlink_counts);
        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "TOTAL_OPERATION_GROUP");
        cgv->total_op_group = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init total_op_group val is (%d)!",
                                                                         cgv->total_op_group);
        tmp_char = get_operation_string (config_file_path, "GLOBAL", "TEST_DIRECTORY");
        GET_CONFIG_STRING(cgv->test_dir_path, tmp_char);

        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init test_dir_path val is (%s)!",
                                                                         cgv->test_dir_path);

        tmp_char = get_operation_string (config_file_path, "GLOBAL", "VALIDATION_DIRECTORY");
        GET_CONFIG_STRING(cgv->validation_dir_path, tmp_char);

        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init validation_dir_path val is (%s)!",
                                                                          cgv->validation_dir_path);

        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "WORKING_THREADS");
        cgv->working_threads = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init working_threads val is (%d)!",
                                                                          cgv->working_threads);

        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "OPERATIONS_WAITING_LIST");
        cgv->operations_waiting_list = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init operations_waiting_list val is (%d)!",
                                                                         cgv->operations_waiting_list);

        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "OPERATIONS_OF_OPENED_FILE");
        cgv->operations_of_opened_file = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init operations_of_opened_file val is (%d)!",
                                                                         cgv->operations_of_opened_file);

        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "OPERATIONS_OF_UNOPENED_FILE");
        cgv->operations_of_unopened_file = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init operations_of_opened_file val is (%d)!",
                                                                         cgv->operations_of_opened_file);

        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "OPERATIONS_OF_OPENED_DIR");
        cgv->operations_of_opened_dir = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init operations_of_opened_dir val is (%d)!",
                                                                          cgv->operations_of_opened_dir);

        tmp_int = get_operation_percent_int (config_file_path,
                                            "GLOBAL", "OPERATIONS_OF_UNOPENED_DIR");
        cgv->operations_of_unopened_dir = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init operations_of_unopened_dir val is (%d)!",
                                                                         cgv->operations_of_unopened_dir);

        tmp_int = get_operation_percent_int (config_file_path,
                                             "GLOBAL", "OPERATIONS_OF_SYMLINK");
        cgv->operations_of_symlink = tmp_int;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init operations_of_symlink val is (%d)!",
                                                                          cgv->operations_of_symlink);

        tmp_double = get_operation_percent (config_file_path,
                                           "GLOBAL", "NEW_DIR");
        cgv->new_dir_ratio = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init new_dir_ratio val is (%lf)!",
                                                                          cgv->new_dir_ratio);

        tmp_double = get_operation_percent (config_file_path,
                                            "GLOBAL", "OPEN_DIR");
        cgv->open_dir_ratio = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init open_dir_ratio val is (%lf)!",
                                                                          cgv->open_dir_ratio);

        tmp_double = get_operation_percent (config_file_path,
                                            "GLOBAL", "NEW_FILE");
        cgv->new_file_ratio = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init new_file_ratio val is (%lf)!",
                                                                          cgv->new_file_ratio);

        tmp_double = get_operation_percent (config_file_path,
                                            "GLOBAL", "OPEN_FILE");
        cgv->open_file_ratio = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init open_file_ratio val is (%lf)!",
                                                                          cgv->open_file_ratio);

        tmp_double = get_operation_percent (config_file_path,
                                           "OPENED_DIR", "FSYNC");
        cgv->fsync_open_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fsync_open_dir val is (%lf)!",
                                                                          cgv->fsync_open_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_DIR", "FSETXATTR");
        cgv->fsetxattr_open_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fsetxattr_open_dir val is (%lf)!",
                                                                           cgv->fsetxattr_open_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_DIR", "FGETXATTR");
        cgv->fgetxattr_open_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fgetxattr_open_dir val is (%lf)!",
                                                                          cgv->fgetxattr_open_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_DIR", "FREMOVEXATTR");
        cgv->fremovexattr_open_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fremovexattr_open_dir val is (%lf)!",
                                                                          cgv->fremovexattr_open_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_DIR", "READDIR");
        cgv->readdir_open_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init readdir_open_dir val is (%lf)!",
                                                                          cgv->readdir_open_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_DIR", "LK");
        cgv->lk_open_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init lk_open_dir val is (%lf)!",
                                                                          cgv->lk_open_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_DIR", "FCHMOD");
        cgv->fchmod_open_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fchmod_open_dir val is (%lf)!",
                                                                          cgv->fchmod_open_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_DIR", "FCHOWN");
        cgv->fchown_open_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fchown_open_dir val is (%lf)!",
                                                                           cgv->fchown_open_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_DIR", "FUTIMES");
        cgv->futimes_open_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init futimes_open_dir val is (%lf)!",
                                                                          cgv->futimes_open_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_DIR", "FSTAT");
        cgv->fstat_open_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fstat_open_dir val is (%lf)!",
                                                                          cgv->fstat_open_dir);
        tmp_double = cgv->fsync_open_dir + cgv->fsetxattr_open_dir +
                     cgv->fgetxattr_open_dir + cgv->fremovexattr_open_dir +
                     cgv->readdir_open_dir + cgv->lk_open_dir +
                     cgv->fchmod_open_dir + cgv->fchown_open_dir +
                     cgv->futimes_open_dir + cgv->fstat_open_dir;

        if (fabs(tmp_double - 1.0) >= 1E-6) {
                printf ("tmp_double = %lf\n",tmp_double);
                working_journal_log("CONFIGURE_INIT", LOG_INFO,
                                     "fsync_open_dir + fsetxattr_open_dir + "
                                     " fgetxattr_open_dir + fremovexattr_open_dir +"
                                     " readdir_open_dir + lk_open_dir + fchmod_open_dir"
                                     " + fchown_open_dir + futimes_open_dir + fstat_open_dir"
                                     " != 100\%% procedure will exit!");
                exit (EXIT_FAILURE);
        }

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_DIR", "ACCESS");
        cgv->access_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init access_unopen_dir val is (%lf)!",
                                                                          cgv->access_unopen_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_DIR", "RENAME");
        cgv->rename_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init rename_unopen_dir val is (%lf)!",
                                                                          cgv->rename_unopen_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_DIR", "SYMLINK");
        cgv->symlink_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init symlink_unopen_dir val is (%lf)!",
                                                                          cgv->symlink_unopen_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_DIR", "SETXATTR");
        cgv->setxattr_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init setxattr_unopen_dir val is (%lf)!",
                                                                          cgv->setxattr_unopen_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_DIR", "GETXATTR");
        cgv->getxattr_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init getxattr_unopen_dir val is (%lf)!",
                                                                          cgv->getxattr_unopen_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_DIR", "REMOVEXATTR");
        cgv->removexattr_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init removexattr_unopen_dir val is (%lf)!",
                                                                           cgv->removexattr_unopen_dir);

        tmp_double = get_operation_percent (config_file_path,
                                           "UNOPENED_DIR", "CHOWN");
        cgv->chown_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init chown_unopen_dir val is (%lf)!",
                                                                                 cgv->chown_unopen_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_DIR", "CHMOD");
        cgv->chmod_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init chmod_unopen_dir val is (%lf)!",
                                                                           cgv->chmod_unopen_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_DIR", "UTIMES");
        cgv->utimes_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init utimes_unopen_dir val is (%lf)!",
                                                                          cgv->utimes_unopen_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_DIR", "RMDIR");
        cgv->rmdir_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init rmdir_unopen_dir val is (%lf)!",
                                                                          cgv->rmdir_unopen_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_DIR", "STAT");
        cgv->stat_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init stat_unopen_dir val is (%lf)!",
                                                                          cgv->stat_unopen_dir);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_DIR", "STATFS");
        cgv->statfs_unopen_dir = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init statfs_unopen_dir val is (%lf)!",
                                                                           cgv->statfs_unopen_dir);

        tmp_double = cgv->access_unopen_dir + cgv->rename_unopen_dir +
                     cgv->symlink_unopen_dir + cgv->setxattr_unopen_dir +
                     cgv->getxattr_unopen_dir + cgv->removexattr_unopen_dir +
                     cgv->chmod_unopen_dir + cgv->rmdir_unopen_dir +
                     cgv->statfs_unopen_dir + cgv->chown_unopen_dir +
                     cgv->utimes_unopen_dir + cgv->stat_unopen_dir;

        if (fabs(tmp_double - 1.0) >= 1E-6) {
                working_journal_log("CONFIGURE_INIT", LOG_INFO,
                                "access_unopen_dir + rename_unopen_dir +"
                                " symlink_unopen_dir + setxattr_unopen_dir + "
                                " getxattr_unopen_dir + removexattr_unopen_dir +"
                                " rmdir_unopen_dir + fchmod_unopen_dir"
                                " + chown_unopen_dir + utimes_unopen_dir +"
                                " stat_unopen_dir + statfs_unopen_dir"
                                " != 100\% procedure will exit!");
                exit (EXIT_FAILURE);
        }

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "WRITEV");
        cgv->writev_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init writev_open_file val is (%lf)!",
                                                                          cgv->writev_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "READV");
        cgv->readv_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init readv_open_file val is (%lf)!",
                                                                          cgv->readv_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "FALLOCATE");
        cgv->fallocate_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fallocate_open_file val is (%lf)!",
                                                                          cgv->fallocate_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "FTRUNCATE");
        cgv->ftruncate_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init ftruncate_open_file val is (%lf)!",
                                                                          cgv->ftruncate_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "FSYNC");
        cgv->fsync_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fsync_open_file val is (%lf)!",
                                                                          cgv->fsync_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                           "OPENED_FILE", "FSETXATTR");
        cgv->fsetxattr_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fsetxattr_open_file val is (%lf)!",
                                                                          cgv->fsetxattr_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "FGETXATTR");
        cgv->fgetxattr_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fgetxattr_open_file val is (%lf)!",
                                                                           cgv->fgetxattr_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "FREMOVEXATTR");
        cgv->fremovexattr_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fremovexattr_open_file val is (%lf)!",
                                                                                 cgv->fremovexattr_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "LK");
        cgv->lk_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init lk_open_file val is (%lf)!",
                                                                          cgv->lk_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "FCHOWN");
        cgv->fchown_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fchown_open_file val is (%lf)!",
                                                                                 cgv->fchown_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "FCHMOD");
        cgv->fchmod_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fchmod_open_file val is (%lf)!",
                                                                                 cgv->fchmod_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "FUTIMES");
        cgv->futimes_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init futimes_open_file val is (%lf)!",
                                                                          cgv->futimes_open_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "OPENED_FILE", "FSTAT");
        cgv->fstat_open_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init fstat_open_file val is (%lf)!",
                                                                          cgv->fstat_open_file);
        tmp_double = cgv->writev_open_file + cgv->readv_open_file +
                     cgv->fallocate_open_file + cgv->ftruncate_open_file +
                     cgv->fsync_open_file + cgv->fsetxattr_open_file +
                     cgv->fgetxattr_open_file + cgv->fremovexattr_open_file +
                     cgv->lk_open_file + cgv->fchmod_open_file +
                     cgv->fchown_open_file + cgv->futimes_open_file +
                     cgv->fstat_open_file;

        if (fabs(tmp_double - 1.0) >= 1E-6) {
                working_journal_log("CONFIGURE_INIT", LOG_INFO, "writev_open_file + readv_open_file +"
                                                                " fallocate_open_file + ftruncate_open_file + "
                                                                " fsetxattr_open_file + fsync_open_file +"
                                                                " fgetxattr_open_file + fremovexattr_open_file +"
                                                                " lk_open_file + fchmod_open_file"
                                                                " + chown_open_file + futimes_open_file +"
                                                                " fstat_open_file != 100\% procedure will exit!");
                exit (EXIT_FAILURE);
        }

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "RENAME");
        cgv->rename_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init rename_unopen_file val is (%lf)!",
                                                                          cgv->rename_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "SYMLINK");
        cgv->symlink_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init symlink_unopen_file val is (%lf)!",
                                                                           cgv->symlink_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "LINK");
        cgv->link_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init link_unopen_file val is (%lf)!",
                                                                           cgv->link_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                           "UNOPENED_FILE", "TRUNCATE");
        cgv->truncate_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init truncate_unopen_file val is (%lf)!",
                                                                           cgv->truncate_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "SETXATTR");
        cgv->setxattr_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init setxattr_unopen_file val is (%lf)!",
                                                                           cgv->setxattr_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "GETXATTR");
        cgv->getxattr_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init getxattr_unopen_file val is (%lf)!",
                                                                                 cgv->getxattr_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "REMOVEXATTR");
        cgv->removexattr_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init removexattr_unopen_file val is (%lf)!",
                                                                           cgv->removexattr_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "ACCESS");
        cgv->access_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init access_unopen_file val is (%lf)!",
                                                                                 cgv->access_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "CHOWN");
        cgv->chown_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init chown_unopen_file val is (%lf)!",
                                                                                 cgv->chown_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "CHMOD");
        cgv->chmod_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init chmod_unopen_file val is (%lf)!",
                                                                                 cgv->chmod_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "UTIMES");
        cgv->utimes_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init utimes_unopen_file val is (%lf)!",
                                                                           cgv->utimes_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "STAT");
        cgv->stat_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init stat_unopen_file val is (%lf)!",
                                                                           cgv->stat_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                           "UNOPENED_FILE", "UNLINK");
        cgv->unlink_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init unlink_unopen_file val is (%lf)!",
                                                                           cgv->unlink_unopen_file);

        tmp_double = get_operation_percent (config_file_path,
                                            "UNOPENED_FILE", "STATFS");
        cgv->statfs_unopen_file = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init statfs_unopen_file val is (%lf)!",
                                                                                 cgv->statfs_unopen_file);
        tmp_double = cgv->link_unopen_file + cgv->rename_unopen_file + 
                     cgv->symlink_unopen_file + cgv->setxattr_unopen_file +
                     cgv->getxattr_unopen_file + cgv->removexattr_unopen_file +
                     cgv->access_unopen_file + cgv->truncate_unopen_file +
                     cgv->chmod_unopen_file + cgv->unlink_unopen_file +
                     cgv->statfs_unopen_file + cgv->chown_unopen_file +
                     cgv->utimes_unopen_file + cgv->stat_unopen_file;

        if (fabs(tmp_double - 1.0) >= 1E-6) {
                working_journal_log("CONFIGURE_INIT", LOG_INFO, "link_unopen_file + rename_unopen_file +"
                                                                " symlink_unopen_file + setxattr_unopen_file + "
                                                                " getxattr_unopen_file + removexattr_unopen_file +"
                                                                " fchmod_unopen_file + unlink_unopen_file"
                                                                " + chown_unopen_file + utimes_unopen_file +"
                                                                " stat_unopen_file + statfs_unopen_file"
                                                                " != 100\% procedure will exit!");
                exit (EXIT_FAILURE);
        }

        tmp_double = get_operation_percent (config_file_path,
                                           "SYMBOL_LINK", "READLINK");
        cgv->readlink_symbol_link = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init readlink_symbol_link val is (%lf)!",
                                                                           cgv->readlink_symbol_link);

        tmp_double = get_operation_percent (config_file_path,
                                            "SYMBOL_LINK", "UNLINK");
        cgv->unlink_symbol_link = tmp_double;
        working_journal_log("CONFIGURE_INIT", LOG_INFO, "init unlink_symbol_link val is (%lf)!",
                                                                        cgv->unlink_symbol_link);

        tmp_double = cgv->readlink_symbol_link + cgv->unlink_symbol_link;
        if (fabs(tmp_double - 1.0) >= 1E-6) {
                working_journal_log("CONFIGURE_INIT", LOG_INFO, "readlink_symbol_link +"
                                                                " unlink_symbol_link "
                                                                "!= 100\% procedure will exit!");
                exit (EXIT_FAILURE);
        }

        return 0;
}
