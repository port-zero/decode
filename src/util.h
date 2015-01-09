#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VERSION "0.1"
#define COMMENT ";"

static inline void print_usage(int returncode){
    printf("decode, the interpreter disassembler\nVersion %s\n\nUsage: decode <obj_file>", VERSION);
    exit(returncode);
}

static inline void die(const char* message){
    fprintf(stderr, "%s\n", message);
    exit(1);
}

static inline int starts_with(const char* a, const char* b){
    if(strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}

static inline char* copy(void* dest, char* src, char s){
    if(!memcpy((char*)dest, src, s))
        die("Could not copy memory");

    return src + s;
}

#endif
