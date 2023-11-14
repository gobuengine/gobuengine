#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include <json-glib/json-glib.h>
#include "binn_json.h"

static binn *APIENTRY json_obj_to_binn(JsonNode *);

static const char hexdigits[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void to_hex(char *source, int size, char *dest)
{
  char *end = source + size;
  for (; source < end; source++)
  {
    unsigned char c = *source;
    *(dest++) = hexdigits[(c >> 4) & 0xf];
    *(dest++) = hexdigits[c & 0xf];
  }
  *dest = 0;
}

static void json_foreach_object(JsonObject *object, const gchar *key, JsonNode *node, binn *obj)
{
  binn_object_set_new(obj, key, json_obj_to_binn(node));
}

static void json_foreach_array(JsonArray *array, guint index_, JsonNode *node, binn *list)
{
  binn_list_add_new(list, json_obj_to_binn(node));
}

binn *APIENTRY json_obj_to_binn(JsonNode *node)
{
  binn *obj, *list;

  switch (json_node_get_node_type(node))
  {
  case JSON_NODE_OBJECT:
    obj = binn_object();
    json_object_foreach_member(json_node_get_object(node), json_foreach_object, obj);
    return obj;
    break;

  case JSON_NODE_ARRAY:
    list = binn_list();
    json_array_foreach_element(json_node_get_array(node), json_foreach_array, list);
    return list;
    break;

  case JSON_NODE_VALUE:
    switch (json_node_get_value_type(node))
    {
    case G_TYPE_STRING:
      return binn_string(g_strdup(json_node_get_string(node)), BINN_TRANSIENT);
      break;
    case G_TYPE_BOOLEAN:
      return binn_bool(json_node_get_boolean(node));
      break;
    case G_TYPE_INT64:
      return binn_int64(json_node_get_int(node));
      break;
    case G_TYPE_DOUBLE:
      return binn_double(json_node_get_double(node));
      break;
    }
    break;

  case JSON_NODE_NULL:
    return binn_null();
    break;
  }

  return NULL;
}

BINN_PRIVATE JsonBuilder *binn_to_json_obj2(binn *base, JsonBuilder *builder)
{
  if (builder == NULL)
  {
    builder = json_builder_new();
  }

  int intvalue;
  binn_iter iter;
  binn binn_value = {0};
  int id, size;
  char key[256], *ptr;
  double floatvalue;

  if (base == NULL)
    return NULL;

  switch (base->type)
  {
  case BINN_STRING:
  case BINN_DATE:
  case BINN_TIME:
  case BINN_DATETIME:
  case BINN_DECIMAL:
  case BINN_XML:
  case BINN_HTML:
  case BINN_CSS:
  case BINN_JSON:
  case BINN_JAVASCRIPT:
    json_builder_add_string_value(builder, base->ptr);
    break;

  case BINN_BLOB:
    size = (base->size * 2) + 1;
    ptr = malloc(size);
    if (!ptr)
      return NULL;
    to_hex(base->ptr, base->size, ptr);
    json_builder_add_string_value(builder, ptr);
    free(ptr);
    break;

  case BINN_INT8:
    intvalue = base->vint8;
    goto loc_integer;
  case BINN_UINT8:
    intvalue = base->vuint8;
    goto loc_integer;
  case BINN_INT16:
    intvalue = base->vint16;
    goto loc_integer;
  case BINN_UINT16:
    intvalue = base->vuint16;
    goto loc_integer;
  case BINN_INT32:
    intvalue = base->vint32;
    goto loc_integer;
  case BINN_UINT32:
    intvalue = base->vuint32;
    goto loc_integer;
  case BINN_INT64:
    intvalue = base->vint64;
    goto loc_integer;
  case BINN_UINT64:
    intvalue = base->vuint64;
  loc_integer:
    json_builder_add_int_value(builder, intvalue);
    break;

  case BINN_BOOL:
    json_builder_add_boolean_value(builder, base->vbool);
    break;

  case BINN_FLOAT:
    json_builder_add_double_value(builder, (double)base->vfloat);
    break;

  case BINN_DOUBLE:
    json_builder_add_double_value(builder, base->vdouble);
    break;

  case BINN_CURRENCYSTR:
    ptr = (char *)base->ptr;
    floatvalue = atof(ptr);
    json_builder_add_double_value(builder, floatvalue);
    break;

  case BINN_OBJECT:
    json_builder_begin_object(builder);
    binn_object_foreach(base, key, binn_value)
    {
      json_builder_set_member_name(builder, key);
      builder = binn_to_json_obj2(&binn_value, builder);
    }
    json_builder_end_object(builder);
    break;

  case BINN_MAP:
    json_builder_begin_object(builder);
    binn_map_foreach(base, id, binn_value)
    {
      json_builder_set_member_name(builder, g_strdup_printf("%d", id));
      builder = binn_to_json_obj2(&binn_value, builder);
    }
    break;

  case BINN_LIST:
    json_builder_begin_array(builder);
    binn_list_foreach(base, binn_value)
    {
      builder = binn_to_json_obj2(&binn_value, builder);
    }
    json_builder_end_array(builder);
    break;

  case BINN_NULL:
    json_builder_add_null_value(builder);
    break;

  default:
    return NULL;
    break;
  }

  return builder;
}

binn *APIENTRY binn_serialize_from_file(const char *filename)
{
  JsonParser *parser = json_parser_new();
  json_parser_load_from_file(parser, filename, NULL);
  return json_obj_to_binn(json_parser_get_root(parser));
}

bool APIENTRY binn_deserialize_from_file(binn *base, const char *filename)
{
  char *buffer = binn_serialize(base);
  return (bool)g_file_set_contents(filename, buffer, strlen(buffer), NULL);
}

binn *APIENTRY binn_serialize_load(char *str)
{
  JsonParser *parser = json_parser_new();
  if (json_parser_load_from_data(parser, str, strlen(str), NULL))
  {
    return json_obj_to_binn(json_parser_get_root(parser));
  }
  return NULL;
}

char *APIENTRY binn_serialize(void *base)
{
  JsonBuilder *builder = binn_to_json_obj2(base, NULL);

  JsonGenerator *gen = json_generator_new();
  JsonNode *root = json_builder_get_root(builder);
  json_generator_set_root(gen, root);

  char *str = json_generator_to_data(gen, NULL);
  g_object_unref(builder);
  return str;
}
