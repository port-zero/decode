#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VERSION "0.1"
#define LUA ".*\\.luac"
#define LUA_SIG "\x1BLua"
#define LUA_VERSION 0x51
#define LUA_RETURN 30
#define PYTHON "*.pyc"
#define COMMENT ";"

#define RETRIEVE_LUA_OPCODE(N) N & 0x3F
#define RETRIEVE_LUA_FIELD_A(N) N >> 9
#define RETRIEVE_LUA_FIELD_B(N) (N >> 18) & 0x1FF
#define RETRIEVE_LUA_FIELD_Bx(N) (N >> 18)
#define RETRIEVE_LUA_FIELD_sBx(N) (N >> 18)
#define RETRIEVE_LUA_FIELD_C(N) (N >> 26) & 0xFF

union lua_int_type {
    int small;
    long big;
};

union lua_instruction_type {
    int small;
    long big;
};

union lua_size_t_type {
    size_t small;
    unsigned long long big;
};

static const char* const LUA_OPCODE[] = { "MOVE", "LOADK", "LOADBOOL", "LOADNIL",
                                          "GETUPVAL", "GETGLOBAL", "GETTABLE",
                                          "SETGLOBAL", "SETUPVAL", "SETTABLE",
                                          "NEWTABLE", "SELF", "ADD", "SUB", "MUL", 
                                          "DIV", "MOD", "POW", "UNM", "NOT", "LEN", 
                                          "CONCAT", "JMP", "EQ", "LT", "LE", "TEST", 
                                          "TESTSET", "CALL", "TAILCALL", "RETURN", 
                                          "FORLOOP", "FORPREP", "TFORLOOP", "SETLIST", 
                                          "CLOSE", "CLOSURE", "VARARG" };


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
    if(!memcpy(dest, src, s))
        die("Could not copy memory");

    return src + s;
}

#endif
