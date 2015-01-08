#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#include "util.h"

static char lua_int = 0;
static char lua_size_t = 0;
static char lua_instruction = 0;

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

    puts("File has 'luac' ending, assuming lua bytecode.");

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
        die("This disassembler only works with Lua 5.2.");

    lua_int = content[7];
    lua_size_t = content[8];
    lua_instruction = content[9];

    printf("%s Endianness: %s endian\n", COMMENT, content[6] == 0 ? "big" : "little");
    printf("%s Int size: %i\n", COMMENT, lua_int);
    printf("%s Size_t size: %i\n", COMMENT, lua_size_t);
    printf("%s Instruction size: %i\n", COMMENT, lua_instruction);
    printf("%s lua_Number size: %i\n", COMMENT, content[10]);

    return content;
}

void lua(char* file){
    char* file_contents = lua_check(file);
    char* stripped = file+12;
    if(sizeof(size_t) >= lua_size_t)
        size_t string_size = 0;
    else if(sizeof(unsigned long long) >= lua_size_t)
        unsigned long long string_size = 0;
    else
        die("The lua binary size_t datatype is too big for this machine.");

    if(!memcpy(&string_size, stripped, lua_size_t)
        die("Could not copy memory");
    
    printf("%s\n", file_contents);
    printf("%s\n", stripped);

    free(file);
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
