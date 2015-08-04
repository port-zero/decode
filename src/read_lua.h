#ifndef READ_LUA_H
#define READ_LUA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lua_util.h"
#include "util.h"

static inline char* print_lua_string(const char* prepend, char* stripped, unsigned long offset){
    unsigned long long c;
    unsigned long long i;

    stripped = copy(&c, stripped, lua_size_t);

    if(prepend) printf("%s %s ", COMMENT, prepend);
    for(i = offset; i < c; i++) putchar(*(stripped+i));
    if(!c) puts("(none)");
    else putchar('\n');
        
    stripped += c;
    
    return stripped;
}

static inline lua_code* get_lua_opcodes(lua_code* stripped){
    uint32_t ins;
    int p, o;
    register int i;
    unsigned char c;
    const int size = 100;
    char buffer[size];

    stripped->code = copy(&p, stripped->code, lua_int);

    if(!p) return stripped;

    lua_code_allocate(stripped, p);

    for(i = 0; i < p; i++){
        ins = 0;
        stripped->code = copy(&ins, stripped->code, lua_instruction);
        c = (unsigned char) RETRIEVE_LUA_OPCODE(ins);
        if(c > 37){
            fprintf(stderr, "Opcode %d does not exist. Corrupted?\n", c); 
            print_backtrace();
            continue;
        }
        o = snprintf(buffer, (size_t) size, "%-8s\t", LUA_OPCODE[c]);

        switch(LUA_OPCODE_FIELDS[c]){
            case A:
                o += snprintf(buffer+o, (size_t)(size-o), "%8d\t|\t%8s\t|\n", RETRIEVE_LUA_FIELD_A(ins), "");
                break;
            case SBX:
                o += snprintf(buffer+o, (size_t)(size-o), "%8s\t|\t%8d\n", "", RETRIEVE_LUA_FIELD_SBX(ins));
                break;
            case AB:
                o += snprintf(buffer+o, (size_t)(size-o), "%8d\t|\t%8d\t|\n", RETRIEVE_LUA_FIELD_A(ins),
                       RETRIEVE_LUA_FIELD_B(ins));
                break;
            case AC:
                o += snprintf(buffer+o, (size_t)(size-o), "%8d\t|\t%8s\t|\t%8d\n", RETRIEVE_LUA_FIELD_A(ins), "",
                       RETRIEVE_LUA_FIELD_C(ins));
                break;
            case ASBX:
                o += snprintf(buffer+o, (size_t)(size-o), "%8d\t|\t%8d\t\n", RETRIEVE_LUA_FIELD_A(ins),
                       RETRIEVE_LUA_FIELD_SBX(ins));
                break;
            case ABC:
                o += snprintf(buffer+o, (size_t)(size-o), "%8d\t|\t%8d\t|\t%8d\n", RETRIEVE_LUA_FIELD_A(ins),
                       RETRIEVE_LUA_FIELD_B(ins),
                       RETRIEVE_LUA_FIELD_C(ins));
                break;
            case ABX:
                o += snprintf(buffer+o, (size_t)(size-o), "%8d\t|\t%8d\n", RETRIEVE_LUA_FIELD_A(ins),
                       RETRIEVE_LUA_FIELD_BX(ins));
                break;
            default:
                fprintf(stderr, "Opcode argument encoding wrong: Corrupted?\n");
        }
        lua_code_add_decoded(stripped, buffer, o, i);
    }

    return stripped;
}

static inline lua_code* print_lua_constants(lua_code* stripped){
    int p;
    int64_t t;
    register unsigned int i;

    stripped->code = copy(&p, stripped->code, lua_int);

    printf("%s Constants: (Size %u)\n", COMMENT, p);
    printf("%s Line\tType\tValue\n", COMMENT);
    printf("%s -------------------\n", COMMENT);

    for(i = 0; i < p; i++){
        printf("%04u\t", i);
        switch(*(stripped->code)++){
            case 0:
                puts("NIL");
                break;
            case 1:
                printf("BOOL: %s\n", *(stripped->code)++ ? "TRUE" : "FALSE");
                break;
            case 3:
                printf("NUMBER: ");
                stripped->code = copy(&t, stripped->code, 8);
                printf("%lld\n", t);
                break;
            case 4:
                printf("STRING: ");
                stripped->code = print_lua_string(NULL, stripped->code, 0);
                break;
            default:
                printf("%i\t", (*(stripped->code--)));
                die("UNKNOWN CONSTANT TYPE: CORRUPTED?");
        }
    }

    return stripped;
}

static inline lua_code* get_lua_source_line_positions(lua_code* stripped){
    register int i;
    int c, p;

    stripped->code = copy(&c, stripped->code, lua_int);

    if(c == 0) return stripped;

    if(stripped->decoded_size != c) fprintf(stderr, 
                                            "INFO: Source Lines List (Size %d) do not map to instructions (Size %d).\n",
                                            c, 
                                            stripped->decoded_size);

    for(i = 0; i < c; i++){
        stripped->code = copy(&p, stripped->code, lua_int);

        stripped->lines[i] = p;
    }

    return stripped;
}

static inline lua_code* print_lua_locals(lua_code* stripped){
    register unsigned int i;
    unsigned int c, number;

    stripped->code = copy(&c, stripped->code, lua_int);

    if(c == 0) return stripped;

    printf("%s Constant list(size %u): \n", COMMENT, c);

    for(i = 0; i < c; i++){
        stripped->code = print_lua_string("Name:\t", stripped->code, 0);

        stripped->code = copy(&number, stripped->code, lua_int);

        printf("%s Start of scope: %u\t", COMMENT, number);

        stripped->code = copy(&number, stripped->code, lua_int);

        printf("End of scope: %u\n", number);
    }

    return stripped;
}

static inline lua_code* print_lua_upvalues(lua_code* stripped){
    register unsigned int i;
    unsigned int c;

    stripped->code = copy(&c, stripped->code, lua_int);

    if(c == 0) return stripped;

    printf("%s Upvalue list (size %u):\n", COMMENT, c);
    for(i = 0; i < c; i++) stripped->code = print_lua_string(NULL, stripped->code, 0);

    return stripped;
}

static inline char* lua_check(const char* file){
    size_t unused;
    char* content = NULL;
    size_t file_size = 0;
    FILE* lua_file = fopen(file, "r");

    if(!lua_file) die("Could not open file.");
    
    fseek(lua_file, 0, SEEK_END);
    file_size = (size_t) ftell(lua_file);
    fseek(lua_file, 0, SEEK_SET);

    if(!file_size || file_size < LUA_HEADER_SIZE) die("File is empty/not big enough to fit lua header.");

    content = (char*) malloc(file_size + 1);

    if(!content) die("Could not allocate enough memory.");

    unused = fread(content, file_size, 1, lua_file);
    if(unused != 1) fprintf(stderr, "Reading the file in may have failed.\n");
    fclose(lua_file);
    content[file_size] = 0;

    if(starts_with(content, LUA_SIG) == 0) die("Lua header not correct. Signature verification failed.");

    if(content[4] != LUA_VERSION) die("This disassembler only works with Lua 5.1.");

    lua_int = content[7];
    lua_size_t = content[8];
    lua_instruction = content[9];

    printf("%s Disassembly of file: %s\n", COMMENT, file);
    printf("%s Endianness: %s endian\n", COMMENT, content[6] == 0 ? "big" : "little");
    printf("%s Int size: %i\n", COMMENT, lua_int);
    printf("%s Size_t size: %i\n", COMMENT, lua_size_t);
    printf("%s Instruction size: %i\n", COMMENT, lua_instruction);
    printf("%s lua_Number size: %i\n", COMMENT, content[10]);

    if(sizeof(size_t) < lua_size_t) puts("The lua binary size_t datatype is bigger than the size_t of this machine. This might lead to strange behaviour in the source line positions section. Please report any issues on Github(https://github.com/hellerve/decode/issues) or to veit@veitheller.de");
    if(sizeof(unsigned int) < lua_int) puts("The lua binary int datatype is bigger than the int of this machine. This might lead to strange behaviour in the source line positions section. Please report any issues on Github(https://github.com/hellerve/decode/issues) or to veit@veitheller.de");

    putchar('\n');
    
    return content;
}

static inline lua_code* print_lua_general_info(lua_code* stripped){
    unsigned int p;

    stripped->code = copy(&p, stripped->code, lua_int);
    printf("%s Line defined: %u\n", COMMENT, p);

    stripped->code = copy(&p, stripped->code, lua_int);
    printf("%s Last Line defined: %u\n", COMMENT, p);

    printf("%s Number of Upvalues: %i\n", COMMENT, *(stripped->code)++);
    printf("%s Number of Parameters: %i\n", COMMENT, *(stripped->code)++);
    printf("%s Vararg flag: %i\n", COMMENT, *(stripped->code)++);
    printf("%s Maximum stack size: %i\n", COMMENT, *(stripped->code)++);

    return stripped;
}

static inline lua_code* print_lua_function_prototypes(lua_code* stripped);

static inline lua_code* print_lua_function(lua_code* stripped){
    stripped->code = print_lua_string("Source file name:", stripped->code, 0);

    stripped = print_lua_general_info(stripped);
    putchar('\n');
    stripped = get_lua_opcodes(stripped);
    stripped = print_lua_constants(stripped);
    putchar('\n');
    stripped = print_lua_function_prototypes(stripped);
    putchar('\n');
    
    if(stripped) stripped = get_lua_source_line_positions(stripped);
    
    lua_code_print(stripped);
    
    if(stripped){
        putchar('\n');
        stripped = print_lua_locals(stripped);
        putchar('\n');
        stripped = print_lua_upvalues(stripped);
    }

    return stripped;
}

static inline lua_code* print_lua_function_prototypes(lua_code* stripped){
    unsigned int c;
    register unsigned int i;

    stripped->code = copy(&c, stripped->code, lua_int);

    printf("%s Function prototypes list(Size %u): {\n", COMMENT, c);

    for(i = 0; i < c; i++) stripped = print_lua_function(stripped);

    printf("%s }\n", COMMENT);

    return stripped;
}

void lua(char* file){
    char* file_contents = lua_check(file);
    lua_code* stripped = lua_code_new(file_contents+LUA_HEADER_SIZE);

    stripped = print_lua_function(stripped);

    puts("; End of binary information.");

    free((char*)file_contents);
    lua_code_delete(stripped);
}


#endif
