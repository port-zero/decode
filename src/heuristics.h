#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "read_lua.h"
#include "read_py.h"

static inline void heuristics(char* file){
    char* content = NULL;
    FILE* f = fopen(file, "r");
    size_t unused;

    if(!f)
        die("Could not open file.");

    content = (char*) malloc(10 * sizeof(char));

    if(!content)
        die("Could not allocate enough memory.");

    unused = fread(content, 9, 1, f);
    if(unused != 1) fprintf(stderr, "Reading the file in may have failed.\n");
    fclose(f);
    content[9] = 0;

    if(starts_with(content, LUA_SIG))
        lua(file);
    else if(starts_with(content, PY_SIG))
        py(file);
    else
        puts("Could not match magic numbers. decode only support Lua 5.1/Python 3.4.");

    free(content);
}

#endif
