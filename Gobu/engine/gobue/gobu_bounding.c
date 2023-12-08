#include "gobu_bounding.h"
#include "gobu_transform.h"

ECS_COMPONENT_DECLARE(GBoundingBox);

static void GobuBoundingBox_Update(ecs_iter_t* it);

void GobuBoundingBoxImport(ecs_world_t* world)
{
    ECS_MODULE(world, GobuBoundingBox);
    ECS_IMPORT(world, GobuCustom);
    ECS_IMPORT(world, GobuTransform);

    ECS_COMPONENT_DEFINE(world, GBoundingBox);

    // ECS_SYSTEM(world, GobuBoundingBox_Update, EcsPreUpdate, GBoundingBox, GPosition, GScale, GRotation);
}

static void GobuBoundingBox_Update(ecs_iter_t* it)
{
    GBoundingBox* box = ecs_field(it, GBoundingBox, 1);
    GPosition* post = ecs_field(it, GPosition, 2);
    GScale* sca = ecs_field(it, GScale, 3);
    GRotation* rot = ecs_field(it, GRotation, 4);

    for (int i = it->count - 1; i >= 0; i--)
    {
        box[i].min = (gb_vec2_t){ post[i].x, post[i].y };
        box[i].max = (gb_vec2_t){ post[i].x + sca[i].x, post[i].y + sca[i].y };

        // Aplicar rotación al bounding box
        float cos_theta = cos(rot[i].x);
        float sin_theta = sin(rot[i].x);

        float x1 = box[i].min.x * cos_theta - box[i].min.y * sin_theta;
        float y1 = box[i].min.x * sin_theta + box[i].min.y * cos_theta;

        float x2 = box[i].max.x * cos_theta - box[i].max.y * sin_theta;
        float y2 = box[i].max.x * sin_theta + box[i].max.y * cos_theta;

        box[i].min = (gb_vec2_t){ x1, y1 };
        box[i].max = (gb_vec2_t){ x2, y2 };
    }
}

// convierta el bounding box a un rectangulo
Rectangle GBoundingBox_ToRectangle(GBoundingBox box)
{
    return (Rectangle){ box.min.x, box.min.y, box.max.x - box.min.x, box.max.y - box.min.y };
}
 