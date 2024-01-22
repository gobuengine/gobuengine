
#include <stdio.h>
#include <stdint.h>

#if gb_log_error
#define gb_log_error
#else 
#define gb_log_error g_error
#endif

#if gb_log_warn
#define gb_log_warn
#else 
#define gb_log_warn g_warning
#endif

#if gb_log_success
#define gb_log_success
#else 
#define gb_log_success g_message
#endif

#if gb_log_info
#define gb_log_info
#else 
#define gb_log_info g_info
#endif

#define TF gb_strdups

#define gb_return_if_fail(expr)                          \
    if (!(expr))                                         \
    {                                                    \
        gb_log_warn("Assertion '%s' warn", #expr); \
        return;                                          \
    }

#define gb_return_val_if_fail(expr, val)                 \
    if (!(expr))                                         \
    {                                                    \
        gb_log_warn("Assertion '%s' warn", #expr); \
        return val;                                      \
    }

#define gb_if_default_value(value, default_value) value = ((value) == 0 ? (default_value) : (value))

