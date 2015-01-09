#ifndef LUA_UTIL_H
#define LUA_UTIL_H


#define LUA ".*\\.luac"
#define LUA_SIG "\x1B\x4C\x75\x61" //  = \27Lua
#define LUA_VERSION 0x51

#define RETRIEVE_LUA_OPCODE(N) N & 0x3F
#define RETRIEVE_LUA_FIELD_A(N) N & 0x3FC0000
#define RETRIEVE_LUA_FIELD_B(N) N & 0x3FE00
#define RETRIEVE_LUA_FIELD_BX(N) N && 0x3FFFF
#define RETRIEVE_LUA_FIELD_SBX(N) (signed) RETRIEVE_LUA_FIELD_BX(N)
#define RETRIEVE_LUA_FIELD_C(N) N & 0x3FC0000

enum LUA_FIELD_USAGE{
    AB,
    ABX,
    ABC,
    SBX,
    AC,
    ASBX,
    A
};

static const char* const LUA_OPCODE[] = { "MOVE", "LOADK", "LOADBOOL", "LOADNIL",
                                          "GETUPVAL", "GETGLOBAL", "GETTABLE",
                                          "SETGLOBAL", "SETUPVAL", "SETTABLE",
                                          "NEWTABLE", "SELF", "ADD", "SUB", "MUL", 
                                          "DIV", "MOD", "POW", "UNM", "NOT", "LEN", 
                                          "CONCAT", "JMP", "EQ", "LT", "LE", "TEST", 
                                          "TESTSET", "CALL", "TAILCALL", "RETURN", 
                                          "FORLOOP", "FORPREP", "TFORLOOP", "SETLIST", 
                                          "CLOSE", "CLOSURE", "VARARG" };

static const char LUA_OPCODE_FIELDS[] = { AB, ABX, ABC, AB, AB, ABX, ABC, ABX, AB, ABC,
                                          ABC, ABC, ABC, ABC, ABC, ABC, ABC, ABC, AB, AB,
                                          AB, ABC, SBX, ABC, ABC, ABC, AC, ABC, ABC, ABC, 
                                          AB, ASBX, ASBX, AC, ABC, A, ABX, AB };

static char lua_int = 0;
static char lua_size_t = 0;
static char lua_instruction = 0;


#endif
