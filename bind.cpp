#include <lua.hpp>
#include <iostream>
#include <assert.h>


// g++ -std=c++11 -I/data1/apache2/htdocs/LuaJIT/src -Wl,-rpath=/data1/apache2/htdocs/LuaJIT/src  -L/data1/apache2/htdocs/LuaJIT/src -lluajit t.cpp


static void lbind_message (const char *msg) {
    fprintf(stderr, "\n=====\nError: %s\n=====\n", msg);
}

static int lbind_report (lua_State *L, int status) {
    if (status != LUA_OK) {
        const char *msg = lua_tostring(L, -1);
        lbind_message(msg);
        lua_pop(L, 1);  /* remove error message */
    }

    return status;
}

/*
** Message handler used to run all chunks
*/
static int l_msghandler (lua_State *L) {
    const char *msg = lua_tostring(L, 1);
    if (msg == NULL) {  /* is error object not a string? */
        if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
            lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
            return 1;  /* that is the message */
        else
            msg = lua_pushfstring(L, "(error object is a %s value)",
                                  luaL_typename(L, 1));
    }

    /* return the traceback */
    luaL_traceback(L, L, msg, 1);  /* append a standard traceback */

    return 1;
}

static int lbind_docall (lua_State *L, int narg, int nres) {
    int status;
    int base = lua_gettop(L) - narg;  /* function index */
    if (base < 0) assert(0);
    lua_pushcfunction(L, &l_msghandler);  /* push message handler */
    lua_insert(L, base);  /* put it under function and args */
    status = lua_pcall(L, narg, nres, base);
    lua_remove(L, base);  /* remove message handler from the stack */

    return lbind_report(L, status);
}

static int l_dofile(lua_State *L) {
    char *name = (char *)lua_touserdata(L, 1);
    int status = luaL_loadfile(L, name);

    if (status == LUA_OK) status = lbind_docall(L, 0, 0);
    lbind_report(L, status);

    lua_pushinteger(L, status == LUA_OK);

    return 1;
}

/*
** Main body of stand-alone interpreter (to be called in protected mode).
** Reads the options and handles them all.
*/
static int lbind_dofile (lua_State *L, const char *filename) {
    lua_pushcfunction(L, &l_dofile);  /* to call 'l_dofile' in protected mode */
    lua_pushlightuserdata(L, const_cast<char *> (filename));

    int status = lbind_docall(L, 1, 1);   /* do the call */

    int ok = 0;
    if (status == LUA_OK) {
        ok = lua_tointeger(L, -1);
        lua_pop(L , 1);     /*necessary */
    }

    return ok;
}

int main(void)
{
    int status = LUA_OK, result;
    lua_State *L = luaL_newstate();  /* create state */
    if (L == NULL) {
        fprintf(stderr, "cannot create state: not enough memory");
        return EXIT_FAILURE;
    }

    result = lbind_dofile(L, "t.lua");
    printf("load file ok ? %d\n", result);
    if(!result) {
        return EXIT_FAILURE;
    }

    lua_getglobal(L, "init");
    lua_pushinteger(L, 10);

    int error = lbind_docall(L, 1, 1);
    printf("call func ok ? %d\n", error == LUA_OK);

    printf("stack num is: %d\n", lua_gettop(L));

    printf("return : %d\n", lua_tointeger(L, -1));

    lua_close(L);

    return (result && status == LUA_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
}