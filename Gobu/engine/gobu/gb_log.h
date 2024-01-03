
#include <stdio.h>
#include <stdint.h>

#if gb_log_error
#define gb_log_error
#endif

#if gb_log_warn
#define gb_log_warn
#endif

#if gb_log_success
#define gb_log_success
#endif

#if gb_log_info
#define gb_log_info
#endif

#define TF gb_strdups

#define gb_return_if_fail(expr)                          \
    if (!(expr))                                         \
    {                                                    \
        gb_log_warn(TF("Assertion '%s' failed", #expr)); \
        return;                                          \
    }
#define gb_return_val_if_fail(expr, val)                 \
    if (!(expr))                                         \
    {                                                    \
        gb_log_warn(TF("Assertion '%s' failed", #expr)); \
        return val;                                      \
    }

#define gb_if_default_value(value, default_value) value = ((value) == 0 ? (default_value) : (value))

