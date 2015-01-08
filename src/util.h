#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VERSION "0.1"
#define LUA ".*\\.luac"
#define LUA_SIG "\27Lua"
#define PYTHON "*.pyc"
#define LUA_VERSION 0x52
#define COMMENT ";"

static const char* const LUA_OPCODE[] = { "MOVE", "LOADK", "LOADKX", "LOADBOOL", "LOADNIL",
                                          "GETUPVAL", "GETTABUP", "GETTABLE", "SETTABUP",
                                          "SETUPVAL", "SETTABLE", "NEWTABLE", "SELF",
                                          "ADD", "SUB", "MUL", "DIV", "MOD", "POW",
                                          "UNM", "NOT", "LEN", "CONCAT", "JMP",
                                          "EQ", "LT", "LE", "TEST", "TESTSET",
                                          "CALL", "TAILCALL", "RETURN", "FORLOOP",
                                          "FORPREP", "TFORCALL", "TFORLOOP", "SETLIST",
                                          "CLOSURE", "VARARG", "EXTRAARG" };

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

#endif
