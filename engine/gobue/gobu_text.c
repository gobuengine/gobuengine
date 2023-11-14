#include "gobu_text.h"
#include "gobu_transform.h"
#include "gobu_rendering.h"
#include "gobu_resource.h"
#include "raygo/rlgl.h"
#include "raygo/raymath.h"

ECS_COMPONENT_DECLARE(GText);

static void GobuText_Set(ecs_iter_t* it);
static void GobuText_Draw(ecs_iter_t* it);

void GobuTextImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuText);
    ECS_IMPORT(world, GobuTransform);

    ECS_COMPONENT_DEFINE(world, GText);

    const RenderPhases *phases = ecs_singleton_get(world, RenderPhases);

    ECS_OBSERVER(world, GobuText_Set, EcsOnSet, GText);

    ECS_SYSTEM(world, GobuText_Draw, phases->Draw, GText, ? GPosition, ? GScale, ? GRotation);
}

static void GobuText_Set(ecs_iter_t* it)
{
    GText* text = ecs_field(it, GText, 1);

    for (int i = 0; i < it->count; i++)
    {
        text[i].font = gobu_resource(it->world, text[i].resource)->font;
    }
}

static void GobuText_Draw(ecs_iter_t* it)
{
    GText* text = ecs_field(it, GText, 1);
    GPosition* post = ecs_field(it, GPosition, 2);
    GScale* scale = ecs_field(it, GScale, 3);
    GRotation* rota = ecs_field(it, GRotation, 4);

    for (int i = 0; i < it->count; i++)
    {
        Vector2 origin = (Vector2){ 0.0f, 0.0f };
        float rotation = (rota) ? rota[i] : 0.0f;
        GPosition post_ = (post) ? post[i] : (GPosition) { 0.0f, 0.0f };
        GScale scale_ = (scale) ? scale[i] : (GScale) { 1.0f, 1.0f };
        float fontSize = (text[i].size == 0) ? 20.0f : text[i].size;
        float spacing = (text[i].spacing == 0.0f) ? (fontSize / 10) : text[i].spacing;
        Color color = (text[i].color.a == 0) ? WHITE : text[i].color;

        rlPushMatrix();
        {
            rlTranslatef(post_.x, post_.y, 0.0f);
            rlRotatef(rotation, 0.0f, 0.0f, 1.0f);
            rlTranslatef(-origin.x, -origin.y, 0.0f);
            rlScalef(scale_.x, scale_.y, 1.0f);
            {
                DrawTextEx(text[i].font, text[i].text, Vector2Zero(), fontSize, spacing, color);
            }
        }
        rlPopMatrix();
    }
}

