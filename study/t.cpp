#include <lua.hpp>
#include <iostream>


// g++ -std=c++11 -I/data1/apache2/htdocs/LuaJIT/src -Wl,-rpath=/data1/apache2/htdocs/LuaJIT/src  -L/data1/apache2/htdocs/LuaJIT/src -lluajit t.cpp

static int errlog(lua_State *L)
{
    printf("top errlog 1 : %d\n", lua_gettop(L));

    printf("\n%s\n",lua_tostring(L, -1));

//    lua_pop(L, -1);
    printf("top errlog 2 : %d\n", lua_gettop(L));

    return 1;
}


int main(void)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    luaL_dofile(L, "t.lua");

//    lua_pushcfunction(L, errlog);

    lua_getglobal(L, "init");
    lua_pushinteger(L, 10);

    printf("top-1 : %d\n", lua_gettop(L));

    int error = lua_pcall(L, 1, 1, 1);
    printf("top1 : %d\n", lua_gettop(L));
    if(error) {
        printf("top2 : %d\n", lua_gettop(L));
        printf("\n%s\n",lua_tostring(L, -1));
//        printf("\n%d\n",lua_tonumber(L, -1));
        printf("top3 : %d\n", lua_gettop(L));
        //此句不能少，否则lua_tostring在栈顶留下的信息会保留，影响后面函数调用
        lua_pop(L, -1);
        printf("top4 : %d\n", lua_gettop(L));
    }

    printf("top5 : %d\n", lua_gettop(L));

    printf("%f\n", lua_tonumber(L, -1));
    int t = lua_tonumber(L, -1);
    printf("%d\n", 10.00);
    printf("%d\n", (int)10.0);

    printf("%d\n", lua_tointeger(L, -1));
    printf("%s\n", lua_tostring(L, -1));


    lua_close(L);

    return 0;
}

