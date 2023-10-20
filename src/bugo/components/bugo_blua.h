/**********************************************************************************
 * bugo_rectangle.h                                                               *
 **********************************************************************************
 * GOBU ENGINE                                                                    *
 * https://gobuengine.org                                                         *
 **********************************************************************************
 * Copyright (c) 2023 Jhonson Ozuna Mejia                                         *
 *                                                                                *
 * Permission is hereby granted, free of charge, to any person obtaining a copy   *
 * of this software and associated documentation files (the "Software"), to deal  *
 * in the Software without restriction, including without limitation the rights   *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
 * copies of the Software, and to permit persons to whom the Software is          *
 * furnished to do so, subject to the following conditions:                       *
 *                                                                                *
 * The above copyright notice and this permission notice shall be included in all *
 * copies or substantial portions of the Software.                                *
 *                                                                                *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  *
 * SOFTWARE.                                                                      *
 **********************************************************************************/

#ifndef __BUGO_COMPONENT_LUA_H__
#define __BUGO_COMPONENT_LUA_H__
#include <stdio.h>
#include "component.h"

typedef struct ComponentScript {
    bool bActive;
    char* script;
}ComponentScript;

void bugo_ecs_init_lua(ecs_world_t* world);
void bugo_ecs_set_lua(ecs_entity_t entity, ComponentScript* props);
ecs_entity_t bugo_ecs_get_lua_id(void);

#endif // __BUGO_COMPONENT_LUA_H__

