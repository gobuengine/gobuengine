#include "gobu-gfx.h"

go_internal go_gfx_context_t* go__global_gfx_ptr;

#define go_gfxptr       (*go__global_gfx_ptr)       // goGfxContext
#define go_gfxctx       (*(go_gfx_context()))       // goGfxBackendContext
#define go_fontctx      (go_gfx_font_context())     // goGfxFontContext




