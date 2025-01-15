#ifndef GOBU_TEXT_H
#define GOBU_TEXT_H
#include "gobu-type.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4996) // strncpy use in fontstash.h
#endif
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include "externs/fontstash/fontstash.h"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

go_public void go_font_free(go_font_t font);
go_public go_font_t go_font_get_default(void);
go_public go_font_t go_font_load_from_file(const char *filename);
go_public int go_font_text_width(const char *text);
go_public void go_font_draw_begin(go_font_t font, float font_size, go_color_t color);
go_public void go_font_draw_end(const char *text);

#endif // GOBU_TEXT_H

