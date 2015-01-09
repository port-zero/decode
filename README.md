# decode
A Lua/Python Bytecode Disassembler. It is in the very early stages of
development. The Lua Disassembler could be called semi-functional,
a typical listing it provides you with would look like this:

```
$ bin/decode hello.luac
File has 'luac' ending, assuming lua bytecode.

; Disassembly of file: hello.luac
; Endianness: little endian
; Int size: 4
; Size_t size: 8
; Instruction size: 4
; lua_Number size: 8
; Source file name: helloworld.lua
; Line defined: 0
; Last Line defined: 0
; Number of Upvalues: 0
; Number of Downvalues: 0
; Vararg flag: 2
; Maximum stack size: 2

; Opcodes: 
0000    GETGLOBAL
0001    LOADK
0002    CALL
0003    RETURN

; Constants: (Size 2)
0000 STRING: print
0001 STRING: Hello World!

; End of binary information.
```

As you might have noticed, the byte code has no arguments until now. This is
on my [TODO](#todo) list, like a few other features.

The code looks like it just came out of a cows' behind, so beware.

## Why another disassembler?

This disassembler is written purely in C, so it may well be faster than lad, 
ChunkySpy, dis et al.
Also, **it does not depend on any external libraries**. You can use it even
without havin Lua or Python installed. Yay, right?

## TODO

* Parse Lua instruction arguments (functions to retrieve instruction fields exist, but seem wrong)
* Parse remaining parts of Lua binary
  * function prototypes,
  * source line positions,
  * list of locals, and
  * list of upvalues
* Parse Python (only header is parsed until now)
