#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#include "heuristics.h"

static inline char * parse_args(int argc, char** argv){
    register unsigned int i;

    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], "-h") == 0)
            print_usage(0);
        else
            return argv[i];
    }

    die("No input file specified.");

    return NULL;
}

static inline int check_ending(char* file, const char* ending){
    regex_t regex;
    if(regcomp(&regex, ending, 0))
        die("Could not compile regex.");

    return regexec(&regex, file, 0, NULL, 0) == 0;
}

int main(int argc, char** argv){
    char *file = parse_args(argc, argv);

    if(check_ending(file, LUA)){
        puts("File has 'luac' ending, assuming lua bytecode.\n");
        lua(file);
    }
    else if(check_ending(file, PY)){
        puts("File has 'pyc' ending, assuming python bytecode.\n");
        py(file);
    }
    else{
        heuristics(file);
    }

    return 0;
}
