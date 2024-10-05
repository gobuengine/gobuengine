#include "pixio_text.h"
#include "pixio_type.h"

static void observe_set_text_default(ecs_iter_t *it);

void pixio_text_moduleImport(ecs_world_t *world)
{
    ECS_MODULE(world, pixio_text_module);
    ECS_IMPORT(world, pixio_type_module);

    ecs_observer(world, {.query = {.terms = {{ecs_id(pixio_text_t)}}},
                         .events = {EcsOnAdd, EcsOnSet},
                         .callback = observe_set_text_default});
}

static void observe_set_text_default(ecs_iter_t *it)
{
    pixio_text_t *text = ecs_field(it, pixio_text_t, 0);

    for (int i = 0; i < it->count; i++)
    {
        text[i].text = (text[i].text == NULL) ? "Text" : text[i].text;
        text[i].fontSize = (text[i].fontSize == 0) ? 10 : text[i].fontSize;
        text[i].spacing = (text[i].spacing <= 0.0f) ? (text[i].fontSize / 10) : text[i].spacing;
        text[i].color = (text[i].color.a == 0) ? PURPLE : text[i].color;
    }
}
