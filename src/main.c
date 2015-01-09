#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#include "read_lua.h"

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
