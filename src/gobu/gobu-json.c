#include "gobu-json.h"
#include "gobu.h"
#include "binn/binn_json.h"

void GobuAPI_jsonInit(void)
{
    GobuContext *ctx = GobuContextGet();

    ctx->JSON.stringify = binn_serialize;
    ctx->JSON.parse = binn_serialize_load;
    ctx->JSON.parse_from_file = binn_serialize_from_file;
    ctx->JSON.stringify_from_file = binn_deserialize_from_file;
}

void GobuAPI_jsonFree(void)
{
}
