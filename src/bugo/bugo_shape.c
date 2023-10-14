/**********************************************************************************
 * gobu_shape.h                                                                   *
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

#include "bugo.h"

void bugo_draw_shape_rect(Rectangle rec, Vector2 origin, float rotation, Color color)
{
    DrawRectanglePro(rec, origin, rotation, color);
}

void bugo_draw_shape_grid(int slices, float spacing)
{
    int halfSlices = slices / 2;

    bugo_gfx_begin_mode(GOBU_MODE_LINES);
    for (int i = -halfSlices; i <= halfSlices; i++)
    {
        if (i == 0)
        {
            bugo_gfx_color3f(0.18f, 0.18f, 0.18f);
            bugo_gfx_color3f(0.18f, 0.18f, 0.18f);
            bugo_gfx_color3f(0.18f, 0.18f, 0.18f);
            bugo_gfx_color3f(0.18f, 0.18f, 0.18f);
        }
        else
        {
            bugo_gfx_color3f(0.19f, 0.19f, 0.19f);
            bugo_gfx_color3f(0.19f, 0.19f, 0.19f);
            bugo_gfx_color3f(0.19f, 0.19f, 0.19f);
            bugo_gfx_color3f(0.19f, 0.19f, 0.19f);
        }

        bugo_gfx_vert2f((float)i * spacing, (float)-halfSlices * spacing);
        bugo_gfx_vert2f((float)i * spacing, (float)halfSlices * spacing);

        bugo_gfx_vert2f((float)-halfSlices * spacing, (float)i * spacing);
        bugo_gfx_vert2f((float)halfSlices * spacing, (float)i * spacing);
    }
    bugo_gfx_end_mode();
}
