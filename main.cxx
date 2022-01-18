#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: %s <fun file name>\n",argv[0]);
        exit(-1);
    }

    printf("    .data\n");
    printf("fmt:\n");
    printf("    .string \"%%d\\n\"\n");
    printf("    .align 8\n");
    printf("argc:\n");
    printf("    .quad 0\n");
    printf("    .text\n");
    printf("    .global main\n");
    printf("main:\n");
    printf("    mov x0,#0 // indicate success\n");
    printf("    ret // we're out of here\n");
    return 0;
}
