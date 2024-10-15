#include "window.h"

// Close window and unload OpenGL context
void CloseWindow(void)
{

#if defined(SUPPORT_MODULE_RTEXT) && defined(SUPPORT_DEFAULT_FONT)
    UnloadFontDefault();        // WARNING: Module required: rtext
#endif

    rlglClose();                // De-init rlgl

    CORE.Render.ready = false;
    TRACELOG(LOG_INFO, "Window closed successfully");
}

// Check if window has been initialized successfully
bool IsWindowReady(void)
{
    return CORE.Render.ready;
}

// Check if a file has been dropped into window
bool IsFileDropped(void)
{
    bool result = false;

    if (CORE.Render.dropFileCount > 0) result = true;

    return result;
}

// Load dropped filepaths
FilePathList LoadDroppedFiles(void)
{
    FilePathList files = { 0 };

    files.count = CORE.Render.dropFileCount;
    files.paths = CORE.Render.dropFilepaths;

    return files;
}

// Unload dropped filepaths
void UnloadDroppedFiles(FilePathList files)
{
    // WARNING: files pointers are the same as internal ones

    if (files.count > 0)
    {
        for (unsigned int i = 0; i < files.count; i++) RL_FREE(files.paths[i]);

        RL_FREE(files.paths);

        CORE.Render.dropFileCount = 0;
        CORE.Render.dropFilepaths = NULL;
    }
}
