#include "gobu_shapes.h"
#include "gobu_transform.h"
#include "gobu_rendering.h"
#include "raygo/rlgl.h"

ECS_COMPONENT_DECLARE(GShapeRec);

static void GobuShapes_DrawRect(ecs_iter_t* it);

void GobuShapesImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuShapes);
    ECS_IMPORT(world, GobuTransform);

    ECS_COMPONENT_DEFINE(world, GShapeRec);

    const RenderPhases *phases = ecs_singleton_get(world, RenderPhases);

    ECS_SYSTEM(world, GobuShapes_DrawRect, phases->Draw, GShapeRec, ? GPosition, ? GScale, ? GRotation);
}

static void GobuShapes_DrawRect(ecs_iter_t* it)
{
    GShapeRec* rect = ecs_field(it, GShapeRec, 1);
    GPosition* post = ecs_field(it, GPosition, 2);
    GScale* scale = ecs_field(it, GScale, 3);
    GRotation* rota = ecs_field(it, GRotation, 4);

    for (int i = 0; i < it->count; i++)
    {
        Rectangle rec = (Rectangle){ rect[i].x, rect[i].y, rect[i].width, rect[i].height };
        
        Vector2 origin = (Vector2){ 0.0f, 0.0f };
        float rotation = (rota) ? rota[i] : 0.0f;
        GPosition post_ = (post) ? post[i] : (GPosition) { 0.0f, 0.0f };
        GScale scale_ = (scale) ? scale[i] : (GScale) { 1.0f, 1.0f };

        rlPushMatrix();
        {
            rlTranslatef(post_.x, post_.y, 0.0f);
            rlRotatef(rotation, 0.0f, 0.0f, 1.0f);
            rlTranslatef(-origin.x, -origin.y, 0.0f);
            rlScalef(scale_.x, scale_.y, 1.0f);
            {
                DrawRectanglePro(rec, origin, 0.0f, rect[i].color);
            }
        }
        rlPopMatrix();
    }
}

