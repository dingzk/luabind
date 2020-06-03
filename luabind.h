//
// Created by zhenkai on 2020/6/2.
//

#ifndef LUABIND_LUABIND_H
#define LUABIND_LUABIND_H

#include <lua.hpp>
#include <iostream>
#include <assert.h>
#include "bindvar.h"

#define CPP "CPP"

typedef int (*user_CFunction) (lua_State *L);

typedef void (*lbind_CFunction) (Request &request, Response &response);

int lbind_docall (lua_State *L, int narg, int nres);

int lbind_dofile (lua_State *L, const char *filename);

int lbind_register(lua_State *L, const char *funcname, void * func, bool isoriginfunc = true);


#endif //LUABIND_LUABIND_H
