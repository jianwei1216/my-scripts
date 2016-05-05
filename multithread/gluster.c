/* *
 * GLUSTERFSAPI
 * This program is to test multi pthreads and multi process.
 * 2015-10-13 15:47:00
 * zjw
 * This program has three-part:
 *      1. log
 *      2. read/write
 *      3. thread/process
 * */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <glusterfs/api/glfs.h>

/* debug value */
//#define __JUST_READ_NOT_WRITE_
//#define __JUST_WRITE_NOT_READ_

/* global value */
unsigned int read_count = 3;
unsigned int write_count = 3;
glfs_t *fs = NULL;
#define READ_BUF_SIZE      65536
#define WRITE_BUF_SIZE     65536 
#define FILE_SIZE          32768 /*2GB*/
#define FILE_NAME_SIZE     256
#define FILE_NAME_PREFIX   "/newfile"
#define LOG_PATH           "/var/log/glusterfs/myinfo.log"
#define SERVER_NAME		   "test"

/* multi pthread val*/
#define PTHREAD_READ_COUNT      read_count
#define PTHREAD_WRITE_COUNT     write_count

/* muli process val */
#define FORK_READ_COUNT        read_count
#define FORK_WRITE_COUNT       write_count

/* log */
int log_fd = -1;
void log_start (void)
{
    log_fd = open (LOG_PATH, O_RDWR|O_APPEND|O_CREAT, 0644);
    if (log_fd == -1) {
        perror("open log:");
        exit (-1); 
    }

    return;
}

void log_end (void)
{
	if (log_fd != -1)
		close (log_fd);
    return;
}

void log_info (const char* file, const int line, 
               const char*func, const char* info)
{
    int ret = -1;
    time_t time_data = 0;
    struct timeval time_now = {0, };
    struct tm* time_str = NULL;
    char time_buf[256] = {0,};
    char *fmts = "%Y-%m-%d %T";
    char msg_buf[512] = {0, };

    ret = gettimeofday (&time_now, NULL);
    if (ret == -1) {
        perror ("gettimeofday():");
        exit (-1);
    }
    
    time_data = time_now.tv_sec;
    time_str = gmtime(&time_data);
    strftime(time_buf, sizeof(time_buf), fmts, time_str);
    sprintf (msg_buf, "[%s.%-6ld] [%s:%d:%s] %s\n",\
             time_buf, time_now.tv_usec,\
             file, line, func, info);
    if (write (log_fd, msg_buf, strlen(msg_buf)) != strlen(msg_buf)){
        perror("write log");
        exit (-1);
    }

    return;     
}
#define myinfo(info) log_info(__FILE__, __LINE__, __FUNCTION__, info)

/* read */
void read_test (const char* name)
{
    char *read_buf = NULL;
    int read_cnt = -1;
    glfs_fd_t* read_fd = NULL;
    struct stat tmp = {0, };
    char tmp_file[256] = {0, };
    char msg[512] = {0,};
    int lstat_ret = -1;
    int lstat_count = 0;

    if (!name) {
        myinfo("arg is NULL");
        goto err;
    }
    sprintf (tmp_file, "%s.tmp", name);
    read_buf = calloc(READ_BUF_SIZE,sizeof(char));
    if (!read_buf) {
        myinfo(strerror(errno));
        goto err;
    }

    /* file is exist? */
    while(lstat_ret = glfs_lstat(fs, name, &tmp)) {
        if(lstat_count > 200) {
			myinfo(strerror(ENOENT));
            goto err;
        }
        lstat_count++;
        usleep(25000);
    }

    /* open */
    read_fd = glfs_open (fs, name, O_RDONLY);
    if (read_fd == NULL) {
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "[READ]:open %s failed: %s", name, strerror(errno));
        myinfo(msg);
        goto err;
    }
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "[READ]:open %s success", name);
    myinfo(msg);
    /* read */
    while (1) {
        memset(read_buf, '\0', READ_BUF_SIZE);
        read_cnt = glfs_read (read_fd, read_buf, READ_BUF_SIZE-1, 0);
        if (read_cnt == 0) {
            if (glfs_lstat (fs, tmp_file, &tmp) < 0) {
                myinfo ("[READ]:end-of-file");
                break;
            }
            continue;
        } else if (read_cnt == -1) {
            myinfo (strerror(errno));
            break;
        }
        read_buf[read_cnt] = '\0';
        printf ("%d\n", read_cnt);
    }
    /* close */
	if (read_fd) {
		glfs_close (read_fd);
		read_fd = NULL;
	}
err:
    if (read_buf) {
        free (read_buf);
        read_buf = NULL;
    }

    return;
}

/* write */
void write_test (const char* name)
{
    glfs_fd_t *write_fd = NULL;
    glfs_fd_t *tmp_fd = NULL;
    int write_cnt = -1;
    int file_size = 0;
    char *write_buf = NULL;
    char tmp_file[256] = {0,};
    struct stat tmp = {0,};
    char msg[512] = {0, };

    if (!name) {
        myinfo("arg is NULL");
        goto err;
    }

    if (glfs_lstat(fs, name, &tmp) == 0) {
        /* file is already exist! */
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "[WRITE]:%s is already exist! create it failed!", name);
        myinfo(msg);
        goto err;
    }

    sprintf(tmp_file, "%s.tmp", name);
    write_buf = calloc (WRITE_BUF_SIZE, sizeof(char));
    if (!write_buf) {
        myinfo(strerror(errno));
        goto err;
    }
    /* create tmp_file */
    tmp_fd = glfs_creat(fs, tmp_file, O_RDWR, 0644); 
    if (tmp_fd == NULL) {
        myinfo(strerror(errno));
        goto err;
    } else {
        glfs_close(tmp_fd);
    }
    /* create file */
    write_fd = glfs_creat(fs, name, O_WRONLY, 0644);
    if (write_fd == NULL) {
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "[WRITE]:create %s failed: %s", name, strerror(errno));
        myinfo(msg); 
        glfs_unlink(fs, tmp_file);
        goto err;
    }
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "[WRITE]:create %s sucess", name);
    myinfo(msg); 
    /* write */
    memset(write_buf, '1', WRITE_BUF_SIZE);
    while (1) {
        if (file_size > FILE_SIZE) {
            myinfo("[WRITE]:write-over-file");
            break;
        }
        write_cnt = glfs_write (write_fd, write_buf, WRITE_BUF_SIZE, 0); 
        if (write_cnt != WRITE_BUF_SIZE) {
            myinfo(strerror(errno));
            break;
        }
        file_size++;
    }
    /* close */
    glfs_close(write_fd);  
    glfs_unlink(fs, tmp_file);
err:
    if (write_buf) {
        free(write_buf);
        write_buf = NULL;
    }
    return;
}

void* pthread_write (void* arg)
{
    write_test((char*)arg);

    return;
}

void* pthread_read (void* arg)
{
    read_test((char*)arg);  

    return;
}

/* just read */
void just_read (void)
{
	const char* filename = "/testfile-gluster";
	int i = 0;

	read_test(filename);

	return;
}

/* just write */
void just_write (void)
{
	const char* filename = "/testfile-gluster";
	
	write_test(filename);

	return;
}

/* multi pthread */
void pthread_test (void)
{
    pthread_t wpid[PTHREAD_WRITE_COUNT];
    pthread_t rpid[PTHREAD_READ_COUNT];
    char filename[PTHREAD_WRITE_COUNT][FILE_NAME_SIZE];
    int i = 0;
    int lstat_ret = -1;
    struct stat tmp = {0,};

    //srand((unsigned int)time(NULL));
    srand((unsigned int)1);
    for (i = 0; i < PTHREAD_WRITE_COUNT; i++) {
        memset(filename[i], '\0', FILE_NAME_SIZE);
        sprintf(filename[i], "%s%d", FILE_NAME_PREFIX, i);
    }

#ifdef __JUST_WRITE_NOT_READ_
    /* pthread write */
    for (i = 0; i < PTHREAD_WRITE_COUNT; i++) {
        wpid[i] = -1;
        if (pthread_create(&wpid[i],\
            NULL, pthread_write, (void*)filename[i]) != 0) {
            myinfo(strerror(errno));
            continue;
        }
    }
#endif

#ifdef __JUST_READ_NOT_WRITE_
    /* pthread_read */
    for (i = 0; i < PTHREAD_READ_COUNT; i++) {
        rpid[i] = -1;
        if (pthread_create(&rpid[i], NULL, pthread_read,\
            (void*)filename[i%PTHREAD_WRITE_COUNT])) {
            myinfo(strerror(errno));
            continue;
        }
    }
#endif

    /* pthread join */
#ifdef __JUST_WRITE_NOT_READ_
    for (i = 0; i < PTHREAD_WRITE_COUNT; i++) {
        if (pthread_join(wpid[i], NULL)) {
            myinfo(strerror(errno));
        }
    }
#endif

#ifdef __JUST_READ_NOT_WRITE_
    for (i = 0; i < PTHREAD_READ_COUNT; i++) {
        if (pthread_join(rpid[i], NULL)) {
            myinfo(strerror(errno));
        }
    }
#endif

    return;
}

/* multi process */
void process_test (void)
{
    char filename[FORK_WRITE_COUNT][FILE_NAME_SIZE];
    int i = 0;
    pid_t wpid[FORK_WRITE_COUNT];
    pid_t rpid[FORK_READ_COUNT];

    //srand((unsigned int)time(NULL));
    srand((unsigned int)1);
    for (i = 0; i < FORK_WRITE_COUNT; i++) {
        memset(filename[i], '\0', FILE_NAME_SIZE); 
        sprintf(filename[i], "%s%d",FILE_NAME_PREFIX, rand());
    }

#ifdef __JUST_WRITE_NOT_READ_
    /* fork write */  
    for (i = 0; i < FORK_WRITE_COUNT; i++) {
        wpid[i] = -1;
        wpid[i] = fork(); 
        if (wpid[i] == -1) {
            myinfo(strerror(errno));
            continue;
        } else if (wpid[i] == 0) {
            /* create file in the child */
            write_test(filename[i]);           
            return; 
        }
    }
#endif

#ifdef __JUST_READ_NOT_WRITE_
    /* fork read */
    for (i = 0; i < FORK_READ_COUNT; i++) {
        rpid[i] = -1;
        rpid[i] = fork();
        if (rpid[i] == -1) {
            myinfo(strerror(errno));
            continue;
        } else if (rpid[i] == 0) {
            /* read file in the child */
            read_test(filename[i%FORK_WRITE_COUNT]);
            return;
        }
    }
#endif

#ifdef __JUST_WRITE_NOT_READ_
    /* wait */
    for (i = 0; i < FORK_WRITE_COUNT; i++) {
        if (wpid[i] == -1) {
            continue;
        }
        if (waitpid(wpid[i], NULL, 0) == -1) {
            myinfo("waitpid():wpid error"); 
            continue;
        }
    }
#endif

#ifdef __JUST_READ_NOT_WRITE_
    for (i = 0; i < FORK_READ_COUNT; i++) {
        if (rpid[i] == -1) {
            continue;
        }
        if (waitpid(rpid[i], NULL, 0) == -1) {
            myinfo("waitpid(): rpid error");
            continue;
        }
    }
#endif

    return;
}

/* init server */
int server_init(glfs_t **fs)
{
	if (!fs) {
		myinfo("arg is NULL");
		goto err;
	}
	*fs = NULL;
	/* glfs_new */
	*fs = glfs_new(SERVER_NAME);	
	if (*fs == NULL) {
		myinfo(strerror(errno));
		goto err;
	}
	/* glfs_set_volfile_server */
	if (glfs_set_volfile_server(*fs, "tcp", "127.0.0.1", 0) == -1) {
		myinfo(strerror(errno));
		goto err;
	}
	/* glfs_init */	
	if (glfs_init(*fs) == -1) {
		myinfo(strerror(errno));
		goto err;
	}	
	return 0;
err:
	if (*fs) {
		glfs_fini(*fs);
		*fs = NULL;
	}
	return -1;
}

/*fini server */
int server_fini(glfs_t *fs)
{
	if (fs == NULL) {
		myinfo("arg is NULL");
		return -1;
	}
#if 0 /**/
	if (glfs_unset_volfile_server(fs, "tcp", "127.0.0.1", 0) == -1) {
		myinfo(strerror(errno));
	}
#endif
	if (glfs_fini(fs) != 0) {
		myinfo(strerror(errno));
		return -1;
	}
	return 0;
}

/* main */
int main(int argc, char* argv[])
{
    log_start();
	if (argc == 1) {
		goto usage;
	}
	/* init server */
	if (server_init(&fs) == -1) {
		goto out;
	}	

	/* just write files */		
	if (!strcmp("-write", argv[1]) && argc == 2) {
		just_write();
			
		goto out;
	}
	/* just read files */
	if (!strcmp("-read", argv[1]) && argc == 2) {
		just_read();

		goto out;
	}

	if (argc == 4) {
		/* write read files */
		write_count = atoi(argv[2]);
		read_count = atoi(argv[3]);
		if (read_count < 0 || read_count > 1000) {
			printf ("read_count range is 1~1000\n"); 
			goto out;
		}
		if (write_count < 0 || write_count > 1000) {
			printf ("write_count range is 1~1000\n"); 
			goto out;
		}
	}
    /* multi pthread */
    if (!strcmp("-pthread", argv[1]) && argc == 4) {
        pthread_test();    
		goto out;
    }

    /* multi process */
    if (!strcmp("-process", argv[1])&& argc == 4) {
        process_test(); 
		goto  out;
    }

usage:
	printf ("Usage: %s [-pthread|-process|-read|-write] write_count read_count\nNote: macro:__JUST_READ_NOT_WRITE_/__JUST_WRITE_NOT_READ in Makefile.\nExample: ./posix -pthread 1 1\n\n",
			argv[0]);
out:
	if (fs) {
		server_fini(fs);
	}
    log_end();

    return 0;
}
