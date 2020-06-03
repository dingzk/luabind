//
// Created by zhenkai on 2020/6/2.
//

#ifndef LUABIND_BINDVAR_H
#define LUABIND_BINDVAR_H

#include <lua.hpp>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define LT_NONE (-1)
#define LT_NIL 0
#define LT_INTEGER 1
#define LT_REAL 2
#define LT_STRING 3
#define LT_BOOLEAN 4
#define LT_POINTER 5

#define LT_DUPSTRING 10

#define LT_ARRAY 6
#define LT_MAP 7

#define DEFAULT_ARGS_NUM 3

typedef struct lval {
    int type;
    union {
        void *p;            /* light userdata */
        lua_Integer i;      /* integer numbers */
        lua_Number n;       /* float numbers */
        const char *str;    /* string */
        int b;              /* bool */
    } v;
} lval;

// 参数容器
typedef struct lvars {
    int n;
    int cap;
    lval *v;
} lvars;

class LUABridge {
private:
    lval* newval();
protected:
    lua_State *L;
    lvars *args;
public:
    LUABridge(lua_State *l);
    virtual ~LUABridge();

    float tonumber(int index);
    const char *tostring(int index);
    bool tobool(int index);
    void *topointer(int index);

    void pushnumber(float value);
    void pushstring(const char *value);
    void pushdupstring(char *value);
    void pushbool(bool value);
    void pushpointer(void *value);
    void pushnil();

};

class Request : public LUABridge {
public:
    Request(lua_State *l);

    lvars *gencargs();
};

class Response : public LUABridge {
public:
    Response(lua_State *l);

    int genluaargs();
};

#endif //LUABIND_BINDVAR_H
