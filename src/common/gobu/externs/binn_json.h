#include "binn.h"
#include <stdbool.h>

char *APIENTRY binn_stringify(void *);

binn *APIENTRY binn_parse(char *);

binn *APIENTRY binn_load_from_file(const char *);

bool APIENTRY binn_save_to_file(binn *base, const char *filename);
