#ifndef PIXIO_BASE_H
#define PIXIO_BASE_H

#include <stdio.h>
#include <stdint.h>
#include "pixio.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // API

    // module import
    void pixio_base_moduleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif // PIXIO_BASE_H

