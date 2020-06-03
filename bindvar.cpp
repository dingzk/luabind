//
// Created by zhenkai on 2020/6/2.
//

#include "bindvar.h"
#include <unordered_map>
#include <string>

static void freeargs(lvars *args)
{
    if (args == nullptr) {
        return;
    }
    if (args->v != nullptr) {
        for (int i = 0; i < args->n; ++i) {
            if (args->v[i].type == LT_DUPSTRING) {
                free(const_cast<char *>(args->v[i].v.str));
            }
        }
        free(args->v); // 指针类型等lua GC自动释放?
        args->v = nullptr;
    }
    free(args);
    args = nullptr;
}

lval* LUABridge::newval()
{
    if (args->n == 0) {
        args->v = (lval *) malloc(sizeof(lval) * args->cap);
        memset(args->v, 0, sizeof(lval) * args->cap);
    } else if (args->n >= args->cap) {
        args->v = (lval *) realloc(args->v, sizeof(lval) * args->cap * 2);
        memset(args->v + args->cap, 0, sizeof(lval) * args->cap);
        args->cap *= 2;
    }

    return args->v + args->n++;
}

static void read_table(lua_State *L, std::unordered_map<std::string, std::string> &args, int index)
{
    lua_pushnil(L);  /* first key */
    while (lua_next(L, index) != 0) {
        args.insert({std::string(lua_tostring(L, -2)), std::string(lua_tostring(L, -1))});
        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(L, 1);
    }
}

LUABridge::LUABridge(lua_State *l) : L(l)
{
    args = (lvars *) malloc(sizeof(lvars));
    memset(args, 0, sizeof(lvars));

    args->cap = DEFAULT_ARGS_NUM;
}

LUABridge::~LUABridge() {
    freeargs(args);
}

float LUABridge::tonumber(int index) {
    if (index > args->n) {
        assert(0);
    }
    assert(args->v[index].type == LT_REAL);

    return args->v[index].v.n;
}

bool LUABridge::tobool(int index) {
    if (index > args->n) {
        assert(0);
    }
    assert(args->v[index].type == LT_BOOLEAN);

    return args->v[index].v.b;
}

const char * LUABridge::tostring(int index) {
    if (index > args->n) {
        assert(0);
    }
    assert(args->v[index].type == LT_STRING || args->v[index].type == LT_DUPSTRING);

    return args->v[index].v.str;
}

void * LUABridge::topointer(int index) {
    if (index > args->n) {
        assert(0);
    }
    assert(args->v[index].type == LT_POINTER);

    return args->v[index].v.p;
}

void LUABridge::pushnil() {
    lval *val = newval();
    val->type = LT_NIL;
    val->v.p = nullptr;
}

void LUABridge::pushnumber(float value) {
    lval *val = newval();
    val->type = LT_REAL;
    val->v.n = value;
}

void LUABridge::pushbool(bool value) {
    lval *val = newval();
    val->type = LT_BOOLEAN;
    val->v.b = value;
}

void LUABridge::pushpointer(void *value) {
    lval *val = newval();
    val->type = LT_POINTER;
    val->v.p = value;
}

void LUABridge::pushstring(const char *value) {
    lval *val = newval();
    val->type = LT_STRING;
    val->v.str = value;
}

void LUABridge::pushdupstring(char *value) {
    if (value == nullptr) {
        return;
    }
    lval *val = newval();
    val->type = LT_DUPSTRING;
    val->v.str = strdup(value);
}

Request::Request(lua_State *l) : LUABridge(l) {}

Response::Response(lua_State *l) : LUABridge(l) {}

lvars * Request::gencargs() {
    int n = lua_gettop(L);
    if (n <= 0) {
        return nullptr;
    }
    args->cap = n;
    for (int i = 1; i <= n ; ++i) {
        switch (lua_type(L, i)) {
            case LUA_TNONE:
            case LUA_TNIL:
                pushnil();
                break;
            case LUA_TNUMBER:
                pushnumber(lua_tonumber(L, i));
                break;
            case LUA_TBOOLEAN:
                pushbool(lua_toboolean(L, i));
                break;
            case LUA_TSTRING:
                pushstring(lua_tostring(L, i));
                break;
            case LUA_TUSERDATA:
                pushpointer(lua_touserdata(L, i));
                break;
            case LUA_TTABLE:
            case LUA_TFUNCTION:
            case LUA_TTHREAD:
            case LUA_TLIGHTUSERDATA:
            default:
                luaL_error(L, "not support this arg type!");
                break;
        }
    }

    return args;
}

int Response::genluaargs() {
    if (args->n <= 0) {
        return 0;
    }
    for (int i = 0; i < args->n; ++i) {
        switch (args->v[i].type) {
            case LT_NONE:
            case LT_NIL:
                lua_pushnil(L);
                break;
            case LT_REAL:
            case LT_INTEGER:
                lua_pushnumber(L, tonumber(i));
                break;
            case LT_BOOLEAN:
                lua_pushboolean(L, tobool(i));
                break;
            case LT_STRING:
            case LT_DUPSTRING:
                lua_pushstring(L, tostring(i));
                break;
            case LT_POINTER:
                lua_pushlightuserdata(L, topointer(i));
                break;
            case LT_ARRAY:
            case LT_MAP:
            default:
                luaL_error(L, "not support this arg type!");
                break;
        }
    }

    return args->n;
}

