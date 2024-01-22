#ifndef GB_PROJECT_H
#define GB_PROJECT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    bool gb_project_load_from_file_from_file(const char* filename);

    const char* gb_project_get_path(void);

    void gb_project_unload(void);

#ifdef __cplusplus
}
#endif

#endif // GB_PROJECT_H

