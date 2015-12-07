#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <execinfo.h>

#define VERSION "0.1"
#define COMMENT ";"

static inline void print_usage(int returncode){
    printf("decode, the interpreter disassembler\nVersion %s\n\nUsage: decode <obj_file>\n", VERSION);
    exit(returncode);
}

static inline void print_version(int returncode){
    printf("decode, the interpreter disassembler\nVersion %s\n", VERSION);
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
    if(!memcpy(dest, src, (size_t)s))
        die("Could not copy memory");

    return src + s;
}

static inline void print_backtrace() {
    void* trace[10];
    int c, i;
    char** names;

    c = backtrace(trace, 10);
    names = backtrace_symbols(trace, c);

    for(i = 0; i < c; i++) printf("; %s\n", names[i]);
}

#endif
