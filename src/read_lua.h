#ifndef READ_LUA_H
#define READ_LUA_H

#include <stdio.h>
#include <stdlib.h>

#include "lua_util.h"
#include "util.h"

static inline char* print_lua_string(const char* prepend, char* stripped, unsigned long offset){
    unsigned long long c;
    unsigned long long i;

    stripped = copy(&c, stripped, lua_size_t);

    printf("%s %s ", COMMENT, prepend);
    for(i = offset; i < c; i++) putchar(*(stripped+i));
    putchar('\n');
        
    stripped += c;
    
    return stripped;
}

static inline char* print_lua_opcodes(char* stripped){
    uint32_t ins;
    long p;
    unsigned long i;

    printf("%s Opcodes: \n", COMMENT);

    stripped = copy(&p, stripped, lua_int);

    for(i = 0; i < p;){
        stripped = copy(&ins, stripped, lua_instruction);
        printf("%04lu\t%s\n", i++, 
               LUA_OPCODE[(unsigned char) (RETRIEVE_LUA_OPCODE(ins))]);/*
               RETRIEVE_LUA_FIELD_A(ins),
               RETRIEVE_LUA_FIELD_Bx(ins));*/
    }

    return stripped;
}

static inline char* print_lua_constants(char* stripped){
    long p;
    unsigned long i;

    stripped = copy(&p, stripped, lua_int);

    printf("\n%s Constants: (Size %ld)\n", COMMENT, p);

    for(i = 0; i < p; i++){
        printf("%04lu ", i);
        switch(*(stripped)++){
            case 0:
                puts("NIL");
                break;
            case 1:
                printf("BOOL: %s\n", *(stripped)++ ? "TRUE" : "FALSE");
                break;
            case 3:
                printf("NUMBER: \n");
            case 4:
                stripped = print_lua_string("STRING:", stripped, 0);
                break;
            default:
                putchar(*(stripped));
                die("UNKNOWN CONSTANT TYPE: CORRUPTED?");
        }
    }

    return stripped;
}

static inline char* lua_check(const char* file){
    char* content = NULL;
    size_t file_size = 0;
    FILE* lua_file = fopen(file, "r");

    puts("File has 'luac' ending, assuming lua bytecode.\n");

    if(!lua_file)
        die("Could not open file.");
    
    fseek(lua_file, 0, SEEK_END);
    file_size = (size_t) ftell(lua_file);
    fseek(lua_file, 0, SEEK_SET);

    if(!file_size || file_size < 12)
        die("File is empty/not big enough to fit lua header.");

    content = (char*) malloc(file_size + 1);

    if(!content)
        die("Could not allocate enough memory.");

    fread(content, file_size, 1, lua_file);
    fclose(lua_file);
    content[file_size] = 0;

    if(starts_with(content, LUA_SIG) == 0)
        die("Lua header not right. Signature verification failed.");

    if(content[4] != LUA_VERSION)
        die("This disassembler only works with Lua 5.1.");

    lua_int = content[7];
    lua_size_t = content[8];
    lua_instruction = content[9];

    printf("%s Disassembly of file: %s\n", COMMENT, file);
    printf("%s Endianness: %s endian\n", COMMENT, content[6] == 0 ? "big" : "little");
    printf("%s Int size: %i\n", COMMENT, lua_int);
    printf("%s Size_t size: %i\n", COMMENT, lua_size_t);
    printf("%s Instruction size: %i\n", COMMENT, lua_instruction);
    printf("%s lua_Number size: %i\n", COMMENT, content[10]);

    if(sizeof(unsigned long long) < lua_size_t) die("The lua binary size_t datatype is too big for this machine.");
    if(sizeof(long) < lua_int) die("The lua binary int datatype is too big for this machine.");
    
    return content;
}

static inline char* print_lua_general_info(char* stripped){
    long p;

    stripped = print_lua_string("Source file name:", stripped, 1);

    stripped = copy(&p, stripped, lua_int);
    printf("%s Line defined: %ld\n", COMMENT, p);

    stripped = copy(&p, stripped, lua_int);
    printf("%s Last Line defined: %ld\n", COMMENT, p);

    printf("%s Number of Upvalues: %i\n", COMMENT, *(stripped)++);
    printf("%s Number of Downvalues: %i\n", COMMENT, *(stripped)++);
    printf("%s Vararg flag: %i\n", COMMENT, *(stripped)++);
    printf("%s Maximum stack size: %i\n", COMMENT, *(stripped)++);

    return stripped;
}

void lua(char* file){
    char* file_contents = lua_check(file);
    char* stripped = file_contents+12;

    stripped = print_lua_general_info(stripped);

    putchar('\n');

    stripped = print_lua_opcodes(stripped);

    stripped = print_lua_constants(stripped);

    free((char*)file_contents);
}


#endif
