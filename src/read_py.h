#ifndef READ_PY_H
#define READ_PY_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "py_util.h"
#include "util.h"

static inline char* py_check(const char* file, size_t* file_size){
    char* content = NULL;
    char* copy_p = NULL;
    time_t timestamp;
    int size;
    size_t unused;
    FILE* py_file = fopen(file, "r");

    if(!py_file)
        die("Could not open file.");
    
    fseek(py_file, 0, SEEK_END);
    *file_size = (size_t) ftell(py_file);
    fseek(py_file, 0, SEEK_SET);

    if(!(*file_size) || *file_size < 12)
        die("File is empty/not big enough to fit python header.");

    content = (char*) malloc((*file_size) + 1);

    if(!content)
        die("Could not allocate enough memory.");

    unused = fread(content, *file_size, 1, py_file);
    if(unused != 1) fprintf(stderr, "Reading the file in may have failed.\n");
    fclose(py_file);
    content[*file_size] = 0;

    if(starts_with(content, PY_SIG) == 0)
        die("Python not right. Only Python3.4 bytecode is supported.");

    copy_p = content+4;

    copy_p = copy(&timestamp, copy_p, 4);
    copy_p = copy(&size, copy_p, 4);
    *file_size = (size_t)size;

    printf("%s Disassembly of file: %s\n", COMMENT, file);
    printf("%s Compilation Timestamp: %s", COMMENT, ctime(&timestamp));
    printf("%s Size: %i\n", COMMENT, size);

    return content;
}

static inline char* print_py_opcodes(char* content, size_t len){
    /* All further info is from CPython: Python/marshal.c */
    size_t i;
    unsigned char x;
    uint16_t arg;
    for(i = 35; i <= len; i++) {
      x = (unsigned char) content[i];
      printf("[%zu] %s(%d) ", i-35, PY_OPCODES[x], x);
      if(HAS_ARG(x)) {
        arg = (uint16_t) ((content[i+1]<<8) + content[i+2]);
        printf("%d\n", arg);
        i += 2;
      }
      else puts("");
    }
    return content;
}

static inline void py(char* file){
    size_t len;
    initialize_py_ops();

    char* file_contents = py_check(file, &len);
    char* stripped = file_contents+PY_HEADER_SIZE;

    putchar('\n');
    stripped = print_py_opcodes(stripped, len);
    putchar('\n');

    puts("; End of binary information.");

    free(file_contents);
}

#endif
