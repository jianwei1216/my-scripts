#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* 函数原型声明，至少需要一个确定的参数，注意括号内的省略号*/ 
int demo(char* , ...); 

void main(void) 
{ 
   /*demo("DEMO", "This", "is", "a", "demo!", "");*/
   demo("DEMO", "hello");
} 

int demo(char *msg, ...) 
{ 
    va_list argp; 
    int argno = 0; 
    char* para; 

    printf ("First parameter is %s\n", msg);

    va_start(argp, msg); 
    para = va_arg(argp, char*);
    printf ("%s\n", para);
#if 0
    while (1) 
    { 
        para = va_arg(argp, char*); 
        if ( strcmp( para, "" ) == 0  ) 
            break; 
        printf("Parameter #%d is: %s\n", argno, para); 
        argno++; 
    }
#endif
    va_end( argp  ); 

    return 0; 
}
