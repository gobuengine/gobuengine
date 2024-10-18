#ifndef PIXI_H
#define PIXI_H
#include <stdio.h>
#include "src/raylib.h"

#ifdef PIXI_RLGL_API
#include "src/rlgl.h"
#endif

#include "src/raymath.h"

void pixi_draw_grid(int width, int height, int size);

#endif // PIXI_H

