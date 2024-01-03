/**********************************************************************************************
*
*   rcore_<platform> template - Functions to manage window, graphics device and inputs
*
*   PLATFORM: <PLATFORM>
*       - TODO: Define the target platform for the core
*
*   LIMITATIONS:
*       - Limitation 01
*       - Limitation 02
*
*   POSSIBLE IMPROVEMENTS:
*       - Improvement 01
*       - Improvement 02
*
*   ADDITIONAL NOTES:
*       - TRACELOG() function is located in raylib [utils] module
*
*   CONFIGURATION:
*       #define RCORE_PLATFORM_CUSTOM_FLAG
*           Custom flag for rcore on target platform -not used-
*
*   DEPENDENCIES:
*       - <platform-specific SDK dependency>
*       - gestures: Gestures system for touch-ready devices (or simulated from mouse inputs)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5) and contributors
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

#define GLFW_KEY_F12 
#define GLFW_MOD_CONTROL    0x0002
#define GLFW_KEY_F12        301
#define GLFW_TRUE           1
#define GLFW_FALSE          0
#define GLFW_REPEAT         2
#define GLFW_PRESS          1
#define GLFW_RELEASE        0
#define GLFW_MOD_CAPS_LOCK       0x0010
#define GLFW_MOD_NUM_LOCK        0x0020

// TODO: Include the platform specific libraries

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct {
    // TODO: Define the platform specific variables required
} PlatformData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
extern CoreData CORE;                   // Global CORE state context

static PlatformData platform = { 0 };   // Platform specific data

//----------------------------------------------------------------------------------
// Module Internal Functions Declaration
//----------------------------------------------------------------------------------
int InitPlatform(void);          // Initialize platform (graphics, inputs and more)
bool InitGraphicsDevice(void);   // Initialize graphics device

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
// NOTE: Functions declaration is provided by raylib.h

//----------------------------------------------------------------------------------
// Module Functions Definition: Window and Graphics Device
//----------------------------------------------------------------------------------

// Check if application should close
bool WindowShouldClose(void)
{
    if (CORE.Window.ready) return CORE.Window.shouldClose;
    else return true;
}

// Toggle fullscreen mode
void ToggleFullscreen(void)
{
    TRACELOG(LOG_WARNING, "ToggleFullscreen() not available on target platform");
}

// Toggle borderless windowed mode
void ToggleBorderlessWindowed(void)
{
    TRACELOG(LOG_WARNING, "ToggleBorderlessWindowed() not available on target platform");
}

// Set window state: maximized, if resizable
void MaximizeWindow(void)
{
    TRACELOG(LOG_WARNING, "MaximizeWindow() not available on target platform");
}

// Set window state: minimized
void MinimizeWindow(void)
{
    TRACELOG(LOG_WARNING, "MinimizeWindow() not available on target platform");
}

// Set window state: not minimized/maximized
void RestoreWindow(void)
{
    TRACELOG(LOG_WARNING, "RestoreWindow() not available on target platform");
}

// Set window configuration state using flags
void SetWindowState(unsigned int flags)
{
    TRACELOG(LOG_WARNING, "SetWindowState() not available on target platform");
}

// Clear window configuration state flags
void ClearWindowState(unsigned int flags)
{
    TRACELOG(LOG_WARNING, "ClearWindowState() not available on target platform");
}

// Set icon for window
void SetWindowIcon(Image image)
{
    TRACELOG(LOG_WARNING, "SetWindowIcon() not available on target platform");
}

// Set icon for window
void SetWindowIcons(Image* images, int count)
{
    TRACELOG(LOG_WARNING, "SetWindowIcons() not available on target platform");
}

// Set title for window
void SetWindowTitle(const char* title)
{
    CORE.Window.title = title;
}

// Set window position on screen (windowed mode)
void SetWindowPosition(int x, int y)
{
    TRACELOG(LOG_WARNING, "SetWindowPosition() not available on target platform");
}

// Set monitor for the current window
void SetWindowMonitor(int monitor)
{
    TRACELOG(LOG_WARNING, "SetWindowMonitor() not available on target platform");
}

// Set window minimum dimensions (FLAG_WINDOW_RESIZABLE)
void SetWindowMinSize(int width, int height)
{
    CORE.Window.screenMin.width = width;
    CORE.Window.screenMin.height = height;
}

// Set window maximum dimensions (FLAG_WINDOW_RESIZABLE)
void SetWindowMaxSize(int width, int height)
{
    CORE.Window.screenMax.width = width;
    CORE.Window.screenMax.height = height;
}

// Set window dimensions
void SetWindowSize(int width, int height)
{
    TRACELOG(LOG_WARNING, "SetWindowSize() not available on target platform");
}

// Set window opacity, value opacity is between 0.0 and 1.0
void SetWindowOpacity(float opacity)
{
    TRACELOG(LOG_WARNING, "SetWindowOpacity() not available on target platform");
}

// Set window focused
void SetWindowFocused(void)
{
    TRACELOG(LOG_WARNING, "SetWindowFocused() not available on target platform");
}

// Get native window handle
void* GetWindowHandle(void)
{
    TRACELOG(LOG_WARNING, "GetWindowHandle() not implemented on target platform");
    return NULL;
}

// Get number of monitors
int GetMonitorCount(void)
{
    TRACELOG(LOG_WARNING, "GetMonitorCount() not implemented on target platform");
    return 1;
}

// Get number of monitors
int GetCurrentMonitor(void)
{
    TRACELOG(LOG_WARNING, "GetCurrentMonitor() not implemented on target platform");
    return 0;
}

// Get selected monitor position
Vector2 GetMonitorPosition(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorPosition() not implemented on target platform");
    return (Vector2) { 0, 0 };
}

// Get selected monitor width (currently used by monitor)
int GetMonitorWidth(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorWidth() not implemented on target platform");
    return 0;
}

// Get selected monitor height (currently used by monitor)
int GetMonitorHeight(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorHeight() not implemented on target platform");
    return 0;
}

// Get selected monitor physical width in millimetres
int GetMonitorPhysicalWidth(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorPhysicalWidth() not implemented on target platform");
    return 0;
}

// Get selected monitor physical height in millimetres
int GetMonitorPhysicalHeight(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorPhysicalHeight() not implemented on target platform");
    return 0;
}

// Get selected monitor refresh rate
int GetMonitorRefreshRate(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorRefreshRate() not implemented on target platform");
    return 0;
}

// Get the human-readable, UTF-8 encoded name of the selected monitor
const char* GetMonitorName(int monitor)
{
    TRACELOG(LOG_WARNING, "GetMonitorName() not implemented on target platform");
    return "";
}

// Get window position XY on monitor
Vector2 GetWindowPosition(void)
{
    TRACELOG(LOG_WARNING, "GetWindowPosition() not implemented on target platform");
    return (Vector2) { 0, 0 };
}

// Get window scale DPI factor for current monitor
Vector2 GetWindowScaleDPI(void)
{
    TRACELOG(LOG_WARNING, "GetWindowScaleDPI() not implemented on target platform");
    return (Vector2) { 1.0f, 1.0f };
}

// Set clipboard text content
void SetClipboardText(const char* text)
{
    TRACELOG(LOG_WARNING, "SetClipboardText() not implemented on target platform");
}

// Get clipboard text content
// NOTE: returned string is allocated and freed by GLFW
const char* GetClipboardText(void)
{
    TRACELOG(LOG_WARNING, "GetClipboardText() not implemented on target platform");
    return NULL;
}

// Show mouse cursor
void ShowCursor(void)
{
    CORE.Input.Mouse.cursorHidden = false;
}

// Hides mouse cursor
void HideCursor(void)
{
    CORE.Input.Mouse.cursorHidden = true;
}

// Enables cursor (unlock cursor)
void EnableCursor(void)
{
    // Set cursor position in the middle
    SetMousePosition(CORE.Window.screen.width / 2, CORE.Window.screen.height / 2);

    CORE.Input.Mouse.cursorHidden = false;
}

// Disables cursor (lock cursor)
void DisableCursor(void)
{
    // Set cursor position in the middle
    SetMousePosition(CORE.Window.screen.width / 2, CORE.Window.screen.height / 2);

    CORE.Input.Mouse.cursorHidden = true;
}

// Swap back buffer with front buffer (screen drawing)
void SwapScreenBuffer(void)
{
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Misc
//----------------------------------------------------------------------------------

// Get elapsed time measure in seconds since InitTimer()
double GetTime(void)
{
    double time = 0.0;
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);
    unsigned long long int nanoSeconds = (unsigned long long int)ts.tv_sec * 1000000000LLU + (unsigned long long int)ts.tv_nsec;

    time = (double)(nanoSeconds - CORE.Time.base) * 1e-9;  // Elapsed time since InitTimer()

    return time;
}

// Open URL with default system browser (if available)
// NOTE: This function is only safe to use if you control the URL given.
// A user could craft a malicious string performing another action.
// Only call this function yourself not with user input or make sure to check the string yourself.
// Ref: https://github.com/raysan5/raylib/issues/686
void OpenURL(const char* url)
{
    // Security check to (partially) avoid malicious code on target platform
    if (strchr(url, '\'') != NULL) TRACELOG(LOG_WARNING, "SYSTEM: Provided URL could be potentially malicious, avoid [\'] character");
    else
    {
        // TODO:
    }
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Inputs
//----------------------------------------------------------------------------------

static const int keymap_raylib[] =
    {0, 1, 2, 3, 4, 5, 6, 7, KEY_BACKSPACE, KEY_TAB, 10, 11, 12, KEY_ENTER, 14, 15,
     KEY_LEFT_SHIFT, KEY_LEFT_CONTROL, KEY_LEFT_ALT, KEY_PAUSE, KEY_CAPS_LOCK, 21, 22,
     23, 24, 25, 26, KEY_ESCAPE, 28, 29, 30, 31, KEY_SPACE, KEY_PAGE_UP, KEY_PAGE_DOWN,
     KEY_END, KEY_HOME, KEY_LEFT, KEY_UP, KEY_RIGHT, KEY_DOWN, 41, 42, 43, KEY_PRINT_SCREEN,
     KEY_INSERT, KEY_DELETE, 47, 48, KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX,
     KEY_SEVEN, KEY_EIGHT, KEY_NINE, 58, 59, 60, 61, 62, 63, 64, KEY_A, KEY_B, KEY_C, KEY_D,
     KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
     KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z, KEY_KB_MENU,
     KEY_KB_MENU, 93, 94, 95, KEY_KP_0, KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_4,
     KEY_KP_5, KEY_KP_6, KEY_KP_7, KEY_KP_8, KEY_KP_9, KEY_KP_MULTIPLY, KEY_KP_ADD, 108,
     KEY_KP_SUBTRACT, KEY_KP_DECIMAL, KEY_KP_DIVIDE, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
     KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, 124, 125,
     126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
     KEY_NUM_LOCK, KEY_SCROLL_LOCK, 146, 147, 148, 149, 150, 151, 152,
     153, 154, 155, 156, 157, 158, 159, 160, KEY_RIGHT_SHIFT,
     162, KEY_RIGHT_CONTROL, 164, KEY_RIGHT_ALT, 166, 167, 168, 169, 170,
     171, 172, 173, 174, 175, 176, 177, 178, 179,
     180, 181, 182, 183, 184, 185, KEY_SEMICOLON, KEY_EQUAL, KEY_COMMA,
     KEY_MINUS, KEY_PERIOD, KEY_SLASH, 192, 193, 194, 195, 196, 197,
     198, 199, 200, 201, 202, 203, 204, 205, 206,
     207, 208, 209, 210, 211, 212, 213, 214, 215,
     216, 217, 218, KEY_LEFT_BRACKET, KEY_BACKSLASH, KEY_RIGHT_BRACKET, 222, 223, 224,
     225, 226, 227, 228, 229, 230, 231, 232, 233,
     234, 235, 236, 237, 238, 239, 240, 241, 242,
     243, 244, 245, 246, 247, 248, 249};

void keycallback(int key, int scancode, int action, int mods)
{
    int kkey = keymap_raylib[key];
    if (kkey < 0) return;    // Security check, macOS fn key generates -1

    // WARNING: GLFW could return GLFW_REPEAT, we need to consider it as 1
    // to work properly with our implementation (IsKeyDown/IsKeyUp checks)
    if (action == GLFW_RELEASE) CORE.Input.Keyboard.currentKeyState[kkey] = 0;
    else if(action == GLFW_PRESS) CORE.Input.Keyboard.currentKeyState[kkey] = 1;
    else if(action == GLFW_REPEAT) CORE.Input.Keyboard.keyRepeatInFrame[kkey] = 1;

    // WARNING: Check if CAPS/NUM key modifiers are enabled and force down state for those keys
    if (((kkey == KEY_CAPS_LOCK) && ((mods & GLFW_MOD_CAPS_LOCK) > 0)) ||
        ((kkey == KEY_NUM_LOCK) && ((mods & GLFW_MOD_NUM_LOCK) > 0))) CORE.Input.Keyboard.currentKeyState[kkey] = 1;

    // Check if there is space available in the key queue
    if ((CORE.Input.Keyboard.keyPressedQueueCount < MAX_KEY_PRESSED_QUEUE) && (action == GLFW_PRESS))
    {
        // Add character to the queue
        CORE.Input.Keyboard.keyPressedQueue[CORE.Input.Keyboard.keyPressedQueueCount] = kkey;
        CORE.Input.Keyboard.keyPressedQueueCount++;
    }
}

// Set internal gamepad mappings
int SetGamepadMappings(const char* mappings)
{
    TRACELOG(LOG_WARNING, "SetGamepadMappings() not implemented on target platform");
    return 0;
}

// Set mouse position XY
void SetMousePosition(int x, int y)
{
    CORE.Input.Mouse.currentPosition = (Vector2){ (float)x, (float)y };
    CORE.Input.Mouse.previousPosition = CORE.Input.Mouse.currentPosition;
}

// Set mouse cursor
void SetMouseCursor(int cursor)
{
    TRACELOG(LOG_WARNING, "SetMouseCursor() not implemented on target platform");
}

// Register all input events
void PollInputEvents(void)
{
#if defined(SUPPORT_GESTURES_SYSTEM)
    // NOTE: Gestures update must be called every frame to reset gestures correctly
    // because ProcessGestureEvent() is just called on an event, not every frame
    UpdateGestures();
#endif

    // Reset keys/chars pressed registered
    CORE.Input.Keyboard.keyPressedQueueCount = 0;
    CORE.Input.Keyboard.charPressedQueueCount = 0;
    // Reset key repeats
    for (int i = 0; i < MAX_KEYBOARD_KEYS; i++) CORE.Input.Keyboard.keyRepeatInFrame[i] = 0;

    // Reset last gamepad button/axis registered state
    CORE.Input.Gamepad.lastButtonPressed = 0;       // GAMEPAD_BUTTON_UNKNOWN
    //CORE.Input.Gamepad.axisCount = 0;
    // Keyboard/Mouse input polling (automatically managed by GLFW3 through callback)

    // Register previous keys states
    for (int i = 0; i < MAX_KEYBOARD_KEYS; i++)
    {
        CORE.Input.Keyboard.previousKeyState[i] = CORE.Input.Keyboard.currentKeyState[i];
        CORE.Input.Keyboard.keyRepeatInFrame[i] = 0;
    }

    // Register previous mouse states
    for (int i = 0; i < MAX_MOUSE_BUTTONS; i++) CORE.Input.Mouse.previousButtonState[i] = CORE.Input.Mouse.currentButtonState[i];

    // Register previous mouse wheel state
    CORE.Input.Mouse.previousWheelMove = CORE.Input.Mouse.currentWheelMove;
    CORE.Input.Mouse.currentWheelMove = (Vector2){ 0.0f, 0.0f };

    // Register previous mouse position
    CORE.Input.Mouse.previousPosition = CORE.Input.Mouse.currentPosition;

    // Register previous touch states
    for (int i = 0; i < MAX_TOUCH_POINTS; i++) CORE.Input.Touch.previousTouchState[i] = CORE.Input.Touch.currentTouchState[i];

    // Reset touch positions
    //for (int i = 0; i < MAX_TOUCH_POINTS; i++) CORE.Input.Touch.position[i] = (Vector2){ 0, 0 };

    // Map touch position to mouse position for convenience
    // WARNING: If the target desktop device supports touch screen, this behavious should be reviewed!
    // TODO: GLFW does not support multi-touch input just yet
    // https://www.codeproject.com/Articles/668404/Programming-for-Multi-Touch
    // https://docs.microsoft.com/en-us/windows/win32/wintouch/getting-started-with-multi-touch-messages
    CORE.Input.Touch.position[0] = CORE.Input.Mouse.currentPosition;

    CORE.Window.resizedLastFrame = false;
}



//----------------------------------------------------------------------------------
// Module Internal Functions Definition
//----------------------------------------------------------------------------------

void ViewportSizeCallback(int width, int height)
{
    // Reset viewport and projection matrix for new size
    SetupViewport(width, height);

    CORE.Window.currentFbo.width = width;
    CORE.Window.currentFbo.height = height;
    CORE.Window.resizedLastFrame = true;

    if (IsWindowFullscreen()) return;

    // Set current screen size
#if defined(__APPLE__)
    CORE.Window.screen.width = width;
    CORE.Window.screen.height = height;
#else
    if ((CORE.Window.flags & FLAG_WINDOW_HIGHDPI) > 0)
    {
        Vector2 windowScaleDPI = GetWindowScaleDPI();

        CORE.Window.screen.width = (unsigned int)(width / windowScaleDPI.x);
        CORE.Window.screen.height = (unsigned int)(height / windowScaleDPI.y);
    }
    else
    {
        CORE.Window.screen.width = width;
        CORE.Window.screen.height = height;
    }
#endif

    // NOTE: Postprocessing texture is not scaled to new size
}

// Initialize platform: graphics, inputs and more
int InitPlatform(void)
{
    // TODO: Initialize graphic device: display/window
    // It usually requires setting up the platform display system configuration
    // and connexion with the GPU through some system graphic API
    // raylib uses OpenGL so, platform should create that kind of connection
    // Below example illustrates that process using EGL library
    //----------------------------------------------------------------------------
    CORE.Window.fullscreen = true;
    CORE.Window.flags |= FLAG_FULLSCREEN_MODE;

    CORE.Window.ready = true;

    CORE.Window.render.width = CORE.Window.screen.width;
    CORE.Window.render.height = CORE.Window.screen.height;
    CORE.Window.currentFbo.width = CORE.Window.render.width;
    CORE.Window.currentFbo.height = CORE.Window.render.height;

//----------------------------------------------------------------------------

// If everything work as expected, we can continue
    CORE.Window.render.width = CORE.Window.screen.width;
    CORE.Window.render.height = CORE.Window.screen.height;
    CORE.Window.currentFbo.width = CORE.Window.render.width;
    CORE.Window.currentFbo.height = CORE.Window.render.height;

    // TODO: Load OpenGL extensions
    // NOTE: GL procedures address loader is required to load extensions
    //----------------------------------------------------------------------------
    rlLoadExtensions();
    //----------------------------------------------------------------------------

    // TODO: Initialize input events system
    // It could imply keyboard, mouse, gamepad, touch...
    // Depending on the platform libraries/SDK it could use a callbacks mechanims
    // For system events and inputs evens polling on a per-frame basis, use PollInputEvents()
    //----------------------------------------------------------------------------
    // ...
    //----------------------------------------------------------------------------

    // TODO: Initialize timming system
    //----------------------------------------------------------------------------
    InitTimer();
    //----------------------------------------------------------------------------

    // TODO: Initialize storage system
    //----------------------------------------------------------------------------
    CORE.Storage.basePath = GetWorkingDirectory();
    //----------------------------------------------------------------------------

    return 0;
}

// Close platform
void ClosePlatform(void)
{
#if defined(_WIN32) && defined(SUPPORT_WINMM_HIGHRES_TIMER) && !defined(SUPPORT_BUSY_WAIT_LOOP)
    timeEndPeriod(1);           // Restore time period
#endif
}

// EOF
