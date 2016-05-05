/**********************************************
AUTHOR:zhd
TIME:Sat 26 Dec 2015 09:59:19 AM CST
FILENAME:glusterfs_test.c
DESCRIPTION:.............
**********************************************/
#include "shark_file.h"
#include "shark_ops.h"
#include "journal.h"
#include <unistd.h>
#include <time.h>

/*linshiyue*/
#include "shark_posix.h"
#include "checkout.h"
/*linshiyue*/
extern file_node_table_t       *file_node_table;
extern config_globle_var        cgv;
extern global_journal_t         gjournal;
extern int                      total_ops_cnt;            


/*linshiyue*/
extern thread_manager  *manager;
extern error_list      *all_error_list;
extern thread_pool     *check_pool;
extern  int             check_flag;


extern unsigned int    groups_index ;
extern  int             conf_thread_size;


extern char  digioceand_mount[1024] ;
extern char  disk_mount[1024] ;
extern int   digioceand_mount_size ;
extern int   disk_mount_size;
/*linshiyue*/
#if 1
int
main(int argc, char **argv)
{
        int                     ret = -1;
        printf("global config init\n");

        gjournal_init(&gjournal);
        config_globle_var_init (&cgv, "./conf");

        srandom(time(NULL)+getpid());
        file_node_table = file_node_table_initialize();
        printf("initial dirnum is %d,filenum is %d, symlinknum is %d\n",
                        cgv.initial_dir_cnts,cgv.initial_file_cnts,cgv.initial_symlink_counts);
        create_original_file(cgv.initial_dir_cnts,cgv.initial_file_cnts,cgv.initial_symlink_counts);

        create_layout_table_list();
        list_all_random();
        file_ops_group_create();
        //sleep(10);

/*linshiyue*/
	manager_state   state;
	get_configuration();
	error_init();
	if(!all_error_list){
                printf("all_error_list  is  NULL \n");
                return -1;
        }

        manager_init();
        if(!manager){
                printf("init  manager  error ,pool  fail  \n");
                exit(-1);
        }
	while(1){
		sleep(3);
		pthread_mutex_lock(&manager->mutex);
                state = check_pool_state();
                ret = get_manager_state(state);
		pthread_mutex_unlock(&manager->mutex);
		if(ret == -1){
                        printf("------------  -1\n");
                        exit(-1);
                }
		if(ret == 1){
                        printf("------------  1\n");
			goto check;
		}
	}

check:

	free(manager);
        check_pool_init(digioceand_mount, check_type);
        //error_path_list         *error1;
        //error_path_list         *error2;

	if(!check_pool){
                printf("pool_init  is  fail \n");
                return -1;
        }

	pthread_t               pth;

        ret = pthread_create(&pth, NULL, (void *)check_all, NULL);

        if(ret == -1){

                printf("pthread_create  is  fail  \n");
                return -1;
        }

        while(1){
//		printf("check_flag----->%d\n",check_flag);
                sleep(1);
                if(check_flag == 1)
                {
                        printf("checkout done ......\n");
                        goto destroy_check;
                }
        }

destroy_check:
	checkout_pool_destroy();
        pthread_cancel(pth);

	free_error_list();
        if (file_node_table) {
                file_node_table_destroy(file_node_table);
        }
        gjournal_procedure_destroy();
        return ret;
}
#endif

#if 0
int
main()
{
        int                     ret = 0;
        int                     i = 0;
        file_node_t             *file = NULL;
        
        printf("global config init\n");

        gjournal_init(&gjournal);
        config_globle_var_init (&cgv, "./conf");

        srandom(time(NULL)+getpid());

        file_node_table = file_node_table_initialize();
        create_original_file(25,25,25);
#if 0
        for(i = 0;i<25;i++) {
                file = file_delete_form_rbt(DIR_TYPE,i,1);
                if (file) {
                        printf("file_name is %s\n",file->file_name);
                }
        }
#endif
        if (file_node_table) {
                file_node_table_destroy(file_node_table);
        }
        return ret;
}
#endif
