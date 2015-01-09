#ifndef LUA_UTIL_H
#define LUA_UTIL_H


#define LUA ".*\\.luac"
#define LUA_SIG "\x1BLua"
#define LUA_VERSION 0x51
#define LUA_RETURN 30

#define RETRIEVE_LUA_OPCODE(N) N & 0x3F
#define RETRIEVE_LUA_FIELD_A(N) N >> 9
#define RETRIEVE_LUA_FIELD_B(N) (N >> 18) & 0x1FF
#define RETRIEVE_LUA_FIELD_Bx(N) (N >> 18)
#define RETRIEVE_LUA_FIELD_sBx(N) (N >> 18)
#define RETRIEVE_LUA_FIELD_C(N) (N >> 26) & 0xFF

static const char* const LUA_OPCODE[] = { "MOVE", "LOADK", "LOADBOOL", "LOADNIL",
                                          "GETUPVAL", "GETGLOBAL", "GETTABLE",
                                          "SETGLOBAL", "SETUPVAL", "SETTABLE",
                                          "NEWTABLE", "SELF", "ADD", "SUB", "MUL", 
                                          "DIV", "MOD", "POW", "UNM", "NOT", "LEN", 
                                          "CONCAT", "JMP", "EQ", "LT", "LE", "TEST", 
                                          "TESTSET", "CALL", "TAILCALL", "RETURN", 
                                          "FORLOOP", "FORPREP", "TFORLOOP", "SETLIST", 
                                          "CLOSE", "CLOSURE", "VARARG" };

static char lua_int = 0;
static char lua_size_t = 0;
static char lua_instruction = 0;


#endif
