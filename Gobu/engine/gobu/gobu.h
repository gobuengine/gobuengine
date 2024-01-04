#ifndef GB_ENGINE_H
#define GB_ENGINE_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"

#include "components/gb_components.h"
#include "systems/gb_systems.h"

#include "gb_ecs.h"
#include "gb_fs.h"
#include "gb_input.h"
#include "gb_log.h"
#include "gb_path.h"
#include "gb_project.h"
#include "gb_setting.h"
#include "gb_string.h"

#ifdef __cplusplus
extern "C"
{
#endif

    ecs_world_t *gb_init(gb_app_t *app);

    void gb_app_main(ecs_world_t *world);

    void gb_app_progress(ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif
