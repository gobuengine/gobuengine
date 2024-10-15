/**********************************************************************************************
*
*   rcore - Graphic device/context management and input management
*
*   CONFIGURATION:
*       #define SUPPORT_DEFAULT_FONT (default)
*           Default font is loaded on window initialization to be available for the user to render simple text.
*           NOTE: If enabled, uses external module functions to load default raylib font (module: text)
*
*       #define SUPPORT_BUSY_WAIT_LOOP
*           Use busy wait loop for timing sync, if not defined, a high-resolution timer is setup and used
*
*       #define SUPPORT_PARTIALBUSY_WAIT_LOOP
*           Use a partial-busy wait loop, in this case frame sleeps for most of the time and runs a busy-wait-loop at the end
*
*   DEPENDENCIES:
*       raymath  - 2D math functionality (Vector2, Vector3, Matrix, Quaternion)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5) and contributors
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

//----------------------------------------------------------------------------------
// Feature Test Macros required for this module
//----------------------------------------------------------------------------------
#if (defined(__linux__) || defined(PLATFORM_WEB)) && (_XOPEN_SOURCE < 500)
    #undef _XOPEN_SOURCE
    #define _XOPEN_SOURCE 500 // Required for: readlink if compiled with c99 without gnu ext.
#endif

#if (defined(__linux__) || defined(PLATFORM_WEB)) && (_POSIX_C_SOURCE < 199309L)
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199309L // Required for: CLOCK_MONOTONIC if compiled with c99 without gnu ext.
#endif

#include "raylib.h"                 // Declares module functions

// Check if config flags have been externally provided on compilation line
#if !defined(EXTERNAL_CONFIG_FLAGS)
    #include "config.h"             // Defines module configuration flags
#endif

#include "utils.h"                  // Required for: TRACELOG() macros

#include <stdlib.h>                 // Required for: srand(), rand(), atexit()
#include <stdio.h>                  // Required for: sprintf() [Used in OpenURL()]
#include <string.h>                 // Required for: strrchr(), strcmp(), strlen(), memset()
#include <time.h>                   // Required for: time() [Used in InitTimer()]
#include <math.h>                   // Required for: tan() [Used in BeginMode3D()], atan2f() [Used in LoadVrStereoConfig()]

#define RLGL_IMPLEMENTATION
#include "rlgl.h"                   // OpenGL abstraction layer to OpenGL 1.1, 3.3+ or ES2

#define RAYMATH_IMPLEMENTATION
#include "raymath.h"                // Vector2, Vector3, Quaternion and Matrix functionality

#if defined(SUPPORT_COMPRESSION_API)
    #define SINFL_IMPLEMENTATION
    #define SINFL_NO_SIMD
    #include "external/sinfl.h"     // Deflate (RFC 1951) decompressor

    #define SDEFL_IMPLEMENTATION
    #include "external/sdefl.h"     // Deflate (RFC 1951) compressor
#endif

#if defined(SUPPORT_RPRAND_GENERATOR)
    #define RPRAND_IMPLEMENTATION
    #include "external/rprand.h"
#endif

#if defined(__linux__) && !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
#endif

// Platform specific defines to handle GetApplicationDirectory()
#if (defined(_WIN32)) || (defined(_MSC_VER))
    #ifndef MAX_PATH
        #define MAX_PATH 1025
    #endif
__declspec(dllimport) unsigned long __stdcall GetModuleFileNameA(void *hModule, void *lpFilename, unsigned long nSize);
__declspec(dllimport) unsigned long __stdcall GetModuleFileNameW(void *hModule, void *lpFilename, unsigned long nSize);
__declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int cp, unsigned long flags, void *widestr, int cchwide, void *str, int cbmb, void *defchar, int *used_default);
unsigned int __stdcall timeBeginPeriod(unsigned int uPeriod);
unsigned int __stdcall timeEndPeriod(unsigned int uPeriod);
#elif defined(__linux__)
    #include <unistd.h>
#elif defined(__FreeBSD__)
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #include <unistd.h>
#elif defined(__APPLE__)
    #include <sys/syslimits.h>
    #include <mach-o/dyld.h>
#endif // OSs

#define _CRT_INTERNAL_NONSTDC_NAMES  1
#include <sys/stat.h>               // Required for: stat(), S_ISREG [Used in GetFileModTime(), IsFilePath()]

#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
    #define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

#if defined(_WIN32) && (defined(_MSC_VER) || defined(__TINYC__))
    #define DIRENT_MALLOC RL_MALLOC
    #define DIRENT_FREE RL_FREE

    #include "external/dirent.h"    // Required for: DIR, opendir(), closedir() [Used in LoadDirectoryFiles()]
#else
    #include <dirent.h>             // Required for: DIR, opendir(), closedir() [Used in LoadDirectoryFiles()]
#endif

#if defined(_WIN32)
    #include <io.h>                 // Required for: _access() [Used in FileExists()]
    #include <direct.h>             // Required for: _getch(), _chdir(), _mkdir()
    #define GETCWD _getcwd          // NOTE: MSDN recommends not to use getcwd(), chdir()
    #define CHDIR _chdir
    #define MKDIR(dir) _mkdir(dir)
#else
    #include <unistd.h>             // Required for: getch(), chdir(), mkdir(), access()
    #define GETCWD getcwd
    #define CHDIR chdir
    #define MKDIR(dir) mkdir(dir, 0777)
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#ifndef MAX_FILEPATH_CAPACITY
    #define MAX_FILEPATH_CAPACITY       8192        // Maximum capacity for filepath
#endif
#ifndef MAX_FILEPATH_LENGTH
    #if defined(_WIN32)
        #define MAX_FILEPATH_LENGTH      256        // On Win32, MAX_PATH = 260 (limits.h) but Windows 10, Version 1607 enables long paths...
    #else
        #define MAX_FILEPATH_LENGTH     4096        // On Linux, PATH_MAX = 4096 by default (limits.h)
    #endif
#endif

#ifndef MAX_KEYBOARD_KEYS
    #define MAX_KEYBOARD_KEYS            512        // Maximum number of keyboard keys supported
#endif
#ifndef MAX_MOUSE_BUTTONS
    #define MAX_MOUSE_BUTTONS              8        // Maximum number of mouse buttons supported
#endif
#ifndef MAX_GAMEPADS
    #define MAX_GAMEPADS                   4        // Maximum number of gamepads supported
#endif
#ifndef MAX_GAMEPAD_AXIS
    #define MAX_GAMEPAD_AXIS               8        // Maximum number of axis supported (per gamepad)
#endif
#ifndef MAX_GAMEPAD_BUTTONS
    #define MAX_GAMEPAD_BUTTONS           32        // Maximum number of buttons supported (per gamepad)
#endif
#ifndef MAX_GAMEPAD_VIBRATION_TIME
    #define MAX_GAMEPAD_VIBRATION_TIME     2.0f     // Maximum vibration time in seconds
#endif
#ifndef MAX_TOUCH_POINTS
    #define MAX_TOUCH_POINTS               8        // Maximum number of touch points supported
#endif
#ifndef MAX_KEY_PRESSED_QUEUE
    #define MAX_KEY_PRESSED_QUEUE         16        // Maximum number of keys in the key input queue
#endif
#ifndef MAX_CHAR_PRESSED_QUEUE
    #define MAX_CHAR_PRESSED_QUEUE        16        // Maximum number of characters in the char input queue
#endif

#ifndef MAX_DECOMPRESSION_SIZE
    #define MAX_DECOMPRESSION_SIZE        64        // Maximum size allocated for decompression in MB
#endif

#ifndef MAX_AUTOMATION_EVENTS
    #define MAX_AUTOMATION_EVENTS      16384        // Maximum number of automation events to record
#endif

#ifndef DIRECTORY_FILTER_TAG
    #define DIRECTORY_FILTER_TAG       "DIR"        // Name tag used to request directory inclusion on directory scan
#endif                                              // NOTE: Used in ScanDirectoryFiles(), ScanDirectoryFilesRecursively() and LoadDirectoryFilesEx()

// Flags operation macros
#define FLAG_SET(n, f) ((n) |= (f))
#define FLAG_CLEAR(n, f) ((n) &= ~(f))
#define FLAG_TOGGLE(n, f) ((n) ^= (f))
#define FLAG_CHECK(n, f) ((n) & (f))

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct { int x; int y; } Point;
typedef struct { unsigned int width; unsigned int height; } Size;

// Core global state context data
typedef struct CoreData {
    struct {
        bool usingFbo;                      // Using FBO (RenderTexture) for rendering instead of default framebuffer
        
        Size display;                       // Display width and height (monitor, device-screen, LCD, ...)
        Size screen;                        // Screen width and height (used render area)
        Size currentFbo;                    // Current render width and height (depends on active fbo)
        Size render;                        // Framebuffer width and height (render area, including black bars if required)
        Point renderOffset;                 // Offset from render area (must be divided by 2)
        Matrix screenScale;                 // Matrix to scale screen (framebuffer rendering)
    } Render;
    struct {
        const char *basePath;               // Base path for data storage

    } Storage;
    struct {
        struct {
            int exitKey;                    // Default exit key
            char currentKeyState[MAX_KEYBOARD_KEYS];        // Registers current frame key state
            char previousKeyState[MAX_KEYBOARD_KEYS];       // Registers previous frame key state

            // NOTE: Since key press logic involves comparing prev vs cur key state, we need to handle key repeats specially
            char keyRepeatInFrame[MAX_KEYBOARD_KEYS];       // Registers key repeats for current frame

            int keyPressedQueue[MAX_KEY_PRESSED_QUEUE];     // Input keys queue
            int keyPressedQueueCount;       // Input keys queue count

            int charPressedQueue[MAX_CHAR_PRESSED_QUEUE];   // Input characters queue (unicode)
            int charPressedQueueCount;      // Input characters queue count

        } Keyboard;
        struct {
            Vector2 offset;                 // Mouse offset
            Vector2 scale;                  // Mouse scaling
            Vector2 currentPosition;        // Mouse position on screen
            Vector2 previousPosition;       // Previous mouse position

            int cursor;                     // Tracks current mouse cursor
            bool cursorHidden;              // Track if cursor is hidden
            bool cursorOnScreen;            // Tracks if cursor is inside client area

            char currentButtonState[MAX_MOUSE_BUTTONS];     // Registers current mouse button state
            char previousButtonState[MAX_MOUSE_BUTTONS];    // Registers previous mouse button state
            Vector2 currentWheelMove;       // Registers current mouse wheel variation
            Vector2 previousWheelMove;      // Registers previous mouse wheel variation

        } Mouse;
        struct {
            int pointCount;                             // Number of touch points active
            int pointId[MAX_TOUCH_POINTS];              // Point identifiers
            Vector2 position[MAX_TOUCH_POINTS];         // Touch position on screen
            char currentTouchState[MAX_TOUCH_POINTS];   // Registers current touch state
            char previousTouchState[MAX_TOUCH_POINTS];  // Registers previous touch state

        } Touch;
        struct {
            int lastButtonPressed;          // Register last gamepad button pressed
            int axisCount[MAX_GAMEPADS];    // Register number of available gamepad axis
            bool ready[MAX_GAMEPADS];       // Flag to know if gamepad is ready
            char name[MAX_GAMEPADS][64];    // Gamepad name holder
            char currentButtonState[MAX_GAMEPADS][MAX_GAMEPAD_BUTTONS];     // Current gamepad buttons state
            char previousButtonState[MAX_GAMEPADS][MAX_GAMEPAD_BUTTONS];    // Previous gamepad buttons state
            float axisState[MAX_GAMEPADS][MAX_GAMEPAD_AXIS];                // Gamepad axis state

        } Gamepad;
    } Input;
    struct {
        double current;                     // Current time measure
        double previous;                    // Previous time measure
        double update;                      // Time measure for frame update
        double draw;                        // Time measure for frame draw
        double frame;                       // Time measure for one frame
        double target;                      // Desired time for one frame, if 0 not applied
        unsigned long long int base;        // Base time measure for hi-res timer (PLATFORM_ANDROID, PLATFORM_DRM)
        unsigned int frameCounter;          // Frame counter

    } Time;
} CoreData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
RLAPI const char *raylib_version = RAYLIB_VERSION;  // raylib version exported symbol, required for some bindings

CoreData CORE = { 0 };                      // Global CORE state context

// Flag to note GPU acceleration is available,
// referenced from other modules to support GPU data loading
// NOTE: Useful to allow Texture, RenderTexture, Font.texture, Mesh.vaoId/vboId, Shader loading
bool isGpuReady = false;

//-----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Module Functions Declaration
// NOTE: Those functions are common for all platforms!
//----------------------------------------------------------------------------------

#if defined(SUPPORT_MODULE_RTEXT) && defined(SUPPORT_DEFAULT_FONT)
extern void LoadFontDefault(void);      // [Module: text] Loads default font on InitWindow()
extern void UnloadFontDefault(void);    // [Module: text] Unloads default font from GPU memory
#endif

static void InitTimer(void);                                // Initialize timer, hi-resolution if available (required by InitPlatform())
static void SetupFramebuffer(int width, int height);        // Setup main framebuffer (required by InitPlatform())
static void SetupViewport(int width, int height);           // Set viewport for a provided width and height

static void ScanDirectoryFiles(const char *basePath, FilePathList *list, const char *filter);   // Scan all files and directories in a base path
static void ScanDirectoryFilesRecursively(const char *basePath, FilePathList *list, const char *filter);  // Scan all files and directories recursively from a base path

#if defined(_WIN32)
// NOTE: We declare Sleep() function symbol to avoid including windows.h (kernel32.lib linkage required)
void __stdcall Sleep(unsigned long msTimeout);              // Required for: WaitTime()
#endif

#if !defined(SUPPORT_MODULE_RTEXT)
const char *TextFormat(const char *text, ...);              // Formatting of text with variables to 'embed'
#endif // !SUPPORT_MODULE_RTEXT

//----------------------------------------------------------------------------------
// Module Functions Definition: Graphics Device
//----------------------------------------------------------------------------------

// Initialize window and OpenGL context
// NOTE: data parameter could be used to pass any kind of required data to the initialization
void pixi_init(int width, int height)
{
    // Initialize window data
    CORE.Render.screen.width = width;
    CORE.Render.screen.height = height;
    CORE.Render.currentFbo.width = width;
    CORE.Render.currentFbo.height = height;
    CORE.Render.screenScale = MatrixIdentity();     // No draw scaling required by default

    // Initialize global input state
    memset(&CORE.Input, 0, sizeof(CORE.Input));     // Reset CORE.Input structure to 0
    CORE.Input.Keyboard.exitKey = KEY_ESCAPE;
    CORE.Input.Mouse.scale = (Vector2){ 1.0f, 1.0f };
    CORE.Input.Mouse.cursor = MOUSE_CURSOR_ARROW;
    CORE.Input.Gamepad.lastButtonPressed = GAMEPAD_BUTTON_UNKNOWN;

    rlLoadExtensions(NULL);

    // Initialize rlgl default data (buffers and shaders)
    // NOTE: CORE.Render.currentFbo.width and CORE.Render.currentFbo.height not used, just stored as globals in rlgl
    rlglInit(CORE.Render.currentFbo.width, CORE.Render.currentFbo.height);
    isGpuReady = true; // Flag to note GPU has been initialized successfully

    // Setup default viewport
    SetupViewport(CORE.Render.currentFbo.width, CORE.Render.currentFbo.height);

#if defined(SUPPORT_MODULE_RTEXT)
    #if defined(SUPPORT_DEFAULT_FONT)
        // Load default font
        // WARNING: External function: Module required: rtext
        LoadFontDefault();
        #if defined(SUPPORT_MODULE_RSHAPES)
        // Set font white rectangle for shapes drawing, so shapes and text can be batched together
        // WARNING: rshapes module is required, if not available, default internal white rectangle is used
        Rectangle rec = GetFontDefault().recs[95];
        SetShapesTexture(GetFontDefault().texture, (Rectangle){ rec.x + 2, rec.y + 2, 1, 1 });
        #endif
    #endif
#else
    #if defined(SUPPORT_MODULE_RSHAPES)
    // Set default texture and rectangle to be used for shapes drawing
    // NOTE: rlgl default texture is a 1x1 pixel UNCOMPRESSED_R8G8B8A8
    Texture2D texture = { rlGetTextureIdDefault(), 1, 1, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
    SetShapesTexture(texture, (Rectangle){ 0.0f, 0.0f, 1.0f, 1.0f });    // WARNING: Module required: rshapes
    #endif
#endif

    CORE.Time.frameCounter = 0;

    // Initialize random seed
    SetRandomSeed((unsigned int)time(NULL));
    
    TRACELOG(LOG_INFO, "SYSTEM: Working Directory: %s", GetWorkingDirectory());
}

// Close window and unload OpenGL context
void pixi_shutdown(void)
{

#if defined(SUPPORT_MODULE_RTEXT) && defined(SUPPORT_DEFAULT_FONT)
    UnloadFontDefault();        // WARNING: Module required: rtext
#endif

    rlglClose();                // De-init rlgl

    TRACELOG(LOG_INFO, "Window closed successfully");
}

Vector2 pixi_get_dpi(void)
{
    Vector2 scale = { 1.0f, 1.0f };
    return scale;
}

// Get current screen width
int pixi_screen_width(void)
{
    return CORE.Render.screen.width;
}

// Get current screen height
int pixi_screen_height(void)
{
    return CORE.Render.screen.height;
}

// Get current render width which is equal to screen width*dpi scale
int pixi_render_width(void)
{
    int width = 0;
#if defined(__APPLE__)
    Vector2 scale = pixi_get_dpi();
    width = (int)((float)CORE.Render.render.width*scale.x);
#else
    width = CORE.Render.render.width;
#endif
    return width;
}

// Get current screen height which is equal to screen height*dpi scale
int pixi_render_height(void)
{
    int height = 0;
#if defined(__APPLE__)
    Vector2 scale = pixi_get_dpi();
    height = (int)((float)CORE.Render.render.height*scale.y);
#else
    height = CORE.Render.render.height;
#endif
    return height;
}

// Check if cursor is not visible
bool pixi_input_is_cursor_hidden(void)
{
    return CORE.Input.Mouse.cursorHidden;
}

// Check if cursor is on the current screen
bool pixi_input_is_cursor_on_screen(void)
{
    return CORE.Input.Mouse.cursorOnScreen;
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Screen Drawing
//----------------------------------------------------------------------------------

// Set background color (framebuffer clear color)
void pixi_render_clear_color(Color color)
{
    rlClearColor(color.r, color.g, color.b, color.a);   // Set clear color
    rlClearScreenBuffers();                             // Clear current framebuffers
}

// Setup canvas (framebuffer) to start drawing
void pixi_render_begin(void)
{
    // WARNING: Previously to pixi_render_begin() other render textures drawing could happen,
    // consequently the measure for update vs draw is not accurate (only the total frame time is accurate)

    CORE.Time.current = GetTime();      // Number of elapsed seconds since InitTimer()
    CORE.Time.update = CORE.Time.current - CORE.Time.previous;
    CORE.Time.previous = CORE.Time.current;

    rlLoadIdentity();                   // Reset current matrix (modelview)
    rlMultMatrixf(MatrixToFloat(CORE.Render.screenScale)); // Apply screen scaling

    //rlTranslatef(0.375, 0.375, 0);    // HACK to have 2D pixel-perfect drawing on OpenGL 1.1
                                        // NOTE: Not required with OpenGL 3.3+
}

// End canvas drawing and swap buffers (double buffering)
void pixi_render_end(void)
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

#if !defined(SUPPORT_CUSTOM_FRAME_CONTROL)
    //SwapScreenBuffer();                  // Copy back buffer to front buffer (screen)

    // Frame time control system
    CORE.Time.current = GetTime();
    CORE.Time.draw = CORE.Time.current - CORE.Time.previous;
    CORE.Time.previous = CORE.Time.current;

    CORE.Time.frame = CORE.Time.update + CORE.Time.draw;

    // Wait for some milliseconds...
    if (CORE.Time.frame < CORE.Time.target)
    {
        WaitTime(CORE.Time.target - CORE.Time.frame);

        CORE.Time.current = GetTime();
        double waitTime = CORE.Time.current - CORE.Time.previous;
        CORE.Time.previous = CORE.Time.current;

        CORE.Time.frame += waitTime;    // Total frame time: update + draw + wait
    }

    //PollInputEvents();      // Poll user events (before next frame update)
#endif

    CORE.Time.frameCounter++;
}

// Initialize 2D mode with custom camera (2D)
void BeginMode2D(Camera2D camera)
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlLoadIdentity();               // Reset current matrix (modelview)

    // Apply 2d camera transformation to modelview
    rlMultMatrixf(MatrixToFloat(GetCameraMatrix2D(camera)));
}

// Ends 2D mode with custom camera
void EndMode2D(void)
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlLoadIdentity();               // Reset current matrix (modelview)

    if (rlGetActiveFramebuffer() == 0) rlMultMatrixf(MatrixToFloat(CORE.Render.screenScale)); // Apply screen scaling if required
}

// Initializes render texture for drawing
void BeginTextureMode(RenderTexture2D target)
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlEnableFramebuffer(target.id); // Enable render target

    // Set viewport and RLGL internal framebuffer size
    rlViewport(0, 0, target.texture.width, target.texture.height);
    rlSetFramebufferWidth(target.texture.width);
    rlSetFramebufferHeight(target.texture.height);

    rlMatrixMode(RL_PROJECTION);    // Switch to projection matrix
    rlLoadIdentity();               // Reset current matrix (projection)

    // Set orthographic projection to current framebuffer size
    // NOTE: Configured top-left corner as (0, 0)
    rlOrtho(0, target.texture.width, target.texture.height, 0, 0.0f, 1.0f);

    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();               // Reset current matrix (modelview)

    //rlScalef(0.0f, -1.0f, 0.0f);  // Flip Y-drawing (?)

    // Setup current width/height for proper aspect ratio
    // calculation when using BeginMode3D()
    CORE.Render.currentFbo.width = target.texture.width;
    CORE.Render.currentFbo.height = target.texture.height;
    CORE.Render.usingFbo = true;
}

// Ends drawing to render texture
void EndTextureMode(void)
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlDisableFramebuffer();         // Disable render target (fbo)

    // Set viewport to default framebuffer size
    SetupViewport(CORE.Render.render.width, CORE.Render.render.height);

    // Go back to the modelview state from pixi_render_begin since we are back to the default FBO
    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();               // Reset current matrix (modelview)
    rlMultMatrixf(MatrixToFloat(CORE.Render.screenScale)); // Apply screen scaling if required

    // Reset current fbo to screen size
    CORE.Render.currentFbo.width = CORE.Render.render.width;
    CORE.Render.currentFbo.height = CORE.Render.render.height;
    CORE.Render.usingFbo = false;
}

// Begin custom shader mode
void BeginShaderMode(Shader shader)
{
    rlSetShader(shader.id, shader.locs);
}

// End custom shader mode (returns to default shader)
void EndShaderMode(void)
{
    rlSetShader(rlGetShaderIdDefault(), rlGetShaderLocsDefault());
}

// Begin blending mode (alpha, additive, multiplied, subtract, custom)
// NOTE: Blend modes supported are enumerated in BlendMode enum
void BeginBlendMode(int mode)
{
    rlSetBlendMode(mode);
}

// End blending mode (reset to default: alpha blending)
void EndBlendMode(void)
{
    rlSetBlendMode(BLEND_ALPHA);
}

// Begin scissor mode (define screen area for following drawing)
// NOTE: Scissor rec refers to bottom-left corner, we change it to upper-left
void BeginScissorMode(int x, int y, int width, int height)
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlEnableScissorTest();

#if defined(__APPLE__)
    if (!CORE.Render.usingFbo)
    {
        Vector2 scale = pixi_get_dpi();
        rlScissor((int)(x*scale.x), (int)(pixi_screen_height()*scale.y - (((y + height)*scale.y))), (int)(width*scale.x), (int)(height*scale.y));
    }
#else
    if (!CORE.Render.usingFbo)
    {
        Vector2 scale = pixi_get_dpi();
        if (scale.x >= 0 && scale.y >= 0)
            rlScissor((int)(x*scale.x), (int)(CORE.Render.currentFbo.height - (y + height)*scale.y), (int)(width*scale.x), (int)(height*scale.y));
    }
#endif
    else
    {
        rlScissor(x, CORE.Render.currentFbo.height - (y + height), width, height);
    }
}

// End scissor mode
void EndScissorMode(void)
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch
    rlDisableScissorTest();
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Shaders Management
//----------------------------------------------------------------------------------

// Load shader from files and bind default locations
// NOTE: If shader string is NULL, using default vertex/fragment shaders
Shader LoadShader(const char *vsFileName, const char *fsFileName)
{
    Shader shader = { 0 };

    char *vShaderStr = NULL;
    char *fShaderStr = NULL;

    if (vsFileName != NULL) vShaderStr = LoadFileText(vsFileName);
    if (fsFileName != NULL) fShaderStr = LoadFileText(fsFileName);

    shader = LoadShaderFromMemory(vShaderStr, fShaderStr);

    UnloadFileText(vShaderStr);
    UnloadFileText(fShaderStr);

    return shader;
}

// Load shader from code strings and bind default locations
Shader LoadShaderFromMemory(const char *vsCode, const char *fsCode)
{
    Shader shader = { 0 };

    shader.id = rlLoadShaderCode(vsCode, fsCode);

    // After shader loading, we TRY to set default location names
    if (shader.id > 0)
    {
        // Default shader attribute locations have been binded before linking:
        //          vertex position location    = 0
        //          vertex texcoord location    = 1
        //          vertex normal location      = 2
        //          vertex color location       = 3
        //          vertex tangent location     = 4
        //          vertex texcoord2 location   = 5
        //          vertex boneIds location     = 6
        //          vertex boneWeights location = 7

        // NOTE: If any location is not found, loc point becomes -1

        shader.locs = (int *)RL_CALLOC(RL_MAX_SHADER_LOCATIONS, sizeof(int));

        // All locations reset to -1 (no location)
        for (int i = 0; i < RL_MAX_SHADER_LOCATIONS; i++) shader.locs[i] = -1;

        // Get handles to GLSL input attribute locations
        shader.locs[SHADER_LOC_VERTEX_POSITION] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION);
        shader.locs[SHADER_LOC_VERTEX_TEXCOORD01] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD);
        shader.locs[SHADER_LOC_VERTEX_TEXCOORD02] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2);
        shader.locs[SHADER_LOC_VERTEX_NORMAL] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL);
        shader.locs[SHADER_LOC_VERTEX_TANGENT] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT);
        shader.locs[SHADER_LOC_VERTEX_COLOR] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR);
        shader.locs[SHADER_LOC_VERTEX_BONEIDS] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_BONEIDS);
        shader.locs[SHADER_LOC_VERTEX_BONEWEIGHTS] = rlGetLocationAttrib(shader.id, RL_DEFAULT_SHADER_ATTRIB_NAME_BONEWEIGHTS);

        // Get handles to GLSL uniform locations (vertex shader)
        shader.locs[SHADER_LOC_MATRIX_MVP] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_MVP);
        shader.locs[SHADER_LOC_MATRIX_VIEW] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_VIEW);
        shader.locs[SHADER_LOC_MATRIX_PROJECTION] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION);
        shader.locs[SHADER_LOC_MATRIX_MODEL] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_MODEL);
        shader.locs[SHADER_LOC_MATRIX_NORMAL] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL);
        shader.locs[SHADER_LOC_BONE_MATRICES] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_BONE_MATRICES);

        // Get handles to GLSL uniform locations (fragment shader)
        shader.locs[SHADER_LOC_COLOR_DIFFUSE] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_UNIFORM_NAME_COLOR);
        shader.locs[SHADER_LOC_MAP_DIFFUSE] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0);  // SHADER_LOC_MAP_ALBEDO
        shader.locs[SHADER_LOC_MAP_SPECULAR] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1); // SHADER_LOC_MAP_METALNESS
        shader.locs[SHADER_LOC_MAP_NORMAL] = rlGetLocationUniform(shader.id, RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2);
    }

    return shader;
}

// Check if a shader is ready
bool IsShaderReady(Shader shader)
{
    return ((shader.id > 0) &&          // Validate shader id (loaded successfully)
            (shader.locs != NULL));     // Validate memory has been allocated for default shader locations

    // The following locations are tried to be set automatically (locs[i] >= 0),
    // any of them can be checked for validation but the only mandatory one is, afaik, SHADER_LOC_VERTEX_POSITION
    // NOTE: Users can also setup manually their own attributes/uniforms and do not used the default raylib ones

    // Vertex shader attribute locations (default)
    // shader.locs[SHADER_LOC_VERTEX_POSITION]      // Set by default internal shader
    // shader.locs[SHADER_LOC_VERTEX_TEXCOORD01]    // Set by default internal shader
    // shader.locs[SHADER_LOC_VERTEX_TEXCOORD02]
    // shader.locs[SHADER_LOC_VERTEX_NORMAL]
    // shader.locs[SHADER_LOC_VERTEX_TANGENT]
    // shader.locs[SHADER_LOC_VERTEX_COLOR]         // Set by default internal shader

    // Vertex shader uniform locations (default)
    // shader.locs[SHADER_LOC_MATRIX_MVP]           // Set by default internal shader
    // shader.locs[SHADER_LOC_MATRIX_VIEW]
    // shader.locs[SHADER_LOC_MATRIX_PROJECTION]
    // shader.locs[SHADER_LOC_MATRIX_MODEL]
    // shader.locs[SHADER_LOC_MATRIX_NORMAL]

    // Fragment shader uniform locations (default)
    // shader.locs[SHADER_LOC_COLOR_DIFFUSE]        // Set by default internal shader
    // shader.locs[SHADER_LOC_MAP_DIFFUSE]          // Set by default internal shader
    // shader.locs[SHADER_LOC_MAP_SPECULAR]
    // shader.locs[SHADER_LOC_MAP_NORMAL]
}

// Unload shader from GPU memory (VRAM)
void UnloadShader(Shader shader)
{
    if (shader.id != rlGetShaderIdDefault())
    {
        rlUnloadShaderProgram(shader.id);

        // NOTE: If shader loading failed, it should be 0
        RL_FREE(shader.locs);
    }
}

// Get shader uniform location
int GetShaderLocation(Shader shader, const char *uniformName)
{
    return rlGetLocationUniform(shader.id, uniformName);
}

// Get shader attribute location
int GetShaderLocationAttrib(Shader shader, const char *attribName)
{
    return rlGetLocationAttrib(shader.id, attribName);
}

// Set shader uniform value
void SetShaderValue(Shader shader, int locIndex, const void *value, int uniformType)
{
    SetShaderValueV(shader, locIndex, value, uniformType, 1);
}

// Set shader uniform value vector
void SetShaderValueV(Shader shader, int locIndex, const void *value, int uniformType, int count)
{
    if (locIndex > -1)
    {
        rlEnableShader(shader.id);
        rlSetUniform(locIndex, value, uniformType, count);
        //rlDisableShader();      // Avoid resetting current shader program, in case other uniforms are set
    }
}

// Set shader uniform value (matrix 4x4)
void SetShaderValueMatrix(Shader shader, int locIndex, Matrix mat)
{
    if (locIndex > -1)
    {
        rlEnableShader(shader.id);
        rlSetUniformMatrix(locIndex, mat);
        //rlDisableShader();
    }
}

// Set shader uniform value for texture
void SetShaderValueTexture(Shader shader, int locIndex, Texture2D texture)
{
    if (locIndex > -1)
    {
        rlEnableShader(shader.id);
        rlSetUniformSampler(locIndex, texture.id);
        //rlDisableShader();
    }
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Screen-space Queries
//----------------------------------------------------------------------------------

// Get camera 2d transform matrix
Matrix GetCameraMatrix2D(Camera2D camera)
{
    Matrix matTransform = { 0 };
    // The camera in world-space is set by
    //   1. Move it to target
    //   2. Rotate by -rotation and scale by (1/zoom)
    //      When setting higher scale, it's more intuitive for the world to become bigger (= camera become smaller),
    //      not for the camera getting bigger, hence the invert. Same deal with rotation
    //   3. Move it by (-offset);
    //      Offset defines target transform relative to screen, but since we're effectively "moving" screen (camera)
    //      we need to do it into opposite direction (inverse transform)

    // Having camera transform in world-space, inverse of it gives the modelview transform
    // Since (A*B*C)' = C'*B'*A', the modelview is
    //   1. Move to offset
    //   2. Rotate and Scale
    //   3. Move by -target
    Matrix matOrigin = MatrixTranslate(-camera.target.x, -camera.target.y, 0.0f);
    Matrix matRotation = MatrixRotate((Vector3){ 0.0f, 0.0f, 1.0f }, camera.rotation*DEG2RAD);
    Matrix matScale = MatrixScale(camera.zoom, camera.zoom, 1.0f);
    Matrix matTranslation = MatrixTranslate(camera.offset.x, camera.offset.y, 0.0f);

    matTransform = MatrixMultiply(MatrixMultiply(matOrigin, MatrixMultiply(matScale, matRotation)), matTranslation);

    return matTransform;
}

// Get the screen space position for a 2d camera world space position
Vector2 GetWorldToScreen2D(Vector2 position, Camera2D camera)
{
    Matrix matCamera = GetCameraMatrix2D(camera);
    Vector3 transform = Vector3Transform((Vector3){ position.x, position.y, 0 }, matCamera);

    return (Vector2){ transform.x, transform.y };
}

// Get the world space position for a 2d camera screen space position
Vector2 GetScreenToWorld2D(Vector2 position, Camera2D camera)
{
    Matrix invMatCamera = MatrixInvert(GetCameraMatrix2D(camera));
    Vector3 transform = Vector3Transform((Vector3){ position.x, position.y, 0 }, invMatCamera);

    return (Vector2){ transform.x, transform.y };
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Timming
//----------------------------------------------------------------------------------

// NOTE: Functions with a platform-specific implementation on rcore_<platform>.c
//double GetTime(void)

// Set target FPS (maximum)
void SetTargetFPS(int fps)
{
    if (fps < 1) CORE.Time.target = 0.0;
    else CORE.Time.target = 1.0/(double)fps;

    TRACELOG(LOG_INFO, "TIMER: Target time per frame: %02.03f milliseconds", (float)CORE.Time.target*1000.0f);
}

// Get current FPS
// NOTE: We calculate an average framerate
int GetFPS(void)
{
    int fps = 0;

#if !defined(SUPPORT_CUSTOM_FRAME_CONTROL)
    #define FPS_CAPTURE_FRAMES_COUNT    30      // 30 captures
    #define FPS_AVERAGE_TIME_SECONDS   0.5f     // 500 milliseconds
    #define FPS_STEP (FPS_AVERAGE_TIME_SECONDS/FPS_CAPTURE_FRAMES_COUNT)

    static int index = 0;
    static float history[FPS_CAPTURE_FRAMES_COUNT] = { 0 };
    static float average = 0, last = 0;
    float fpsFrame = GetFrameTime();

    // if we reset the window, reset the FPS info
    if (CORE.Time.frameCounter == 0)
    {
        average = 0;
        last = 0;
        index = 0;

        for (int i = 0; i < FPS_CAPTURE_FRAMES_COUNT; i++) history[i] = 0;
    }

    if (fpsFrame == 0) return 0;

    if ((GetTime() - last) > FPS_STEP)
    {
        last = (float)GetTime();
        index = (index + 1)%FPS_CAPTURE_FRAMES_COUNT;
        average -= history[index];
        history[index] = fpsFrame/FPS_CAPTURE_FRAMES_COUNT;
        average += history[index];
    }

    fps = (int)roundf(1.0f/average);
#endif

    return fps;
}

// Get time in seconds for last frame drawn (delta time)
float GetFrameTime(void)
{
    return (float)CORE.Time.frame;
}

// Get elapsed time measure in seconds since InitTimer()
double GetTime(void)
{
    double time = 0.1;
    return time;
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Custom frame control
//----------------------------------------------------------------------------------

// NOTE: Functions with a platform-specific implementation on rcore_<platform>.c
//void SwapScreenBuffer(void);
//void //(void);

// Wait for some time (stop program execution)
// NOTE: Sleep() granularity could be around 10 ms, it means, Sleep() could
// take longer than expected... for that reason we use the busy wait loop
// Ref: http://stackoverflow.com/questions/43057578/c-programming-win32-games-sleep-taking-longer-than-expected
// Ref: http://www.geisswerks.com/ryan/FAQS/timing.html --> All about timing on Win32!
void WaitTime(double seconds)
{
    if (seconds < 0) return;    // Security check

#if defined(SUPPORT_BUSY_WAIT_LOOP) || defined(SUPPORT_PARTIALBUSY_WAIT_LOOP)
    double destinationTime = GetTime() + seconds;
#endif

#if defined(SUPPORT_BUSY_WAIT_LOOP)
    while (GetTime() < destinationTime) { }
#else
    #if defined(SUPPORT_PARTIALBUSY_WAIT_LOOP)
        double sleepSeconds = seconds - seconds*0.05;  // NOTE: We reserve a percentage of the time for busy waiting
    #else
        double sleepSeconds = seconds;
    #endif

    // System halt functions
    #if defined(_WIN32)
        Sleep((unsigned long)(sleepSeconds*1000.0));
    #endif
    #if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__EMSCRIPTEN__)
        struct timespec req = { 0 };
        time_t sec = sleepSeconds;
        long nsec = (sleepSeconds - sec)*1000000000L;
        req.tv_sec = sec;
        req.tv_nsec = nsec;

        // NOTE: Use nanosleep() on Unix platforms... usleep() it's deprecated
        while (nanosleep(&req, &req) == -1) continue;
    #endif
    #if defined(__APPLE__)
        usleep(sleepSeconds*1000000.0);
    #endif

    #if defined(SUPPORT_PARTIALBUSY_WAIT_LOOP)
        while (GetTime() < destinationTime) { }
    #endif
#endif
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Misc
//----------------------------------------------------------------------------------

// NOTE: Functions with a platform-specific implementation on rcore_<platform>.c
//void OpenURL(const char *url)

// Set the seed for the random number generator
void SetRandomSeed(unsigned int seed)
{
#if defined(SUPPORT_RPRAND_GENERATOR)
    rprand_set_seed(seed);
#else
    srand(seed);
#endif
}

// Get a random value between min and max included
int GetRandomValue(int min, int max)
{
    int value = 0;

    if (min > max)
    {
        int tmp = max;
        max = min;
        min = tmp;
    }

#if defined(SUPPORT_RPRAND_GENERATOR)
    value = rprand_get_value(min, max);
#else
    // WARNING: Ranges higher than RAND_MAX will return invalid results
    // More specifically, if (max - min) > INT_MAX there will be an overflow,
    // and otherwise if (max - min) > RAND_MAX the random value will incorrectly never exceed a certain threshold
    // NOTE: Depending on the library it can be as low as 32767
    if ((unsigned int)(max - min) > (unsigned int)RAND_MAX)
    {
        TRACELOG(LOG_WARNING, "Invalid GetRandomValue() arguments, range should not be higher than %i", RAND_MAX);
    }

    value = (rand()%(abs(max - min) + 1) + min);
#endif
    return value;
}

// Load random values sequence, no values repeated, min and max included
int *LoadRandomSequence(unsigned int count, int min, int max)
{
    int *values = NULL;

#if defined(SUPPORT_RPRAND_GENERATOR)
    values = rprand_load_sequence(count, min, max);
#else
    if (count > ((unsigned int)abs(max - min) + 1)) return values;  // Security check

    values = (int *)RL_CALLOC(count, sizeof(int));

    int value = 0;
    bool dupValue = false;

    for (int i = 0; i < (int)count;)
    {
        value = (rand()%(abs(max - min) + 1) + min);
        dupValue = false;

        for (int j = 0; j < i; j++)
        {
            if (values[j] == value)
            {
                dupValue = true;
                break;
            }
        }

        if (!dupValue)
        {
            values[i] = value;
            i++;
        }
    }
#endif
    return values;
}

// Unload random values sequence
void UnloadRandomSequence(int *sequence)
{
#if defined(SUPPORT_RPRAND_GENERATOR)
    rprand_unload_sequence(sequence);
#else
    RL_FREE(sequence);
#endif
}

// Takes a screenshot of current screen
// NOTE: Provided fileName should not contain paths, saving to working directory
void TakeScreenshot(const char *fileName)
{
#if defined(SUPPORT_MODULE_RTEXTURES)
    // Security check to (partially) avoid malicious code
    if (strchr(fileName, '\'') != NULL) { TRACELOG(LOG_WARNING, "SYSTEM: Provided fileName could be potentially malicious, avoid [\'] character"); return; }

    Vector2 scale = pixi_get_dpi();
    unsigned char *imgData = rlReadScreenPixels((int)((float)CORE.Render.render.width*scale.x), (int)((float)CORE.Render.render.height*scale.y));
    Image image = { imgData, (int)((float)CORE.Render.render.width*scale.x), (int)((float)CORE.Render.render.height*scale.y), 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };

    char path[512] = { 0 };
    strcpy(path, TextFormat("%s/%s", CORE.Storage.basePath, GetFileName(fileName)));

    ExportImage(image, path);           // WARNING: Module required: rtextures
    RL_FREE(imgData);

    if (FileExists(path)) TRACELOG(LOG_INFO, "SYSTEM: [%s] Screenshot taken successfully", path);
    else TRACELOG(LOG_WARNING, "SYSTEM: [%s] Screenshot could not be saved", path);
#else
    TRACELOG(LOG_WARNING,"IMAGE: ExportImage() requires module: rtextures");
#endif
}

// Setup window configuration flags (view FLAGS)
// NOTE: This function is expected to be called before window creation,
// because it sets up some flags for the window creation process
// To configure window states after creation, just use SetWindowState()
void SetConfigFlags(unsigned int flags)
{
    // Selected flags are set but not evaluated at this point,
    // flag evaluation happens at InitWindow() or SetWindowState()
    // CORE.Render.flags |= flags;
}

//----------------------------------------------------------------------------------
// Module Functions Definition: File system
//----------------------------------------------------------------------------------

// Check if the file exists
bool FileExists(const char *fileName)
{
    bool result = false;

#if defined(_WIN32)
    if (_access(fileName, 0) != -1) result = true;
#else
    if (access(fileName, F_OK) != -1) result = true;
#endif

    // NOTE: Alternatively, stat() can be used instead of access()
    //#include <sys/stat.h>
    //struct stat statbuf;
    //if (stat(filename, &statbuf) == 0) result = true;

    return result;
}

// Check file extension
// NOTE: Extensions checking is not case-sensitive
bool IsFileExtension(const char *fileName, const char *ext)
{
    #define MAX_FILE_EXTENSION_LENGTH  16

    bool result = false;
    const char *fileExt = GetFileExtension(fileName);

    if (fileExt != NULL)
    {
#if defined(SUPPORT_MODULE_RTEXT) && defined(SUPPORT_TEXT_MANIPULATION)
        int extCount = 0;
        const char **checkExts = TextSplit(ext, ';', &extCount); // WARNING: Module required: rtext

        char fileExtLower[MAX_FILE_EXTENSION_LENGTH + 1] = { 0 };
        strncpy(fileExtLower, TextToLower(fileExt), MAX_FILE_EXTENSION_LENGTH); // WARNING: Module required: rtext

        for (int i = 0; i < extCount; i++)
        {
            if (strcmp(fileExtLower, TextToLower(checkExts[i])) == 0)
            {
                result = true;
                break;
            }
        }
#else
        if (strcmp(fileExt, ext) == 0) result = true;
#endif
    }

    return result;
}

// Check if a directory path exists
bool DirectoryExists(const char *dirPath)
{
    bool result = false;
    DIR *dir = opendir(dirPath);

    if (dir != NULL)
    {
        result = true;
        closedir(dir);
    }

    return result;
}

// Get file length in bytes
// NOTE: GetFileSize() conflicts with windows.h
int GetFileLength(const char *fileName)
{
    int size = 0;

    // NOTE: On Unix-like systems, it can by used the POSIX system call: stat(),
    // but depending on the platform that call could not be available
    //struct stat result = { 0 };
    //stat(fileName, &result);
    //return result.st_size;

    FILE *file = fopen(fileName, "rb");

    if (file != NULL)
    {
        fseek(file, 0L, SEEK_END);
        long int fileSize = ftell(file);

        // Check for size overflow (INT_MAX)
        if (fileSize > 2147483647) TRACELOG(LOG_WARNING, "[%s] File size overflows expected limit, do not use GetFileLength()", fileName);
        else size = (int)fileSize;

        fclose(file);
    }

    return size;
}

// Get pointer to extension for a filename string (includes the dot: .png)
const char *GetFileExtension(const char *fileName)
{
    const char *dot = strrchr(fileName, '.');

    if (!dot || dot == fileName) return NULL;

    return dot;
}

// String pointer reverse break: returns right-most occurrence of charset in s
static const char *strprbrk(const char *s, const char *charset)
{
    const char *latestMatch = NULL;

    for (; s = strpbrk(s, charset), s != NULL; latestMatch = s++) { }

    return latestMatch;
}

// Get pointer to filename for a path string
const char *GetFileName(const char *filePath)
{
    const char *fileName = NULL;

    if (filePath != NULL) fileName = strprbrk(filePath, "\\/");

    if (fileName == NULL) return filePath;

    return fileName + 1;
}

// Get filename string without extension (uses static string)
const char *GetFileNameWithoutExt(const char *filePath)
{
    #define MAX_FILENAME_LENGTH     256

    static char fileName[MAX_FILENAME_LENGTH] = { 0 };
    memset(fileName, 0, MAX_FILENAME_LENGTH);

    if (filePath != NULL)
    {
        strcpy(fileName, GetFileName(filePath)); // Get filename.ext without path
        int size = (int)strlen(fileName); // Get size in bytes

        for (int i = size; i > 0; i--) // Reverse search '.'
        {
            if (fileName[i] == '.')
            {
                // NOTE: We break on first '.' found
                fileName[i] = '\0';
                break;
            }
        }
    }

    return fileName;
}

// Get directory for a given filePath
const char *GetDirectoryPath(const char *filePath)
{
    /*
    // NOTE: Directory separator is different in Windows and other platforms,
    // fortunately, Windows also support the '/' separator, that's the one should be used
    #if defined(_WIN32)
        char separator = '\\';
    #else
        char separator = '/';
    #endif
    */
    const char *lastSlash = NULL;
    static char dirPath[MAX_FILEPATH_LENGTH] = { 0 };
    memset(dirPath, 0, MAX_FILEPATH_LENGTH);

    // In case provided path does not contain a root drive letter (C:\, D:\) nor leading path separator (\, /),
    // we add the current directory path to dirPath
    if (filePath[1] != ':' && filePath[0] != '\\' && filePath[0] != '/')
    {
        // For security, we set starting path to current directory,
        // obtained path will be concatenated to this
        dirPath[0] = '.';
        dirPath[1] = '/';
    }

    lastSlash = strprbrk(filePath, "\\/");
    if (lastSlash)
    {
        if (lastSlash == filePath)
        {
            // The last and only slash is the leading one: path is in a root directory
            dirPath[0] = filePath[0];
            dirPath[1] = '\0';
        }
        else
        {
            // NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
            char *dirPathPtr = dirPath;
            if ((filePath[1] != ':') && (filePath[0] != '\\') && (filePath[0] != '/')) dirPathPtr += 2;     // Skip drive letter, "C:"
            memcpy(dirPathPtr, filePath, strlen(filePath) - (strlen(lastSlash) - 1));
            dirPath[strlen(filePath) - strlen(lastSlash) + (((filePath[1] != ':') && (filePath[0] != '\\') && (filePath[0] != '/'))? 2 : 0)] = '\0';  // Add '\0' manually
        }
    }

    return dirPath;
}

// Get previous directory path for a given path
const char *GetPrevDirectoryPath(const char *dirPath)
{
    static char prevDirPath[MAX_FILEPATH_LENGTH] = { 0 };
    memset(prevDirPath, 0, MAX_FILEPATH_LENGTH);
    int pathLen = (int)strlen(dirPath);

    if (pathLen <= 3) strcpy(prevDirPath, dirPath);

    for (int i = (pathLen - 1); (i >= 0) && (pathLen > 3); i--)
    {
        if ((dirPath[i] == '\\') || (dirPath[i] == '/'))
        {
            // Check for root: "C:\" or "/"
            if (((i == 2) && (dirPath[1] ==':')) || (i == 0)) i++;

            strncpy(prevDirPath, dirPath, i);
            break;
        }
    }

    return prevDirPath;
}

// Get current working directory
const char *GetWorkingDirectory(void)
{
    static char currentDir[MAX_FILEPATH_LENGTH] = { 0 };
    memset(currentDir, 0, MAX_FILEPATH_LENGTH);

    char *path = GETCWD(currentDir, MAX_FILEPATH_LENGTH - 1);

    return path;
}

const char *GetApplicationDirectory(void)
{
    static char appDir[MAX_FILEPATH_LENGTH] = { 0 };
    memset(appDir, 0, MAX_FILEPATH_LENGTH);

#if defined(_WIN32)
    int len = 0;
#if defined(UNICODE)
    unsigned short widePath[MAX_PATH];
    len = GetModuleFileNameW(NULL, widePath, MAX_PATH);
    len = WideCharToMultiByte(0, 0, widePath, len, appDir, MAX_PATH, NULL, NULL);
#else
    len = GetModuleFileNameA(NULL, appDir, MAX_PATH);
#endif
    if (len > 0)
    {
        for (int i = len; i >= 0; --i)
        {
            if (appDir[i] == '\\')
            {
                appDir[i + 1] = '\0';
                break;
            }
        }
    }
    else
    {
        appDir[0] = '.';
        appDir[1] = '\\';
    }

#elif defined(__linux__)
    unsigned int size = sizeof(appDir);
    ssize_t len = readlink("/proc/self/exe", appDir, size);

    if (len > 0)
    {
        for (int i = len; i >= 0; --i)
        {
            if (appDir[i] == '/')
            {
                appDir[i + 1] = '\0';
                break;
            }
        }
    }
    else
    {
        appDir[0] = '.';
        appDir[1] = '/';
    }
#elif defined(__APPLE__)
    uint32_t size = sizeof(appDir);

    if (_NSGetExecutablePath(appDir, &size) == 0)
    {
        int len = strlen(appDir);
        for (int i = len; i >= 0; --i)
        {
            if (appDir[i] == '/')
            {
                appDir[i + 1] = '\0';
                break;
            }
        }
    }
    else
    {
        appDir[0] = '.';
        appDir[1] = '/';
    }
#elif defined(__FreeBSD__)
    size_t size = sizeof(appDir);
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1};

    if (sysctl(mib, 4, appDir, &size, NULL, 0) == 0)
    {
        int len = strlen(appDir);
        for (int i = len; i >= 0; --i)
        {
            if (appDir[i] == '/')
            {
                appDir[i + 1] = '\0';
                break;
            }
        }
    }
    else
    {
        appDir[0] = '.';
        appDir[1] = '/';
    }

#endif

    return appDir;
}

// Load directory filepaths
// NOTE: Base path is prepended to the scanned filepaths
// WARNING: Directory is scanned twice, first time to get files count
// No recursive scanning is done!
FilePathList LoadDirectoryFiles(const char *dirPath)
{
    FilePathList files = { 0 };
    unsigned int fileCounter = 0;

    struct dirent *entity;
    DIR *dir = opendir(dirPath);

    if (dir != NULL) // It's a directory
    {
        // SCAN 1: Count files
        while ((entity = readdir(dir)) != NULL)
        {
            // NOTE: We skip '.' (current dir) and '..' (parent dir) filepaths
            if ((strcmp(entity->d_name, ".") != 0) && (strcmp(entity->d_name, "..") != 0)) fileCounter++;
        }

        // Memory allocation for dirFileCount
        files.capacity = fileCounter;
        files.paths = (char **)RL_MALLOC(files.capacity*sizeof(char *));
        for (unsigned int i = 0; i < files.capacity; i++) files.paths[i] = (char *)RL_MALLOC(MAX_FILEPATH_LENGTH*sizeof(char));

        closedir(dir);

        // SCAN 2: Read filepaths
        // NOTE: Directory paths are also registered
        ScanDirectoryFiles(dirPath, &files, NULL);

        // Security check: read files.count should match fileCounter
        if (files.count != files.capacity) TRACELOG(LOG_WARNING, "FILEIO: Read files count do not match capacity allocated");
    }
    else TRACELOG(LOG_WARNING, "FILEIO: Failed to open requested directory");  // Maybe it's a file...

    return files;
}

// Load directory filepaths with extension filtering and recursive directory scan
// NOTE: On recursive loading we do not pre-scan for file count, we use MAX_FILEPATH_CAPACITY
FilePathList LoadDirectoryFilesEx(const char *basePath, const char *filter, bool scanSubdirs)
{
    FilePathList files = { 0 };

    files.capacity = MAX_FILEPATH_CAPACITY;
    files.paths = (char **)RL_CALLOC(files.capacity, sizeof(char *));
    for (unsigned int i = 0; i < files.capacity; i++) files.paths[i] = (char *)RL_CALLOC(MAX_FILEPATH_LENGTH, sizeof(char));

    // WARNING: basePath is always prepended to scanned paths
    if (scanSubdirs) ScanDirectoryFilesRecursively(basePath, &files, filter);
    else ScanDirectoryFiles(basePath, &files, filter);

    return files;
}

// Unload directory filepaths
// WARNING: files.count is not reseted to 0 after unloading
void UnloadDirectoryFiles(FilePathList files)
{
    for (unsigned int i = 0; i < files.capacity; i++) RL_FREE(files.paths[i]);

    RL_FREE(files.paths);
}

// Create directories (including full path requested), returns 0 on success
int MakeDirectory(const char *dirPath)
{
    if ((dirPath == NULL) || (dirPath[0] == '\0')) return 1; // Path is not valid
    if (DirectoryExists(dirPath)) return 0; // Path already exists (is valid)

    // Copy path string to avoid modifying original
    int len = (int)strlen(dirPath) + 1;
    char *pathcpy = (char *)RL_CALLOC(len, 1);
    memcpy(pathcpy, dirPath, len);

    // Iterate over pathcpy, create each subdirectory as needed
    for (int i = 0; (i < len) && (pathcpy[i] != '\0'); i++)
    {
        if (pathcpy[i] == ':') i++;
        else
        {
            if ((pathcpy[i] == '\\') || (pathcpy[i] == '/'))
            {
                pathcpy[i] = '\0';
                if (!DirectoryExists(pathcpy)) MKDIR(pathcpy);
                pathcpy[i] = '/';
            }
        }
    }

    // Create final directory
    if (!DirectoryExists(pathcpy)) MKDIR(pathcpy);

    RL_FREE(pathcpy);

    return 0;
}

// Change working directory, returns true on success
bool ChangeDirectory(const char *dir)
{
    bool result = CHDIR(dir);

    if (result != 0) TRACELOG(LOG_WARNING, "SYSTEM: Failed to change to directory: %s", dir);

    return (result == 0);
}

// Check if a given path point to a file
bool IsPathFile(const char *path)
{
    struct stat result = { 0 };
    stat(path, &result);

    return S_ISREG(result.st_mode);
}

// Check if fileName is valid for the platform/OS
bool IsFileNameValid(const char *fileName)
{
    bool valid = true;

    if ((fileName != NULL) && (fileName[0] != '\0'))
    {
        int length = (int)strlen(fileName);
        bool allPeriods = true;

        for (int i = 0; i < length; i++)
        {
            // Check invalid characters
            if ((fileName[i] == '<') ||
                (fileName[i] == '>') ||
                (fileName[i] == ':') ||
                (fileName[i] == '\"') ||
                (fileName[i] == '/') ||
                (fileName[i] == '\\') ||
                (fileName[i] == '|') ||
                (fileName[i] == '?') ||
                (fileName[i] == '*')) { valid = false; break; }

            // Check non-glyph characters
            if ((unsigned char)fileName[i] < 32) { valid = false; break; }

            // TODO: Check trailing periods/spaces?

            // Check if filename is not all periods
            if (fileName[i] != '.') allPeriods = false;
        }

        if (allPeriods) valid = false;

/*
        if (valid)
        {
            // Check invalid DOS names
            if (length >= 3)
            {
                if (((fileName[0] == 'C') && (fileName[1] == 'O') && (fileName[2] == 'N')) ||   // CON
                    ((fileName[0] == 'P') && (fileName[1] == 'R') && (fileName[2] == 'N')) ||   // PRN
                    ((fileName[0] == 'A') && (fileName[1] == 'U') && (fileName[2] == 'X')) ||   // AUX
                    ((fileName[0] == 'N') && (fileName[1] == 'U') && (fileName[2] == 'L'))) valid = false; // NUL
            }

            if (length >= 4)
            {
                if (((fileName[0] == 'C') && (fileName[1] == 'O') && (fileName[2] == 'M') && ((fileName[3] >= '0') && (fileName[3] <= '9'))) ||  // COM0-9
                    ((fileName[0] == 'L') && (fileName[1] == 'P') && (fileName[2] == 'T') && ((fileName[3] >= '0') && (fileName[3] <= '9')))) valid = false; // LPT0-9
            }
        }
*/
    }

    return valid;
}

// Get file modification time (last write time)
long GetFileModTime(const char *fileName)
{
    struct stat result = { 0 };
    long modTime = 0;

    if (stat(fileName, &result) == 0)
    {
        time_t mod = result.st_mtime;

        modTime = (long)mod;
    }

    return modTime;
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Compression and Encoding
//----------------------------------------------------------------------------------

// Compress data (DEFLATE algorithm)
unsigned char *CompressData(const unsigned char *data, int dataSize, int *compDataSize)
{
    #define COMPRESSION_QUALITY_DEFLATE  8

    unsigned char *compData = NULL;

#if defined(SUPPORT_COMPRESSION_API)
    // Compress data and generate a valid DEFLATE stream
    struct sdefl *sdefl = RL_CALLOC(1, sizeof(struct sdefl));   // WARNING: Possible stack overflow, struct sdefl is almost 1MB
    int bounds = sdefl_bound(dataSize);
    compData = (unsigned char *)RL_CALLOC(bounds, 1);

    *compDataSize = sdeflate(sdefl, compData, data, dataSize, COMPRESSION_QUALITY_DEFLATE);   // Compression level 8, same as stbiw
    RL_FREE(sdefl);

    TRACELOG(LOG_INFO, "SYSTEM: Compress data: Original size: %i -> Comp. size: %i", dataSize, *compDataSize);
#endif

    return compData;
}

// Decompress data (DEFLATE algorithm)
unsigned char *DecompressData(const unsigned char *compData, int compDataSize, int *dataSize)
{
    unsigned char *data = NULL;

#if defined(SUPPORT_COMPRESSION_API)
    // Decompress data from a valid DEFLATE stream
    data = (unsigned char *)RL_CALLOC(MAX_DECOMPRESSION_SIZE*1024*1024, 1);
    int length = sinflate(data, MAX_DECOMPRESSION_SIZE*1024*1024, compData, compDataSize);

    // WARNING: RL_REALLOC can make (and leave) data copies in memory, be careful with sensitive compressed data!
    // TODO: Use a different approach, create another buffer, copy data manually to it and wipe original buffer memory
    unsigned char *temp = (unsigned char *)RL_REALLOC(data, length);

    if (temp != NULL) data = temp;
    else TRACELOG(LOG_WARNING, "SYSTEM: Failed to re-allocate required decompression memory");

    *dataSize = length;

    TRACELOG(LOG_INFO, "SYSTEM: Decompress data: Comp. size: %i -> Original size: %i", compDataSize, *dataSize);
#endif

    return data;
}

// Encode data to Base64 string
char *EncodeDataBase64(const unsigned char *data, int dataSize, int *outputSize)
{
    static const unsigned char base64encodeTable[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
    };

    static const int modTable[] = { 0, 2, 1 };

    *outputSize = 4*((dataSize + 2)/3);

    char *encodedData = (char *)RL_MALLOC(*outputSize);

    if (encodedData == NULL) return NULL;   // Security check

    for (int i = 0, j = 0; i < dataSize;)
    {
        unsigned int octetA = (i < dataSize)? (unsigned char)data[i++] : 0;
        unsigned int octetB = (i < dataSize)? (unsigned char)data[i++] : 0;
        unsigned int octetC = (i < dataSize)? (unsigned char)data[i++] : 0;

        unsigned int triple = (octetA << 0x10) + (octetB << 0x08) + octetC;

        encodedData[j++] = base64encodeTable[(triple >> 3*6) & 0x3F];
        encodedData[j++] = base64encodeTable[(triple >> 2*6) & 0x3F];
        encodedData[j++] = base64encodeTable[(triple >> 1*6) & 0x3F];
        encodedData[j++] = base64encodeTable[(triple >> 0*6) & 0x3F];
    }

    for (int i = 0; i < modTable[dataSize%3]; i++) encodedData[*outputSize - 1 - i] = '=';  // Padding character

    return encodedData;
}

// Decode Base64 string data
unsigned char *DecodeDataBase64(const unsigned char *data, int *outputSize)
{
    static const unsigned char base64decodeTable[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 62, 0, 0, 0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
        11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0, 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
        37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    };

    // Get output size of Base64 input data
    int outSize = 0;
    for (int i = 0; data[4*i] != 0; i++)
    {
        if (data[4*i + 3] == '=')
        {
            if (data[4*i + 2] == '=') outSize += 1;
            else outSize += 2;
        }
        else outSize += 3;
    }

    // Allocate memory to store decoded Base64 data
    unsigned char *decodedData = (unsigned char *)RL_MALLOC(outSize);

    for (int i = 0; i < outSize/3; i++)
    {
        unsigned char a = base64decodeTable[(int)data[4*i]];
        unsigned char b = base64decodeTable[(int)data[4*i + 1]];
        unsigned char c = base64decodeTable[(int)data[4*i + 2]];
        unsigned char d = base64decodeTable[(int)data[4*i + 3]];

        decodedData[3*i] = (a << 2) | (b >> 4);
        decodedData[3*i + 1] = (b << 4) | (c >> 2);
        decodedData[3*i + 2] = (c << 6) | d;
    }

    if (outSize%3 == 1)
    {
        int n = outSize/3;
        unsigned char a = base64decodeTable[(int)data[4*n]];
        unsigned char b = base64decodeTable[(int)data[4*n + 1]];
        decodedData[outSize - 1] = (a << 2) | (b >> 4);
    }
    else if (outSize%3 == 2)
    {
        int n = outSize/3;
        unsigned char a = base64decodeTable[(int)data[4*n]];
        unsigned char b = base64decodeTable[(int)data[4*n + 1]];
        unsigned char c = base64decodeTable[(int)data[4*n + 2]];
        decodedData[outSize - 2] = (a << 2) | (b >> 4);
        decodedData[outSize - 1] = (b << 4) | (c >> 2);
    }

    *outputSize = outSize;
    return decodedData;
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Input Handling: Keyboard
//----------------------------------------------------------------------------------

// Check if a key has been pressed once
bool IsKeyPressed(int key)
{

    bool pressed = false;

    if ((key > 0) && (key < MAX_KEYBOARD_KEYS))
    {
        if ((CORE.Input.Keyboard.previousKeyState[key] == 0) && (CORE.Input.Keyboard.currentKeyState[key] == 1)) pressed = true;
    }

    return pressed;
}

// Check if a key has been pressed again
bool IsKeyPressedRepeat(int key)
{
    bool repeat = false;

    if ((key > 0) && (key < MAX_KEYBOARD_KEYS))
    {
        if (CORE.Input.Keyboard.keyRepeatInFrame[key] == 1) repeat = true;
    }

    return repeat;
}

// Check if a key is being pressed (key held down)
bool IsKeyDown(int key)
{
    bool down = false;

    if ((key > 0) && (key < MAX_KEYBOARD_KEYS))
    {
        if (CORE.Input.Keyboard.currentKeyState[key] == 1) down = true;
    }

    return down;
}

// Check if a key has been released once
bool IsKeyReleased(int key)
{
    bool released = false;

    if ((key > 0) && (key < MAX_KEYBOARD_KEYS))
    {
        if ((CORE.Input.Keyboard.previousKeyState[key] == 1) && (CORE.Input.Keyboard.currentKeyState[key] == 0)) released = true;
    }

    return released;
}

// Check if a key is NOT being pressed (key not held down)
bool IsKeyUp(int key)
{
    bool up = false;

    if ((key > 0) && (key < MAX_KEYBOARD_KEYS))
    {
        if (CORE.Input.Keyboard.currentKeyState[key] == 0) up = true;
    }

    return up;
}

// Get the last key pressed
int GetKeyPressed(void)
{
    int value = 0;

    if (CORE.Input.Keyboard.keyPressedQueueCount > 0)
    {
        // Get character from the queue head
        value = CORE.Input.Keyboard.keyPressedQueue[0];

        // Shift elements 1 step toward the head
        for (int i = 0; i < (CORE.Input.Keyboard.keyPressedQueueCount - 1); i++)
            CORE.Input.Keyboard.keyPressedQueue[i] = CORE.Input.Keyboard.keyPressedQueue[i + 1];

        // Reset last character in the queue
        CORE.Input.Keyboard.keyPressedQueue[CORE.Input.Keyboard.keyPressedQueueCount - 1] = 0;
        CORE.Input.Keyboard.keyPressedQueueCount--;
    }

    return value;
}

// Get the last char pressed
int GetCharPressed(void)
{
    int value = 0;

    if (CORE.Input.Keyboard.charPressedQueueCount > 0)
    {
        // Get character from the queue head
        value = CORE.Input.Keyboard.charPressedQueue[0];

        // Shift elements 1 step toward the head
        for (int i = 0; i < (CORE.Input.Keyboard.charPressedQueueCount - 1); i++)
            CORE.Input.Keyboard.charPressedQueue[i] = CORE.Input.Keyboard.charPressedQueue[i + 1];

        // Reset last character in the queue
        CORE.Input.Keyboard.charPressedQueue[CORE.Input.Keyboard.charPressedQueueCount - 1] = 0;
        CORE.Input.Keyboard.charPressedQueueCount--;
    }

    return value;
}

// Set a custom key to exit program
// NOTE: default exitKey is set to ESCAPE
void SetExitKey(int key)
{
    CORE.Input.Keyboard.exitKey = key;
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Input Handling: Gamepad
//----------------------------------------------------------------------------------

// NOTE: Functions with a platform-specific implementation on rcore_<platform>.c
//int SetGamepadMappings(const char *mappings)

// Check if a gamepad is available
bool IsGamepadAvailable(int gamepad)
{
    bool result = false;

    if ((gamepad < MAX_GAMEPADS) && CORE.Input.Gamepad.ready[gamepad]) result = true;

    return result;
}

// Get gamepad internal name id
const char *GetGamepadName(int gamepad)
{
    return CORE.Input.Gamepad.name[gamepad];
}

// Check if a gamepad button has been pressed once
bool IsGamepadButtonPressed(int gamepad, int button)
{
    bool pressed = false;

    if ((gamepad < MAX_GAMEPADS) && CORE.Input.Gamepad.ready[gamepad] && (button < MAX_GAMEPAD_BUTTONS) &&
        (CORE.Input.Gamepad.previousButtonState[gamepad][button] == 0) && (CORE.Input.Gamepad.currentButtonState[gamepad][button] == 1)) pressed = true;

    return pressed;
}

// Check if a gamepad button is being pressed
bool IsGamepadButtonDown(int gamepad, int button)
{
    bool down = false;

    if ((gamepad < MAX_GAMEPADS) && CORE.Input.Gamepad.ready[gamepad] && (button < MAX_GAMEPAD_BUTTONS) &&
        (CORE.Input.Gamepad.currentButtonState[gamepad][button] == 1)) down = true;

    return down;
}

// Check if a gamepad button has NOT been pressed once
bool IsGamepadButtonReleased(int gamepad, int button)
{
    bool released = false;

    if ((gamepad < MAX_GAMEPADS) && CORE.Input.Gamepad.ready[gamepad] && (button < MAX_GAMEPAD_BUTTONS) &&
        (CORE.Input.Gamepad.previousButtonState[gamepad][button] == 1) && (CORE.Input.Gamepad.currentButtonState[gamepad][button] == 0)) released = true;

    return released;
}

// Check if a gamepad button is NOT being pressed
bool IsGamepadButtonUp(int gamepad, int button)
{
    bool up = false;

    if ((gamepad < MAX_GAMEPADS) && CORE.Input.Gamepad.ready[gamepad] && (button < MAX_GAMEPAD_BUTTONS) &&
        (CORE.Input.Gamepad.currentButtonState[gamepad][button] == 0)) up = true;

    return up;
}

// Get the last gamepad button pressed
int GetGamepadButtonPressed(void)
{
    return CORE.Input.Gamepad.lastButtonPressed;
}

// Get gamepad axis count
int GetGamepadAxisCount(int gamepad)
{
    return CORE.Input.Gamepad.axisCount[gamepad];
}

// Get axis movement vector for a gamepad
float GetGamepadAxisMovement(int gamepad, int axis)
{
    float value = (axis == GAMEPAD_AXIS_LEFT_TRIGGER || axis == GAMEPAD_AXIS_RIGHT_TRIGGER)? -1.0f : 0.0f;

    if ((gamepad < MAX_GAMEPADS) && CORE.Input.Gamepad.ready[gamepad] && (axis < MAX_GAMEPAD_AXIS)) {
        float movement = value < 0.0f ? CORE.Input.Gamepad.axisState[gamepad][axis] : fabsf(CORE.Input.Gamepad.axisState[gamepad][axis]);

        // 0.1f = GAMEPAD_AXIS_MINIMUM_DRIFT/DELTA
        if (movement > value + 0.1f) value = CORE.Input.Gamepad.axisState[gamepad][axis];
    }

    return value;
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Input Handling: Mouse
//----------------------------------------------------------------------------------

// NOTE: Functions with a platform-specific implementation on rcore_<platform>.c
//void SetMousePosition(int x, int y)
//void SetMouseCursor(int cursor)

// Check if a mouse button has been pressed once
bool IsMouseButtonPressed(int button)
{
    bool pressed = false;

    if ((CORE.Input.Mouse.currentButtonState[button] == 1) && (CORE.Input.Mouse.previousButtonState[button] == 0)) pressed = true;

    // Map touches to mouse buttons checking
    if ((CORE.Input.Touch.currentTouchState[button] == 1) && (CORE.Input.Touch.previousTouchState[button] == 0)) pressed = true;

    return pressed;
}

// Check if a mouse button is being pressed
bool IsMouseButtonDown(int button)
{
    bool down = false;

    if (CORE.Input.Mouse.currentButtonState[button] == 1) down = true;

    // NOTE: Touches are considered like mouse buttons
    if (CORE.Input.Touch.currentTouchState[button] == 1) down = true;

    return down;
}

// Check if a mouse button has been released once
bool IsMouseButtonReleased(int button)
{
    bool released = false;

    if ((CORE.Input.Mouse.currentButtonState[button] == 0) && (CORE.Input.Mouse.previousButtonState[button] == 1)) released = true;

    // Map touches to mouse buttons checking
    if ((CORE.Input.Touch.currentTouchState[button] == 0) && (CORE.Input.Touch.previousTouchState[button] == 1)) released = true;

    return released;
}

// Check if a mouse button is NOT being pressed
bool IsMouseButtonUp(int button)
{
    bool up = false;

    if (CORE.Input.Mouse.currentButtonState[button] == 0) up = true;

    // NOTE: Touches are considered like mouse buttons
    if (CORE.Input.Touch.currentTouchState[button] == 0) up = true;

    return up;
}

// Get mouse position X
int GetMouseX(void)
{
    int mouseX = (int)((CORE.Input.Mouse.currentPosition.x + CORE.Input.Mouse.offset.x)*CORE.Input.Mouse.scale.x);
    return mouseX;
}

// Get mouse position Y
int GetMouseY(void)
{
    int mouseY = (int)((CORE.Input.Mouse.currentPosition.y + CORE.Input.Mouse.offset.y)*CORE.Input.Mouse.scale.y);
    return mouseY;
}

// Get mouse position XY
Vector2 GetMousePosition(void)
{
    Vector2 position = { 0 };

    position.x = (CORE.Input.Mouse.currentPosition.x + CORE.Input.Mouse.offset.x)*CORE.Input.Mouse.scale.x;
    position.y = (CORE.Input.Mouse.currentPosition.y + CORE.Input.Mouse.offset.y)*CORE.Input.Mouse.scale.y;

    return position;
}

// Get mouse delta between frames
Vector2 GetMouseDelta(void)
{
    Vector2 delta = { 0 };

    delta.x = CORE.Input.Mouse.currentPosition.x - CORE.Input.Mouse.previousPosition.x;
    delta.y = CORE.Input.Mouse.currentPosition.y - CORE.Input.Mouse.previousPosition.y;

    return delta;
}

// Set mouse offset
// NOTE: Useful when rendering to different size targets
void SetMouseOffset(int offsetX, int offsetY)
{
    CORE.Input.Mouse.offset = (Vector2){ (float)offsetX, (float)offsetY };
}

// Set mouse scaling
// NOTE: Useful when rendering to different size targets
void SetMouseScale(float scaleX, float scaleY)
{
    CORE.Input.Mouse.scale = (Vector2){ scaleX, scaleY };
}

// Get mouse wheel movement Y
float GetMouseWheelMove(void)
{
    float result = 0.0f;

    if (fabsf(CORE.Input.Mouse.currentWheelMove.x) > fabsf(CORE.Input.Mouse.currentWheelMove.y)) result = (float)CORE.Input.Mouse.currentWheelMove.x;
    else result = (float)CORE.Input.Mouse.currentWheelMove.y;

    return result;
}

// Get mouse wheel movement X/Y as a vector
Vector2 GetMouseWheelMoveV(void)
{
    Vector2 result = { 0 };

    result = CORE.Input.Mouse.currentWheelMove;

    return result;
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Input Handling: Touch
//----------------------------------------------------------------------------------

// Get touch position X for touch point 0 (relative to screen size)
int GetTouchX(void)
{
    int touchX = (int)CORE.Input.Touch.position[0].x;
    return touchX;
}

// Get touch position Y for touch point 0 (relative to screen size)
int GetTouchY(void)
{
    int touchY = (int)CORE.Input.Touch.position[0].y;
    return touchY;
}

// Get touch position XY for a touch point index (relative to screen size)
// TODO: Touch position should be scaled depending on display size and render size
Vector2 GetTouchPosition(int index)
{
    Vector2 position = { -1.0f, -1.0f };

    if (index < MAX_TOUCH_POINTS) position = CORE.Input.Touch.position[index];
    else TRACELOG(LOG_WARNING, "INPUT: Required touch point out of range (Max touch points: %i)", MAX_TOUCH_POINTS);

    return position;
}

// Get touch point identifier for given index
int GetTouchPointId(int index)
{
    int id = -1;

    if (index < MAX_TOUCH_POINTS) id = CORE.Input.Touch.pointId[index];

    return id;
}

// Get number of touch points
int GetTouchPointCount(void)
{
    return CORE.Input.Touch.pointCount;
}

//----------------------------------------------------------------------------------
// Module Internal Functions Definition
//----------------------------------------------------------------------------------

// NOTE: Functions with a platform-specific implementation on rcore_<platform>.c
//int InitPlatform(void)
//void ClosePlatform(void)

// Initialize hi-resolution timer
void InitTimer(void)
{
    // Setting a higher resolution can improve the accuracy of time-out intervals in wait functions
    // However, it can also reduce overall system performance, because the thread scheduler switches tasks more often
    // High resolutions can also prevent the CPU power management system from entering power-saving modes
    // Setting a higher resolution does not improve the accuracy of the high-resolution performance counter
#if defined(_WIN32) && defined(SUPPORT_WINMM_HIGHRES_TIMER) && !defined(SUPPORT_BUSY_WAIT_LOOP)
    timeBeginPeriod(1);                 // Setup high-resolution timer to 1ms (granularity of 1-2 ms)
#endif

#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__EMSCRIPTEN__)
    struct timespec now = { 0 };

    if (clock_gettime(CLOCK_MONOTONIC, &now) == 0)  // Success
    {
        CORE.Time.base = (unsigned long long int)now.tv_sec*1000000000LLU + (unsigned long long int)now.tv_nsec;
    }
    else TRACELOG(LOG_WARNING, "TIMER: Hi-resolution timer not available");
#endif

    CORE.Time.previous = GetTime();     // Get time as double
}

void pixi_adjust_viewport_size(int width, int height)
{
    // Reset viewport and projection matrix for new size
    SetupViewport(width, height);

    CORE.Render.currentFbo.width = width;
    CORE.Render.currentFbo.height = height;

    // Set current screen size
    CORE.Render.screen.width = width;
    CORE.Render.screen.height = height;

    // NOTE: Postprocessing texture is not scaled to new size
}

// Set viewport for a provided width and height
void SetupViewport(int width, int height)
{
    CORE.Render.render.width = width;
    CORE.Render.render.height = height;

    // Set viewport width and height
    // NOTE: We consider render size (scaled) and offset in case black bars are required and
    // render area does not match full display area (this situation is only applicable on fullscreen mode)
#if defined(__APPLE__)
    Vector2 scale = pixi_get_dpi();
    rlViewport(CORE.Render.renderOffset.x/2*scale.x, CORE.Render.renderOffset.y/2*scale.y, (CORE.Render.render.width)*scale.x, (CORE.Render.render.height)*scale.y);
#else
    rlViewport(CORE.Render.renderOffset.x/2, CORE.Render.renderOffset.y/2, CORE.Render.render.width, CORE.Render.render.height);
#endif

    rlMatrixMode(RL_PROJECTION);        // Switch to projection matrix
    rlLoadIdentity();                   // Reset current matrix (projection)

    // Set orthographic projection to current framebuffer size
    // NOTE: Configured top-left corner as (0, 0)
    rlOrtho(0, CORE.Render.render.width, CORE.Render.render.height, 0, 0.0f, 1.0f);

    rlMatrixMode(RL_MODELVIEW);         // Switch back to modelview matrix
    rlLoadIdentity();                   // Reset current matrix (modelview)
}

// Compute framebuffer size relative to screen size and display size
// NOTE: Global variables CORE.Render.render.width/CORE.Render.render.height and CORE.Render.renderOffset.x/CORE.Render.renderOffset.y can be modified
void SetupFramebuffer(int width, int height)
{
    // Calculate CORE.Render.render.width and CORE.Render.render.height, we have the display size (input params) and the desired screen size (global var)
    if ((CORE.Render.screen.width > CORE.Render.display.width) || (CORE.Render.screen.height > CORE.Render.display.height))
    {
        TRACELOG(LOG_WARNING, "DISPLAY: Downscaling required: Screen size (%ix%i) is bigger than display size (%ix%i)", CORE.Render.screen.width, CORE.Render.screen.height, CORE.Render.display.width, CORE.Render.display.height);

        // Downscaling to fit display with border-bars
        float widthRatio = (float)CORE.Render.display.width/(float)CORE.Render.screen.width;
        float heightRatio = (float)CORE.Render.display.height/(float)CORE.Render.screen.height;

        if (widthRatio <= heightRatio)
        {
            CORE.Render.render.width = CORE.Render.display.width;
            CORE.Render.render.height = (int)round((float)CORE.Render.screen.height*widthRatio);
            CORE.Render.renderOffset.x = 0;
            CORE.Render.renderOffset.y = (CORE.Render.display.height - CORE.Render.render.height);
        }
        else
        {
            CORE.Render.render.width = (int)round((float)CORE.Render.screen.width*heightRatio);
            CORE.Render.render.height = CORE.Render.display.height;
            CORE.Render.renderOffset.x = (CORE.Render.display.width - CORE.Render.render.width);
            CORE.Render.renderOffset.y = 0;
        }

        // Screen scaling required
        float scaleRatio = (float)CORE.Render.render.width/(float)CORE.Render.screen.width;
        CORE.Render.screenScale = MatrixScale(scaleRatio, scaleRatio, 1.0f);

        // NOTE: We render to full display resolution!
        // We just need to calculate above parameters for downscale matrix and offsets
        CORE.Render.render.width = CORE.Render.display.width;
        CORE.Render.render.height = CORE.Render.display.height;

        TRACELOG(LOG_WARNING, "DISPLAY: Downscale matrix generated, content will be rendered at (%ix%i)", CORE.Render.render.width, CORE.Render.render.height);
    }
    else if ((CORE.Render.screen.width < CORE.Render.display.width) || (CORE.Render.screen.height < CORE.Render.display.height))
    {
        // Required screen size is smaller than display size
        TRACELOG(LOG_INFO, "DISPLAY: Upscaling required: Screen size (%ix%i) smaller than display size (%ix%i)", CORE.Render.screen.width, CORE.Render.screen.height, CORE.Render.display.width, CORE.Render.display.height);

        if ((CORE.Render.screen.width == 0) || (CORE.Render.screen.height == 0))
        {
            CORE.Render.screen.width = CORE.Render.display.width;
            CORE.Render.screen.height = CORE.Render.display.height;
        }

        // Upscaling to fit display with border-bars
        float displayRatio = (float)CORE.Render.display.width/(float)CORE.Render.display.height;
        float screenRatio = (float)CORE.Render.screen.width/(float)CORE.Render.screen.height;

        if (displayRatio <= screenRatio)
        {
            CORE.Render.render.width = CORE.Render.screen.width;
            CORE.Render.render.height = (int)round((float)CORE.Render.screen.width/displayRatio);
            CORE.Render.renderOffset.x = 0;
            CORE.Render.renderOffset.y = (CORE.Render.render.height - CORE.Render.screen.height);
        }
        else
        {
            CORE.Render.render.width = (int)round((float)CORE.Render.screen.height*displayRatio);
            CORE.Render.render.height = CORE.Render.screen.height;
            CORE.Render.renderOffset.x = (CORE.Render.render.width - CORE.Render.screen.width);
            CORE.Render.renderOffset.y = 0;
        }
    }
    else
    {
        CORE.Render.render.width = CORE.Render.screen.width;
        CORE.Render.render.height = CORE.Render.screen.height;
        CORE.Render.renderOffset.x = 0;
        CORE.Render.renderOffset.y = 0;
    }
}

// Scan all files and directories in a base path
// WARNING: files.paths[] must be previously allocated and
// contain enough space to store all required paths
static void ScanDirectoryFiles(const char *basePath, FilePathList *files, const char *filter)
{
    static char path[MAX_FILEPATH_LENGTH] = { 0 };
    memset(path, 0, MAX_FILEPATH_LENGTH);

    struct dirent *dp = NULL;
    DIR *dir = opendir(basePath);

    if (dir != NULL)
    {
        while ((dp = readdir(dir)) != NULL)
        {
            if ((strcmp(dp->d_name, ".") != 0) &&
                (strcmp(dp->d_name, "..") != 0))
            {
            #if defined(_WIN32)
                sprintf(path, "%s\\%s", basePath, dp->d_name);
            #else
                sprintf(path, "%s/%s", basePath, dp->d_name);
            #endif

                if (filter != NULL)
                {
                    if (IsPathFile(path))
                    {
                        if (IsFileExtension(path, filter))
                        {
                            strcpy(files->paths[files->count], path);
                            files->count++;
                        }
                    }
                    else
                    {
                        if (TextFindIndex(filter, DIRECTORY_FILTER_TAG) >= 0)
                        {
                            strcpy(files->paths[files->count], path);
                            files->count++;
                        }
                    }
                }
                else
                {
                    strcpy(files->paths[files->count], path);
                    files->count++;
                }
            }
        }

        closedir(dir);
    }
    else TRACELOG(LOG_WARNING, "FILEIO: Directory cannot be opened (%s)", basePath);
}

// Scan all files and directories recursively from a base path
static void ScanDirectoryFilesRecursively(const char *basePath, FilePathList *files, const char *filter)
{
    char path[MAX_FILEPATH_LENGTH] = { 0 };
    memset(path, 0, MAX_FILEPATH_LENGTH);

    struct dirent *dp = NULL;
    DIR *dir = opendir(basePath);

    if (dir != NULL)
    {
        while (((dp = readdir(dir)) != NULL) && (files->count < files->capacity))
        {
            if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
            {
                // Construct new path from our base path
            #if defined(_WIN32)
                sprintf(path, "%s\\%s", basePath, dp->d_name);
            #else
                sprintf(path, "%s/%s", basePath, dp->d_name);
            #endif

                if (IsPathFile(path))
                {
                    if (filter != NULL)
                    {
                        if (IsFileExtension(path, filter))
                        {
                            strcpy(files->paths[files->count], path);
                            files->count++;
                        }
                    }
                    else
                    {
                        strcpy(files->paths[files->count], path);
                        files->count++;
                    }

                    if (files->count >= files->capacity)
                    {
                        TRACELOG(LOG_WARNING, "FILEIO: Maximum filepath scan capacity reached (%i files)", files->capacity);
                        break;
                    }
                }
                else 
                {
                    if ((filter != NULL) && (TextFindIndex(filter, DIRECTORY_FILTER_TAG) >= 0))
                    {
                        strcpy(files->paths[files->count], path);
                        files->count++;
                    }
                    
                    if (files->count >= files->capacity)
                    {
                        TRACELOG(LOG_WARNING, "FILEIO: Maximum filepath scan capacity reached (%i files)", files->capacity);
                        break;
                    }

                    ScanDirectoryFilesRecursively(path, files, filter);
                }
            }
        }

        closedir(dir);
    }
    else TRACELOG(LOG_WARNING, "FILEIO: Directory cannot be opened (%s)", basePath);
}

#if !defined(SUPPORT_MODULE_RTEXT)
// Formatting of text with variables to 'embed'
// WARNING: String returned will expire after this function is called MAX_TEXTFORMAT_BUFFERS times
const char *TextFormat(const char *text, ...)
{
#ifndef MAX_TEXTFORMAT_BUFFERS
    #define MAX_TEXTFORMAT_BUFFERS      4        // Maximum number of static buffers for text formatting
#endif
#ifndef MAX_TEXT_BUFFER_LENGTH
    #define MAX_TEXT_BUFFER_LENGTH   1024        // Maximum size of static text buffer
#endif

    // We create an array of buffers so strings don't expire until MAX_TEXTFORMAT_BUFFERS invocations
    static char buffers[MAX_TEXTFORMAT_BUFFERS][MAX_TEXT_BUFFER_LENGTH] = { 0 };
    static int index = 0;

    char *currentBuffer = buffers[index];
    memset(currentBuffer, 0, MAX_TEXT_BUFFER_LENGTH);   // Clear buffer before using

    va_list args;
    va_start(args, text);
    int requiredByteCount = vsnprintf(currentBuffer, MAX_TEXT_BUFFER_LENGTH, text, args);
    va_end(args);

    // If requiredByteCount is larger than the MAX_TEXT_BUFFER_LENGTH, then overflow occured
    if (requiredByteCount >= MAX_TEXT_BUFFER_LENGTH)
    {
        // Inserting "..." at the end of the string to mark as truncated
        char *truncBuffer = buffers[index] + MAX_TEXT_BUFFER_LENGTH - 4; // Adding 4 bytes = "...\0"
        sprintf(truncBuffer, "...");
    }

    index += 1;     // Move to next buffer for next function call
    if (index >= MAX_TEXTFORMAT_BUFFERS) index = 0;

    return currentBuffer;
}

#endif // !SUPPORT_MODULE_RTEXT
