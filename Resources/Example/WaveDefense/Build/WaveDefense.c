#include <stdio.h>
#include <stdbool.h>
#include "include/flecs.h"
#include "include/gobuscript.h"
component test_import {
    active: bool
}

GOBUAPI void ExportMainGame(){}