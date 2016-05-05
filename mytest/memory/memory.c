#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

struct test {
    char a;
    int b;
};

void test_memory_alloc()
{
    struct test *ptr = NULL;     
    struct test *ptr2 = NULL;

    ptr = (struct test*)malloc(sizeof(struct test));
    ptr->a = 'A';
    ptr->b = 8;
    printf ("ptr =%p: %c, %d\n", ptr, ptr->a, ptr->b);
    free (ptr);

    ptr2 = (struct test*)malloc(sizeof(struct test));
    printf ("ptr =%p: %c, %d\n", ptr, ptr->a, ptr->b);
    printf ("ptr2=%p\n", ptr2);
    free(ptr2);
}

int main(void)
{
    test_memory_alloc();

    return 0;
}
