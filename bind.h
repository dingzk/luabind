#include <lua.hpp>
#include <iostream>
#include <assert.h>

typedef int (*User_CFunction) (lua_State *L);

int lbind_docall (lua_State *L, int narg, int nres);

int lbind_dofile (lua_State *L, const char *filename);

int lbind_register(lua_State *L, const char *funcname, User_CFunction f);