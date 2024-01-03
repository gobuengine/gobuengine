#include "gb_gfx.h"
#include "thirdparty/goburender/rlgl.h"

void gb_gfx_begin_lines(void)
{
    rlBegin(RL_LINES);
}

void gb_gfx_end()
{
    rlEnd();
}

void gb_gfx_color4ub(gb_color_t color)
{
    rlColor4ub(color.r, color.g, color.b, color.a);
}

void gb_gfx_vertex2f(float x, float y)
{
    rlVertex2f(x, y);
}

void gb_gfx_vertex3f(float x, float y, float z)
{
    rlVertex3f(x, y, z);
}

void gb_gfx_translate(float x, float y, float z)
{
    rlTranslatef(x, y, z);
}

void gb_gfx_rotate(float angle, float x, float y, float z)
{
    rlRotatef(angle, x, y, z);
}

void gb_gfx_scale(float x, float y, float z)
{
    rlScalef(x, y, z);
}

void gb_gfx_push_matrix(void)
{
    rlPushMatrix();
}

void gb_gfx_pop_matrix(void)
{
    rlPopMatrix();
}

void gb_gfx_init()
{
}

void gb_gfx_close()
{
}

void gb_gfx_draw_grid_2d(int slices, float spacing, gb_color_t color)
{
    int halfSlices = slices / 2;

    gb_gfx_begin_lines();
    for (int i = -halfSlices; i <= halfSlices; i++)
    {
        gb_gfx_color4ub(color);
        gb_gfx_color4ub(color);
        gb_gfx_color4ub(color);
        gb_gfx_color4ub(color);

        gb_gfx_vertex2f((float)i * spacing, (float)-halfSlices * spacing);
        gb_gfx_vertex2f((float)i * spacing, (float)halfSlices * spacing);

        gb_gfx_vertex2f((float)-halfSlices * spacing, (float)i * spacing);
        gb_gfx_vertex2f((float)halfSlices * spacing, (float)i * spacing);
    }
    gb_gfx_end();
}

void gb_gfx_draw_gismos(gb_transform_t transform, gb_bounding_t bonding_box)
{
    Rectangle bonding = (Rectangle){0.0f, 0.0f, bonding_box.max.x, bonding_box.max.y};

    gb_gfx_push_matrix();
    {
        DrawRectangleLinesEx(bonding, 2, SKYBLUE);
    }
    gb_gfx_pop_matrix();
}

/**
 * Función que dibuja un rectángulo en la pantalla.
 *
 * @param rect El rectángulo a dibujar.
 */
void gb_gfx_draw_rect(gb_shape_rect_t rect)
{
    DrawRectangle(rect.x, rect.y, rect.width, rect.height, rect.color);
    if (rect.border_width > 0)
        DrawRectangleLinesEx((gb_rect_t){rect.x, rect.y, rect.width, rect.height}, rect.border_width, rect.border_color);
}

/**
 * Función que dibuja un círculo en la pantalla.
 *
 * @param circle El círculo a dibujar.
 */
void gb_gfx_draw_circle(gb_shape_circle_t circle)
{
    DrawCircle(circle.x, circle.y, circle.radius, circle.color);
}

/**
 * Función que dibuja un sprite en la pantalla.
 *
 * @param sprite El sprite a dibujar.
 */
void gb_gfx_draw_sprite(gb_sprite_t sprite)
{
    Rectangle src = (Rectangle){sprite.src.x, sprite.src.y, sprite.src.width, sprite.src.height};
    Rectangle dst = (Rectangle){sprite.dst.x, sprite.dst.y, sprite.dst.width, sprite.dst.height};
    DrawTexturePro(sprite.texture, src, dst, Vector2Zero(), 0.0f, sprite.tint);
}

/**
 * Función para dibujar texto en la pantalla.
 *
 * @param text El texto a dibujar.
 */
void gb_gfx_draw_text(gb_text_t text)
{
    DrawTextEx(text.font, text.text, Vector2Zero(), text.size, text.spacing, text.color);
}

