#ifndef LUA_UTIL_H
#define LUA_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

#define LUA "^.*\\.luac$"
#define LUA_SIG "\x1B\x4C\x75\x61" //  = \27Lua
#define LUA_VERSION 0x51
#define LUA_HEADER_SIZE 12

#define RETRIEVE_LUA_OPCODE(N) (N & 0x3F)
#define RETRIEVE_LUA_FIELD_A(N) (N & 0x3FC0)
#define RETRIEVE_LUA_FIELD_B(N) (N & 0xFF800000)
#define RETRIEVE_LUA_FIELD_BX(N) (N & 0xFFFFC000)
#define RETRIEVE_LUA_FIELD_SBX(N) (signed) RETRIEVE_LUA_FIELD_BX(N)
#define RETRIEVE_LUA_FIELD_C(N) (N & 0x7FC000)

enum LUA_FIELD_USAGE{
    AB,
    ABX,
    ABC,
    SBX,
    AC,
    ASBX,
    A
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

static const char LUA_OPCODE_FIELDS[] = { AB, ABX, ABC, AB, AB, ABX, ABC, ABX, AB, ABC,
                                          ABC, ABC, ABC, ABC, ABC, ABC, ABC, ABC, AB, AB,
                                          AB, ABC, SBX, ABC, ABC, ABC, AC, ABC, ABC, ABC, 
                                          AB, ASBX, ASBX, AC, ABC, A, ABX, AB };

static char lua_int = 0;
static char lua_size_t = 0;
static char lua_instruction = 0;

typedef struct lua_code {
    char* code;
    int decoded_size;
    char** decoded;
    int* lines;
} lua_code;

static inline lua_code* lua_code_new(char* code){
    lua_code* lua_c = (lua_code*) malloc(sizeof(lua_code));

    lua_c->code = code;
    lua_c->decoded_size = -1;

    return lua_c;
}

static inline char lua_code_allocated(lua_code* code){
    return code->decoded_size != -1;
}

static inline void lua_code_delete(lua_code* code){
    if(lua_code_allocated(code)){
        free(code->decoded);
        free(code->lines);
    }

    free(code);
}

static inline void lua_code_allocate(lua_code* code, int size){
    code->decoded = (char**) malloc((long unsigned int)size * sizeof(char*));
    code->lines = (int*) calloc((long unsigned int)size * sizeof(int), 1);
    code->decoded_size = size;
}

static inline void lua_code_add_decoded(lua_code* code, char* decoded, int size, int position){
    code->decoded[position] = strndup(decoded, (size_t) size);
}

static inline void lua_code_print(lua_code* code){
    int i;

    if(!code->code || !lua_code_allocated(code) || !code->decoded || !code->lines) 
        fprintf(stderr, "Tried to print empty code object.");

    printf("%s Opcodes (Size %d): \n", COMMENT, code->decoded_size);
    printf("%s Line\tOpcode\t\tA\tB\tC\n", COMMENT);
    printf("%s ---------------------------------------\n", COMMENT);
    
    for(i = 0; i < code->decoded_size; i++)
        printf("  %04d\t%s", code->lines[i], code->decoded[i]);
}

#endif
