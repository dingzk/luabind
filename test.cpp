#include "luabind.h"

// g++ -std=c++11 -I/data1/apache2/htdocs/LuaJIT/src -Wl,-rpath=/data1/apache2/htdocs/LuaJIT/src  -L/data1/apache2/htdocs/LuaJIT/src -lluajit test.cpp bind.cpp

int user_func(lua_State *L) {

    int a = lua_tointeger(L, -1);

    lua_pushstring(L, "luacfunction");
    lua_pushinteger(L, a * a);

    return 2;
}

void *u_func(Request &request, Response &response) {

    const char *a = request.tostring(0);
    const char *b = request.tostring(1);
    int c = request.tonumber(2);

    const char *h = "const_char";
    char *strtmp = strdup(const_cast<char *>("hello"));

//    response.pushstring(h);
    response.pushdupstring(strtmp);
    response.pushstring(b);
    response.pushnumber(c);

    free(strtmp);

}

int main(void)
{
    int status = LUA_OK, result;
    lua_State *L = luaL_newstate();  /* create state */
    if (L == NULL) {
        fprintf(stderr, "cannot create state: not enough memory");
        return EXIT_FAILURE;
    }
    luaL_openlibs(L);
    result = lbind_dofile(L, "t.lua");
    printf("load file ok ? %d\n", result);
    if(!result) {
        return EXIT_FAILURE;
    }

    lbind_register(L, "func", (void *)u_func, false);
    lbind_register(L, "func2", (void *)user_func, true);

    lua_getglobal(L, "init");
    lua_pushinteger(L, 11);
    int error = lbind_docall(L, 1, 1);

    printf("call func ok ? %d\n", error == LUA_OK);

    printf("stack num is: %d\n", lua_gettop(L));

    printf("return : %d\n", lua_tointeger(L, -1));

    lua_close(L);

    return (result && status == LUA_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
}