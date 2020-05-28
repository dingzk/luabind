#include <lua.hpp>
#include <iostream>
#include <assert.h>

static void dd(lua_State *L) {
    printf("top %d\n", lua_gettop(L));
}

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

int lbind_docall (lua_State *L, int narg, int nres) {
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

    lua_pushboolean(L, status == LUA_OK);

    return 1;
}

/*
** Main body of stand-alone interpreter (to be called in protected mode).
** Reads the options and handles them all.
*/
int lbind_dofile (lua_State *L, const char *filename) {
    lua_pushcfunction(L, &l_dofile);  /* to call 'l_dofile' in protected mode */
    lua_pushlightuserdata(L, const_cast<char *> (filename));

    int status = lbind_docall(L, 1, 1);   /* do the call */

    int ok = 0;
    if (status == LUA_OK) {
        ok = lua_toboolean(L, -1);
        lua_pop(L , 1);     /*necessary */
    }

    return ok;
}

static int l_register(lua_State *L) {
    const char *funcname = (const char *)lua_touserdata(L, 1);
    luaL_checktype(L, 2, LUA_TLIGHTUSERDATA);

    lua_getglobal(L, "CPP");
    if (lua_type(L, -1) != LUA_TTABLE) {
        lua_createtable(L, 0 /* narr */, 1 /* nrec */);
        lua_pushvalue(L, -1);
        lua_setglobal(L, "CPP");
    }

    lua_pushcfunction(L, (lua_CFunction)lua_touserdata(L, 2));
    lua_setfield(L, -2, funcname);

    lua_pop(L, -1);

    return 0;
}

int lbind_register(lua_State *L, const char *funcname, lua_CFunction f) {

    lua_pushcfunction(L, l_register);
    lua_pushlightuserdata(L, const_cast<char *> (funcname));
    lua_pushlightuserdata(L, (void *)f);

    return lbind_docall(L, 2, 0);
}