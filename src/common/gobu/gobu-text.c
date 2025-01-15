#include <stdio.h> // malloc, free, fopen, fclose, ftell, fseek, fread
#include <string.h>
#include <math.h>

#include "gobu-gfx-internal.h"

#include "gobu-util.h"
#include "gobu-gfx.h"

#define FONTSTASH_IMPLEMENTATION
#include "gobu-text.h"

go_public void go_font_free(go_font_t font)
{
}

go_public go_font_t go_font_get_default(void)
{
    return go_gfx_font_default();
}

go_public go_font_t go_font_load_from_file(const char *filename)
{
    go_font_t font_data = {0};
    font_data.id = fonsAddFont(go_fontctx, go_util_name(filename, TRUE), filename);
    return font_data;
}

go_public int go_font_text_width(const char *text)
{
    return fonsTextBounds(go_fontctx, 0, 0, text, 0, NULL);
}

go_public void go_font_draw_begin(go_font_t font, float font_size, go_color_t color)
{
    fonsClearState(go_fontctx);
    fonsSetFont(go_fontctx, font.id);
    fonsSetSize(go_fontctx, font_size);
    fonsSetColor(go_fontctx, go_color_to_uint(color));
}

go_public void go_font_draw_end(const char *text)
{
    fonsDrawText(go_fontctx, 0, 0, text, 0);
}
