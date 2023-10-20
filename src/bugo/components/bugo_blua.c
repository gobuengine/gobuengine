#include "bugo_blua.h"

#include <lualib.h>
#include <lauxlib.h>
#include <lua.h>

static ecs_entity_t ecs_lua_t;
static ecs_entity_t sys_lua_t;
static lua_State* L;

static void bugo_system_on_lua_call(ecs_iter_t* it)
{
    // ComponentScript* l = ecs_field(it, ComponentScript, 1);
    // ComponentRenderer* r = ecs_field(it, ComponentRenderer, 2);

    lua_rawgeti(L, LUA_REGISTRYINDEX, it->ctx);
    lua_pushlightuserdata(L, it);
    if (lua_pcall(L, 1, 0, 0) == LUA_OK)
        lua_pop(L, lua_gettop(L));

// lua_createtable(L, 0, 1);
//     lua_pushstring(L, "add_rotation");
//     lua_pushlightuserdata(L, r->rotation);
//     lua_pushlightuserdata(L, r->rotation);
// lua_settable(L, -3);

// for (int i = 0; i < it->count; i++) {
//     lua_rawgeti(L, LUA_REGISTRYINDEX, it->ctx);
//     lua_pushlightuserdata(L, &l[i]);
//     lua_pushlightuserdata(L, &r[i]);
//     if (lua_pcall(L, 2, 0, 0) == LUA_OK)
//         lua_pop(L, lua_gettop(L));
// }

}

static int iter_count(lua_State* L) {
    ecs_iter_t* it = lua_touserdata(L, 1);
    lua_pushinteger(L, it->count);
    return 1;
}

static int wrap_ecs_field(lua_State* L) {
    ecs_iter_t* it = lua_touserdata(L, 1);
    int column = lua_tointeger(L, 2);
    ComponentRenderer* r = ecs_field(it, ComponentRenderer, column);
    lua_pushlightuserdata(L, r);
    return 1;
}

static int test_set_rotate(lua_State* L) {
    ComponentRenderer* r = lua_touserdata(L, 1);
    int column = lua_tointeger(L, 2);
    float value = lua_tonumber(L, 3);

    r[0].rotation = value;
    return 0;
}

static int test_get_rotate(lua_State* L) {
    ComponentRenderer* r = lua_touserdata(L, 1);
    int column = lua_tointeger(L, 2);
    lua_pushnumber(L, r[0].rotation);
    return 1;
}

static int Engine_System(lua_State* L)
{
    BugoEcsPhaser phaser_n;
    ecs_world_t* world = bugo_ecs_world();

    lua_getglobal(L, "Entity");
    ecs_entity_t entity_self = lua_tonumber(L, -1);

    // parametros func
    int phaser = luaL_checkinteger(L, 1);

    if (phaser == PHASER_UPDATE) {
        phaser_n = PHASER_UPDATE;
    }
    else {
        return 0;
    }
    lua_pushvalue(L, 2);
    int func = luaL_ref(L, LUA_REGISTRYINDEX);

    // Get Component entity
    const ecs_type_t* type = ecs_get_type(world, entity_self);
    for (uint32_t i = (type->count - 1); i <= type->count; i--)
    {
        ecs_id_t id = type->array[i];
        ecs_entity_t component = ecs_pair_second(world, id);
        const void* component_ptr = ecs_get_id(world, entity_self, component);

        const char* name_cmp = ecs_get_name(world, component);

        printf("Name: %s\n", name_cmp);

        // if (strcmp(name_cmp, "ComponentRenderer") == 0) {
            // // ecs_meta_cursor_t cur = ecs_meta_cursor(world, component, component_ptr);
            // ecs_f32_t* fieldv = (ecs_f32_t*)(void*)ECS_ELEM(component_ptr, sizeof(ecs_f32_t) * 1, 7);

            // lua_createtable(L, 0, 1);
            // {
            //     lua_pushstring(L, "self");
            //     lua_pushlightuserdata(L, component_ptr);
            //     lua_settable(L, -3);

            //     lua_pushstring(L, "rotation");
            //     lua_pushnumber(L, fieldv[0]);
            //     lua_settable(L, -3);

            //     lua_pushstring(L, "set_rotate");
            //     lua_pushcfunction(L, _set_rotation);
            //     lua_settable(L, -3);
            // }
            // lua_setglobal(L, name_cmp);
            // lua_pop(L, lua_gettop(L));
        // }
    }

    // Create system
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = {ecs_dependson(bugo_ecs_get_phaser(phaser_n))},
        }),
        .query.filter.terms = {
            {.id = ecs_lua_t},
            {.id = bugo_ecs_get_renderer_id()},
        },
        .callback = bugo_system_on_lua_call,
        .ctx = func,
    });

    printf("INFO: Create System\n");

    return 0;
}

static void bugo_system_on_set(ecs_iter_t* it)
{
    ComponentScript* lua = ecs_field(it, ComponentScript, 1);
    for (int i = 0; i < it->count; i++)
    {
        char* path = g_build_filename(bugo_project_get_path(), "Content", lua[i].script, NULL);
        lua_pushinteger(L, it->entities[i]);
        lua_setglobal(L, "Entity");

        g_return_if_fail(luaL_loadfile(L, path) != LUA_ERRFILE);

        if (lua_pcall(L, 0, 0, 0) == LUA_OK)
            lua_pop(L, lua_gettop(L));

        printf("INFO: ECS-Script load\n");
    }
}

void bugo_ecs_init_lua(ecs_world_t* world)
{
    L = luaL_newstate();
    luaL_openlibs(L);

    const struct luaL_Reg reg_fun[] = {
        {"System", Engine_System},
        {"ecs_iter_count", iter_count},
        {"ecs_field", wrap_ecs_field},
        {"set_rotate", test_set_rotate},
        {"get_rotate", test_get_rotate},
        {NULL, NULL}
    };

    lua_newtable(L);
    luaL_setfuncs(L, &reg_fun, 0);
    lua_setglobal(L, "Engine");

    ecs_lua_t = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, { .name = "ComponentScript" }),
            .type.size = ECS_SIZEOF(ComponentScript),
            .type.alignment = ECS_ALIGNOF(ComponentScript),
    });

    ecs_struct(world, {
        .entity = ecs_lua_t,
        .members = {
            {.name = "bActive", .type = ecs_id(ecs_bool_t) },
            {.name = "script", .type = ecs_id(ecs_string_t) },
        },
    });

    ecs_observer(world, {
        .filter = {.terms = {{.id = ecs_lua_t }}},
        .events = { EcsOnSet },
        .callback = bugo_system_on_set
    });
}

void bugo_ecs_set_lua(ecs_entity_t entity, ComponentScript* props)
{
    ecs_set_id(bugo_ecs_world(), entity, ecs_lua_t, sizeof(ComponentScript), props);
}

ecs_entity_t bugo_ecs_get_lua_id(void)
{
    return ecs_lua_t;
}

