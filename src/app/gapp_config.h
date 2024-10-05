
#include "config.h"

#ifndef GAPP_CONFIG_H
#define GAPP_CONFIG_H

bool gapp_config_init();

GKeyFile *gapp_config_load_from_file(const char *filename);

bool gapp_config_get_bool(GKeyFile *keyfile, const char *group, const char *key);

#endif // GAPP_CONFIG_H

