#ifndef READ_PY_H
#define READ_PY_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "py_util.h"
#include "util.h"

static inline char* py_check(const char* file){
    char* content = NULL;
    char* copy_p = NULL;
    time_t timestamp;
    int size;
    size_t file_size = 0;
    FILE* py_file = fopen(file, "r");

    if(!py_file)
        die("Could not open file.");
    
    fseek(py_file, 0, SEEK_END);
    file_size = (size_t) ftell(py_file);
    fseek(py_file, 0, SEEK_SET);

    if(!file_size || file_size < 12)
        die("File is empty/not big enough to fit python header.");

    content = (char*) malloc(file_size + 1);

    if(!content)
        die("Could not allocate enough memory.");

    fread(content, file_size, 1, py_file);
    fclose(py_file);
    content[file_size] = 0;

    if(starts_with(content, PY_SIG) == 0)
        die("Python not right. Only Python3.4 bytecode is supported.");

    copy_p = content+4;

    copy_p = copy(&timestamp, copy_p, 4);
    copy_p = copy(&size, copy_p, 4);

    printf("%s Disassembly of file: %s\n", COMMENT, file);
    printf("%s Timestamp: %s", COMMENT, ctime(&timestamp));
    printf("%s Size: %i\n", COMMENT, size);

    return content;
}

static inline char* print_py_opcodes(char* content){
    return content;
}

static inline void py(char* file){
    char* file_contents = py_check(file);
    char* stripped = file_contents+PY_HEADER_SIZE;

    putchar('\n');
    stripped = print_py_opcodes(stripped);
    putchar('\n');

    puts("; End of binary information.");

    free(file_contents);
}

#endif
