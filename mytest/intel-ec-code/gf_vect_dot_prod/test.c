#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// for memset, memcmp
#include <errno.h>
#include "erasure_code.h"
#include "types.h"

#ifndef FUNCTION_UNDER_TEST
# define FUNCTION_UNDER_TEST gf_vect_dot_prod
#endif
#ifndef TEST_MIN_SIZE
# define TEST_MIN_SIZE  32
#endif

#define str(s) #s
#define xstr(s) str(s)

#define TEST_LEN 66

#ifndef TEST_SOURCES
# define TEST_SOURCES  16
#endif
#ifndef RANDOMS
# define RANDOMS 20
#endif

#define MMAX TEST_SOURCES
#define KMAX TEST_SOURCES

#ifdef EC_ALIGNED_ADDR
// Define power of 2 range to check ptr, len alignment
# define PTR_ALIGN_CHK_B 0
# define LEN_ALIGN_CHK_B 0	// 0 for aligned only
#else
// Define power of 2 range to check ptr, len alignment
# define PTR_ALIGN_CHK_B 32
# define LEN_ALIGN_CHK_B 32	// 0 for aligned only
#endif

typedef unsigned char u8;

void dump(unsigned char *buf, int len)
{
	int i;
	for (i = 0; i < len;) {
		printf(" %2x", 0xff & buf[i++]);
		if (i % 32 == 0)
			printf("\n");
	}
	printf("\n");
}

void dump_matrix(unsigned char **s, int k, int m)
{
	int i, j;
	for (i = 0; i < k; i++) {
		for (j = 0; j < m; j++) {
			printf(" %2x", s[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void dump_u8xu8(unsigned char *s, int k, int m)
{
	int i, j;
	for (i = 0; i < k; i++) {
		for (j = 0; j < m; j++) {
			printf(" %2x", 0xff & s[j + (i * m)]);
		}
		printf("\n");
	}
	printf("\n");
}

void test_of_all_zeros(void)
{
    printf ("===BEGIN: %s===\n", __FUNCTION__);
    int i; 
    int err = -1;
    void *buf = NULL;
    u8 g_tbls[TEST_SOURCES * 32];
    /*u8 src_in_err[TEST_SOURCES];*/
    /*u8 src_err_list[TEST_SOURCES];*/
    u8 *buffs[TEST_SOURCES];
    u8 *dest = NULL;
    u8 *dest_ref = NULL;
    u8 *temp_buff = NULL;
    u8 g[TEST_SOURCES];

    for (i = 0; i < TEST_SOURCES; i++) {
        err = posix_memalign (&buf, 64, TEST_LEN + 1);
        if (err) {
            perror("posix_memalign()");
            goto out;
        }
        buffs[i] = buf;
    }

    err = posix_memalign (&buf, 64, TEST_LEN + 1);
    if (err) {
        perror ("posix_memalign()");
        goto out;
    }
    dest = buf;

    err = posix_memalign (&buf, 64, TEST_LEN + 1);
    if (err) {
        perror ("posix_memalign()");
        goto out;
    }
    dest_ref = buf;

    err = posix_memalign (&buf, 64, TEST_LEN + 1);
    if (err) {
        perror ("posix_memalign()");
        goto out;
    }
    temp_buff = buf;

    for (i = 0; i < TEST_SOURCES; i++) {
        memset (buffs[i], 0, TEST_LEN + 1);
    }

    memset (dest, 0, TEST_LEN + 1);
    memset (dest_ref, 0, TEST_LEN + 1);
    memset (temp_buff, 0, TEST_LEN + 1);
    memset (g, 0, TEST_SOURCES);

    for (i = 0; i < TEST_SOURCES; i++) {
        gf_vect_mul_init (g[i], &g_tbls[i * 32]);
    }

    gf_vect_dot_prod_base (TEST_LEN, TEST_SOURCES, &g_tbls[0], buffs, dest_ref);

    gf_vect_dot_prod (TEST_LEN, TEST_SOURCES, g_tbls, buffs, dest);

    err = memcmp (dest_ref, dest, TEST_LEN);
    if (err) {
        printf ("Fail zero\n");
        dump_matrix (buffs, 5, TEST_SOURCES);
        printf ("dprod_base:");
        dump(dest_ref, 25);
        printf ("dprod:");
        dump(dest, 25);
        goto out;
    }

    for (i = 0; i < TEST_SOURCES; i++) {
        printf ("%s\n", buffs[i]);
    }

    printf ("dest:%s\n", dest);
    printf ("dest_ref:%s\n", dest_ref);

out:
    for (i = 0; i < TEST_SOURCES; i++) {
        if (buffs[i])
            free (buffs[i]);
    }
    if (dest)
        free (dest);
    if (dest_ref)
        free (dest_ref);
    if (temp_buff)
        free (temp_buff);

    printf ("===END: %s===\n\n", __FUNCTION__);
    return;
}

void rand_data_test_with_varied_parameters(void)
{
    printf ("===BEGIN: %s===\n", __FUNCTION__);
    int i; 
    int j; 
    int rtest;
    int srcs;
    int err = -1;
    void *buf = NULL;
    u8 g_tbls[TEST_SOURCES * 32];
    u8 *buffs[TEST_SOURCES];
    u8 *dest = NULL;
    u8 *dest_ref = NULL;
    u8 *temp_buff = NULL;
    u8 g[TEST_SOURCES];

    for (i = 0; i < TEST_SOURCES; i++) {
        err = posix_memalign (&buf, 64, TEST_LEN + 1);
        if (err) {
            perror("posix_memalign()");
            goto out;
        }
        buffs[i] = buf;
    }

    err = posix_memalign (&buf, 64, TEST_LEN + 1);
    if (err) {
        perror ("posix_memalign()");
        goto out;
    }
    dest = buf;

    err = posix_memalign (&buf, 64, TEST_LEN + 1);
    if (err) {
        perror ("posix_memalign()");
        goto out;
    }
    dest_ref = buf;

    err = posix_memalign (&buf, 64, TEST_LEN + 1);
    if (err) {
        perror ("posix_memalign()");
        goto out;
    }
    temp_buff = buf;

    for (i = 0; i < TEST_SOURCES; i++) {
        memset (buffs[i], 0, TEST_LEN + 1);
    }

    memset (dest, 0, TEST_LEN + 1);
    memset (dest_ref, 0, TEST_LEN + 1);
    memset (temp_buff, 0, TEST_LEN + 1);
    memset (g, 0, TEST_SOURCES);

    for (rtest = 0; rtest < RANDOMS; rtest++) {
        for (srcs = TEST_SOURCES; srcs > 0; srcs--) {
            for (i = 0; i < srcs; i++) {
                for (j = 0; j < TEST_LEN; j++) {
                    buffs[i][j] = rand();
                }
            }

            for (i = 0; i < srcs; i++) {
                g[i] = rand();
            }
                
            for (i = 0; i < TEST_SOURCES; i++) {
                gf_vect_mul_init (g[i], &g_tbls[i * 32]);
            }

            gf_vect_dot_prod_base (TEST_LEN, TEST_SOURCES, &g_tbls[0], buffs, dest_ref);
            gf_vect_dot_prod (TEST_LEN, TEST_SOURCES, g_tbls, buffs, dest);

            err = memcmp (dest_ref, dest, TEST_LEN);
            if (err) {
                printf ("Fail zero\n");
                dump_matrix (buffs, 5, TEST_SOURCES);
                printf ("dprod_base:\n");
                dump(dest_ref, 25);
                printf ("dprod:\n");
                dump(dest, 25);
                goto out;
            }

            for (i = 0; i < TEST_SOURCES; i++) {
                /*printf ("%s\n", buffs[i]);*/
            }

            /*printf ("dest:%s\n", dest);*/
            /*printf ("dest_ref:%s\n", dest_ref);*/
            printf ("success\n");
        }
    }



out:
    for (i = 0; i < TEST_SOURCES; i++) {
        if (buffs[i])
            free (buffs[i]);
    }
    if (dest)
        free (dest);
    if (dest_ref)
        free (dest_ref);
    if (temp_buff)
        free (temp_buff);

    printf ("===END: %s===\n\n", __FUNCTION__);
    return;
}

void test_ec_using_gf_vect_dot_prod (int m, int k)
{
    printf ("===BEGIN: %s===\n", __FUNCTION__);
    int i; 
    int j;
    int r;
    int err = -1;
    int nerrs = 0;
    void *buf = NULL;
    u8 g_tbls[TEST_SOURCES * 32];
    u8 src_in_err[TEST_SOURCES];
    u8 src_err_list[TEST_SOURCES];
    u8 *buffs[TEST_SOURCES];
    u8 *temp_buff = NULL;
    u8 a[MMAX * KMAX];
    u8 b[MMAX * KMAX];
    u8 d[MMAX * KMAX];
    u8 *recov[TEST_SOURCES];

    if (m > MMAX || k > KMAX) {
        printf ("args %s\n", strerror(EINVAL));
        goto out;
    }

    /* 1. 分配内存 */
    for (i = 0; i < m; i++) {
        err = posix_memalign (&buf, 64, TEST_LEN+1);
        if (err) {
            perror("posix_memalign()");
            goto out;
        }
        buffs[i] = buf;
    }

    err = posix_memalign (&buf, 64, TEST_LEN+1);
    if (err) {
        perror ("posix_memalign()");
        goto out;
    }
    temp_buff = buf;

    /* 2. 预先赋值 */
    for (i = 0; i < k; i++) {
        for (j = 0; j < TEST_LEN; j++) {
            buffs[i][j] = '0' + i;
        }
    }

    for (r = 0; r < m; r++) {
        printf ("buffs[%d]=\t%s\n", r, buffs[r]);
    }
    printf ("======================================================\n\n\n");

    /* 3. 生成encode矩阵 */
    gf_gen_rs_matrix (a, m, k);

    /* 4. 编码并生成冗余数据 */
    for (i = k; i < m; i++) {
        for (j = 0; j < k; j++)
            gf_vect_mul_init (a[k * i + j], &g_tbls[j * 32]);

        gf_vect_dot_prod (TEST_LEN, k, g_tbls, buffs, buffs[i]);
    }

    /* 5. 随机生成错误 */
    memset (src_in_err, 0, TEST_SOURCES);
    for (i = 0, nerrs = 0; i < k && nerrs < m - k; i++) {
        err = 1 & rand();
        src_in_err[i] = err;
        if (err)
            src_err_list[nerrs++] = i;
    }

    /* 6. 根据encode矩阵生成invert矩阵 */
    for (i = 0, r = 0; i < k; i++, r++) {
        while (src_in_err[r]) {
            r++;
            continue;
        }
        for (j = 0; j < k; j++) {
            b[k * i + j] = a[k * r + j];
        }
    }

    /* 7. 根据invert矩阵生成decode矩阵 */
    err = gf_invert_matrix ((u8 *)b, (u8 *)d, k);
    if (err < 0) {
        printf ("BAD MATRIX\n");
        goto out;
    }

    /* 8. 将未损坏的数据块放在紧凑的数组中 */
    for (i = 0, r = 0; i < k; i++, r++) {
        while (src_in_err[r]) {
            r++;
            continue;
        }
        recov[i] = buffs[r];
    }

    for (i = 0; i < nerrs; i++) {
        for (j = 0; j < k; j++)
            /* 9. 根据decode矩阵初始化g_tbls矩阵 */
            gf_vect_mul_init (d[k * src_err_list[i] + j], &g_tbls[j * 32]);
        
        /* 10. 根据g_tbls矩阵还原数据 */
        gf_vect_dot_prod (TEST_LEN, k, g_tbls, recov, temp_buff);

        /* 11. 比较还原回来的数据是否和原来的数据一样 */
        err = memcmp (temp_buff, buffs[src_err_list[i]], TEST_LEN);
        if (err != 0) {
            printf ("Fail error recovery (%d, %d, %d)\n", m, k, nerrs);
            printf ("recov %d:", src_err_list[i]);
            dump (temp_buff, 25);
            printf ("orig:");
            dump(buffs[src_err_list[i]], 25);
            goto out;
        }

        /* 12. 打印出来还原的数据和原来的数据块 */
        printf ("Success\n");
        for (r = 0; r < m; r++) {
            /*printf ("buffs[%d]=\t%s\n", r, buffs[r]);*/
        }
        printf ("temp_buff=\t%s\n", temp_buff);
        printf ("buffs[%d]=\t%s\n\n", src_err_list[i], buffs[src_err_list[i]]);
    }

out:
    /* 13. 释放内存 */
    for (i = 0; i < m; i++) {
        if (buffs[i])
            free (buffs[i]);
    }
    if (temp_buff)
        free (temp_buff);

    printf ("===END: %s===\n\n", __FUNCTION__);
    return;
}

int main(int argc, char *argv[])
{
    /*test_of_all_zeros ();*/
    /*rand_data_test_with_varied_parameters();*/
    test_ec_using_gf_vect_dot_prod(9, 5);

    return 0;
}
