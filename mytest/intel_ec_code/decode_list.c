#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define alloca0(size) ({void *__ptr; __ptr = alloca(size); memset(__ptr, 0, size); __ptr; })
#define char_show(error_count, src_in_err, max_count) ({int i; printf ("%d==>", error_count); for (i = 0; i < max_count; i++) {printf ("%c", src_in_err[i]);} printf ("\n");})
#define char_init(src_in_err, max_count) ({int i; for (i = 0; i < max_count; i++) {src_in_err[i] = '0';}})

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

typedef struct {
    struct list_head list;
    char *src_in_err;
    char *src_err_list;
    int nerrs;
    int nsrcerrs;
    unsigned char **decode_matrix;
    unsigned char **decode_tbls;
    int m;
    int k;
} ec_decode_matrix_t;

static long long ec_method_combination (const int m, const int k)
{
    long long ret = -EINVAL;
    unsigned long long redundancy = 0;
    unsigned long long sum = 0;
    unsigned long long i = 0;
    unsigned long long j = 0;
    unsigned long long n1 = 1;
    unsigned long long m1 = 1;
    unsigned long long n1_m1 = 1;

    if (m <=0 || k <=0) {
        ret = -EINVAL;
        printf ("%s", strerror(EINVAL));
        goto out;
    }

    redundancy = m - k;

    for (j = m; j >= 1; j--) {
        n1 *= j;
    }

    for (i = 1; i <= redundancy; i++) {
        m1 = 1;
        n1_m1 = 1;

        for (j = i; j >= 1; j--) {
            m1 *= j;
        }

        for (j = m - i; j >= 1; j--) {
            n1_m1 *= j;
        }

        sum += n1 / (m1 * n1_m1);
    }
    printf ("DEBUG: sum=%d\n", sum);
    ret = sum;
out:
    return ret;
}

static int ec_method_decode_matrix_ftw (int k, int max, int *count,
                                        int error_count, unsigned char *src_in_err)
{
    int i, j;

    for (i = k + 1; i < max; i++) {
        (*count)++;
        src_in_err[i] = '1';
        if ((*count) == error_count) {
            char_show(*count, src_in_err, max);
            (*count)--;
            src_in_err[i] = '0';
            continue;
        } else {
            ec_method_decode_matrix_ftw (i, max, count, error_count, src_in_err);
            src_in_err[i] = '0';
            (*count)--;
        }
    }
    return 0;
}

static int ec_method_decode_matrix_init (const int m, const int k)
{
    int err = -ENOMEM;
    int i = 0;
    int j = 0;
    int n = 0;
    unsigned char *decode_matrix = NULL;
    unsigned char *invert_matrix = NULL;
    unsigned char *decode_tbls = NULL;
    unsigned char *src_in_err = NULL;
    unsigned char *src_err_list = NULL;
    int nerrs = 0;
    int nsrcerrs = 0;
    unsigned long long decode_matrix_count = 0;
    int redundancy = 0;
    int error_count = 0;
    int tmp_count = 0;
    int *tmp_index = NULL;

    if (m <= 0 || k <= 0) {
        err = -EINVAL;
        printf ("%s", strerror(EINVAL));
        goto out;
    }

    decode_matrix_count = ec_method_combination (m, k);
    if (decode_matrix_count < 0) {
        err = decode_matrix_count;
        printf ("%s\n", strerror(-decode_matrix_count));
        goto out;
    }

    src_in_err = alloca0 (m);
    char_init(src_in_err, m);

    redundancy = m - k;
    error_count = 1;
    tmp_index = calloc (redundancy, sizeof(*tmp_index));
    if (tmp_index == NULL) {
        err = -errno;
        printf ("%s\n", strerror(-err));
        goto out;
    }

    while (error_count <= redundancy) {
        printf ("error_count=%d, redundancy=%d\n", error_count, redundancy);

        for (n = 0; n < m; n++) {
            tmp_count = 0;
            char_init(src_in_err, m);
            tmp_count++;
            src_in_err[n] = '1';
            if (tmp_count != error_count) {
                ec_method_decode_matrix_ftw(n, m, &tmp_count, error_count, src_in_err);
                src_in_err[n] = '0';
                tmp_count--;
            } else {
                char_show (error_count, src_in_err, m);
            }
        }

        error_count++;
    }

#if 0
        for (i = 0; i < m; i++) {
            tmp_count++;
            if (tmp_count == error_count) {
                /* allocate src_in_err */
                /* allocate src_err_list */
                tmp_count = 0;
            }
            for (j = i + 1; j < m; j++)
        }

        error_count++;
    }

    for (i = 0; i < m; i++) {
        nerrs = 0;
        nsrcerrs = 0;
        /* allocate memory */
        src_in_err = calloc (m+1, sizeof(*src_in_err));
        src_err_list = calloc (m+1, sizeof(*src_err_list));
        if (src_in_err == NULL || src_err_list == NULL) {
            err = -ENOMEM;
            printf ("Failed to allocate memory for "
                    "src_in_err or src_err_list(%s)",
                    strerror(errno));
            goto out;
        }
        /* init 0 */
        for (n = 0; n < m; n++) {
            src_in_err[n] = '0';
            src_err_list[n] = '0';
        }
        /* allocate value */
        for (j = i; j < i+(m-k); j++) {
            src_in_err[j] = '1';
            src_err_list[nerrs++] = j;
            if (j < k) {
                nsrcerrs++;
            }
        }
        /* print results */
        src_in_err[m] = '\0';
        src_err_list[m] = '\0';
        printf ("src_in_err = ");
        for (j = 0; j < m; j++) {
            printf ("%c", src_in_err[j]);
            printf ("src_in_err=%s\nsrc_err_list=%s\n"
                    "nerrs=%d\nnsrcerrs=%d\n\n",
                    src_in_err, src_err_list,
                    nerrs, nsrcerrs);
        }
        printf ("\n");
    }

	decode_matrix = GF_CALLOC (MMAX * KMAX, sizeof(*decode_matrix),
                               gf_common_mt_char);
	invert_matrix = GF_CALLOC (MMAX * KMAX, sizeof(*invert_matrix),
                               gf_common_mt_char);
	decode_tbls   = GF_CALLOC (MMAX * KMAX * 32, sizeof(*decode_tbls),
                               gf_common_mt_char);
    if (!decode_matrix || !invert_matrix || !decode_tbls) {
        err = -ENOMEM;
        gf_msg ("INTEL_EC_METHOD", GF_LOG_ERROR, ENOMEM,
                EC_MSG_NO_MEMORY, "Failed to allocate "
                "memory for decode_matrix, invert_matrix "
                "and decode_tbls!");
        goto out;
    }

	/* Generate decode matrix */
	err = gf_gen_decode_matrix(encode_matrix, decode_matrix,
				               invert_matrix, decode_index,
                               src_err_list, src_in_err,
				               nerrs, nsrcerrs, k, m);
	if (err != 0) {
        err = -EIO;
        gf_msg ("INTEL_EC_METHOD", GF_LOG_ERROR, EIO,
                EC_MSG_INVALID_REQUEST, "Failed to "
                "gf_gen_decode_matrix()");
        goto out;
	}
#endif

out:
    return err;
}

int main(void)
{
    /*ec_method_combination (3, 2);*/
    ec_method_decode_matrix_init (5, 3);
}
