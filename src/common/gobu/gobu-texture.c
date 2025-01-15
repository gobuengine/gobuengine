
#include "gobu-texture.h"
#include "gobu-gfx.h"

#pragma region stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "externs/stb_image.h"
#pragma endregion

go_public go_texture_t go_texture_load(const char *filename)
{
    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, STBI_rgo_alpha);
    if (!data)
    {
        printf("Failed to load texture: %s", filename);
        return (go_texture_t){0};
    }

    go_texture_t texture = go_gfx_create_texture(data, width, height);

    stbi_image_free(data);

    return texture;
}

go_public bool go_texture_is_valid(go_texture_t texture)
{
    return texture.id != 0;
}

go_public void go_texture_unload(go_texture_t texture)
{
    go_gfx_destroy_texture(texture);
}
