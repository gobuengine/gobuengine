#include "binn.h"

char *APIENTRY binn_serialize(void *);

binn *APIENTRY binn_serialize_from_file(const char *);

binn *APIENTRY binn_serialize_load(char *);

gboolean APIENTRY binn_deserialize_from_file(binn *base, const char *filename);
