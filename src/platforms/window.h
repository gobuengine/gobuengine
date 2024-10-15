#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "platform.h"

typedef struct _PlatformWindow
{
    const char *title;     // Window text title const pointer
    unsigned int flags;    // Configuration flags (bit based), keeps window state
    bool ready;            // Check if window has been initialized successfully
    bool fullscreen;       // Check if fullscreen mode is enabled
    bool shouldClose;      // Check if window set for closing
    bool resizedLastFrame; // Check if window has been resized last frame
    bool eventWaiting;     // Wait for events before ending frame
    bool usingFbo;         // Using FBO (RenderTexture) for rendering instead of default framebuffer

    Point position;         // Window position (required on fullscreen toggle)
    Point previousPosition; // Window previous position (required on borderless windowed toggle)
    Size display;           // Display width and height (monitor, device-screen, LCD, ...)
    Size screen;            // Screen width and height (used render area)
    Size previousScreen;    // Screen previous width and height (required on borderless windowed toggle)
    Size currentFbo;        // Current render width and height (depends on active fbo)
    Size render;            // Framebuffer width and height (render area, including black bars if required)
    Point renderOffset;     // Offset from render area (must be divided by 2)
    Size screenMin;         // Screen minimum width and height (for resizable window)
    Size screenMax;         // Screen maximum width and height (for resizable window)
    Matrix screenScale;     // Matrix to scale screen (framebuffer rendering)

    char **dropFilepaths;       // Store dropped files paths pointers (provided by GLFW)
    unsigned int dropFileCount; // Count dropped files strings

} PlatformWindow;

// Check if a file has been dropped into window
bool IsFileDropped(void);

// Load dropped filepaths
FilePathList LoadDroppedFiles(void);

// Unload dropped filepaths
void UnloadDroppedFiles(FilePathList files);

