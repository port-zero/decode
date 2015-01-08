#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define VERSION "0.1"
#define LUA ".*\\.luac"
#define LUA_SIG "\27Lua"
#define PYTHON "*.pyc"
#define LUA_VERSION 0x52
#define COMMENT ";"
/*
static const char* const LUA_OPCODE[] = { "MOVE", "LOADK", "LOADKX", "LOADBOOL", "LOADNIL",
                                          "GETUPVAL", "GETTABUP", "GETTABLE", "SETTABUP",
                                          "SETUPVAL", "SETTABLE", "NEWTABLE", "SELF",
                                          "ADD", "SUB", "MUL", "DIV", "MOD", "POW",
                                          "UNM", "NOT", "LEN", "CONCAT", "JMP",
                                          "EQ", "LT", "LE", "TEST", "TESTSET",
                                          "CALL", "TAILCALL", "RETURN", "FORLOOP",
                                          "FORPREP", "TFORCALL", "TFORLOOP", "SETLIST",
                                          "CLOSURE", "VARARG", "EXTRAARG" };
*/
static inline void print_usage(int returncode){
    printf("decode, the interpreter disassembler\nVersion %s\n\nUsage: decode <obj_file>", VERSION);
    exit(returncode);
}

static inline void die(const char* message){
    fprintf(stderr, "%s\n", message);
    exit(1);
}

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

static inline int starts_with(const char* a, const char* b){
    if(strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
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

    printf("%s Endianness: %s endian\n", COMMENT, content[6] == 0 ? "big" : "little");
    printf("%s Int size: %i\n", COMMENT, content[7]);
    printf("%s Size_t size: %i\n", COMMENT, content[8]);
    printf("%s Instruction size: %i\n", COMMENT, content[9]);
    printf("%s lua_Number size: %i\n", COMMENT, content[10]);

    return content;
}

void lua(char* file){
    char* file_contents = lua_check(file);
    char* stripped = file+12;

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
