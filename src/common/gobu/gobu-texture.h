#ifndef GOBU_TEXTURE_H
#define GOBU_TEXTURE_H

#include "gobu-type.h"

go_public go_texture_t go_texture_load(const char *filename);
go_public bool go_texture_is_valid(go_texture_t texture);
go_public void go_texture_unload(go_texture_t texture);

#endif // GOBU_TEXTURE_H
