#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#include "util.h"

static char lua_int = 0;
static char lua_size_t = 0;
static char lua_instruction = 0;
static char big = 0;

static inline char * parse_args(int argc, char** argv){
    int i;

    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], "-h") == 0)
            print_usage(0);
        else
            return argv[i];
    }

    die("No input file specified.");

    return NULL;
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

    if(sizeof(size_t) >= lua_size_t) big = 0;
    else if(sizeof(unsigned long long) >= lua_size_t) big = 1;
    else die("The lua binary size_t datatype is too big for this machine.");
    
    return content;
}

static inline char* print_lua_string(const char* prepend, char* stripped, unsigned long offset){
    union lua_size_t_type c;
    unsigned long long i;

    if(big){
        stripped = copy(&c.big, stripped, lua_size_t);
        stripped += lua_size_t;

        printf("%s %s ", COMMENT, prepend);
        for(i = offset; i < c.big; i++)
            putchar(*(stripped+i));
        putchar('\n');

        stripped += c.big;
    } else {
        stripped = copy(&c.small, stripped, lua_size_t);

        printf("%s %s ", COMMENT, prepend);
        for(i = offset; i < c.small; i++)
            putchar(*(stripped+i));
        putchar('\n');

        stripped += c.big;
    }

    return stripped;
}

static inline char* print_lua_opcodes(char* stripped){
    union lua_instruction_type ins;
    union lua_int_type p;
    unsigned long i;

    printf("%s Opcodes: \n", COMMENT);

    if(big){
        stripped = copy(&p.big, stripped, lua_int);

        for(i = 0; i < p.big;){
            stripped = copy(&ins.big, stripped, lua_instruction);
            printf("%04lu\t%s\n", i++, 
                   LUA_OPCODE[(unsigned char) (RETRIEVE_LUA_OPCODE(ins.big))]);/*
                   RETRIEVE_LUA_FIELD_A(ins.big),
                   RETRIEVE_LUA_FIELD_Bx(ins.big));*/
        }
    }else{
        stripped = copy(&p.small, stripped, lua_int);

        for(i = 0; i < p.small;){
            stripped = copy(&ins.small, stripped, lua_instruction);
            printf("%04lu\t%s\n", i++, 
                   LUA_OPCODE[(unsigned char) (RETRIEVE_LUA_OPCODE(ins.small))]);/*,
                   RETRIEVE_LUA_FIELD_A(ins.small),
                   RETRIEVE_LUA_FIELD_Bx(ins.small));*/
        }
    }
    return stripped;
}

static inline char* print_lua_constants(char* stripped){
    union lua_int_type p;
    unsigned long i;

    if(big){
        stripped = copy(&p.big, stripped, lua_int);
        printf("\n%s Constants: (Size %ld)\n", COMMENT, p.big);
        for(i = 0; i < p.big; i++){
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
    }else{
        stripped = copy(&p.small, stripped, lua_int);
        printf("\n%s Constants: (Size %d)\n", COMMENT, p.small);
        for(i = 0; i < p.small; i++){
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
    }

    return stripped;
}


void lua(char* file){
    char* file_contents = lua_check(file);
    char* stripped = file_contents+12;
    union lua_int_type p;

    if(big){
        stripped = print_lua_string("Source file name:", stripped, 1);

        stripped = copy(&p.big, stripped, lua_int);
        printf("%s Line defined: %ld\n", COMMENT, p.big);

        stripped = copy(&p.big, stripped, lua_int);
        printf("%s Last Line defined: %ld\n", COMMENT, p.big);
    } else {
        stripped = print_lua_string("Source file name:", stripped, 1);

        stripped = copy(&p.small, stripped, lua_int);
        printf("%s Line defined: %d\n", COMMENT, p.small);

        stripped = copy(&p.small, stripped, lua_int);
        printf("%s Last Line defined: %d\n", COMMENT, p.small);
    }

    printf("%s Number of Upvalues: %i\n", COMMENT, *(stripped)++);
    printf("%s Number of Downvalues: %i\n", COMMENT, *(stripped)++);
    printf("%s Vararg flag: %i\n", COMMENT, *(stripped)++);
    printf("%s Maximum stack size: %i\n", COMMENT, *(stripped)++);
    putchar('\n');

    stripped = print_lua_opcodes(stripped);

    stripped = print_lua_constants(stripped);

    free((char*)file_contents);
}

int main(int argc, char** argv){
    char *file = parse_args(argc, argv);
    regex_t regex;

    if(regcomp(&regex, LUA, 0))
        die("Could not compile regex.");

    if(regexec(&regex, file, 0, NULL, 0) == 0)
        lua(file);
    else
        die("Unknown file type.");

    return 0;
}
