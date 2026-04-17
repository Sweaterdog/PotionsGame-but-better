/**
 * @file engine.h
 * @brief Universal Terminal-Based Game Engine - Main Header
 * 
 * This is the primary entry point for the game engine. It provides:
 * - Core game loop management
 * - Event system (keyboard, mouse)
 * - State machine for scenes/menus
 * - Initialization and shutdown lifecycle
 * 
 * Usage: Include this file in your game's main source file.
 */

#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * PLATFORM DETECTION
 * ============================================================ */
#if defined(_WIN32) || defined(_WIN64)
    #define ENGINE_PLATFORM_WINDOWS 1
#elif defined(__APPLE__) && defined(__MACH__)
    #define ENGINE_PLATFORM_MACOS 1
#elif defined(__linux__)
    #define ENGINE_PLATFORM_LINUX 1
#elif defined(__unix__)
    #define ENGINE_PLATFORM_UNIX 1
#else
    #define ENGINE_PLATFORM_UNKNOWN 1
#endif

/* ============================================================
 * CONSTANTS
 * ============================================================ */
#define ENGINE_MAX_STATES       32
#define ENGINE_MAX_EVENTS       256
#define ENGINE_TITLE_LENGTH     128
#define ENGINE_DEFAULT_WIDTH    80
#define ENGINE_DEFAULT_HEIGHT   24
#define ENGINE_TICKS_PER_SECOND 60

/* ============================================================
 * EVENT SYSTEM
 * ============================================================ */

/** Event types supported by the engine */
typedef enum {
    ENGINE_EVENT_NONE = 0,
    ENGINE_EVENT_KEY_PRESS,
    ENGINE_EVENT_KEY_RELEASE,
    ENGINE_EVENT_KEY_REPEAT,
    ENGINE_EVENT_MOUSE_MOVE,
    ENGINE_EVENT_MOUSE_BUTTON_DOWN,
    ENGINE_EVENT_MOUSE_BUTTON_UP,
    ENGINE_EVENT_WINDOW_RESIZE,
    ENGINE_EVENT_QUIT,
    ENGINE_EVENT_COUNT
} EngineEventType;

/** Key codes for cross-platform keyboard input */
typedef enum {
    KEY_NULL = 0,
    
    /* Special keys */
    KEY_ESCAPE = 256,
    KEY_ENTER,
    KEY_TAB,
    KEY_BACKSPACE,
    KEY_INSERT,
    KEY_DELETE,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_DOWN,
    KEY_UP,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_HOME,
    KEY_END,
    
    /* Function keys */
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    
    /* Control keys */
    KEY_SPACE = 32,
    KEY_APOSTROPHE = 39,
    KEY_COMMA = 44,
    KEY_MINUS = 45,
    KEY_PERIOD = 46,
    KEY_SLASH = 47,
    
    /* Digits */
    KEY_0 = 48, KEY_1, KEY_2, KEY_3, KEY_4, 
    KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    
    /* Letters */
    KEY_A = 65, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G,
    KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N,
    KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U,
    KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    
    /* Punctuation */
    KEY_SEMICOLON = 59,
    KEY_LEFT_BRACKET = 91,
    KEY_BACKSLASH = 92,
    KEY_RIGHT_BRACKET = 93,
    KEY_GRAVE_ACCENT = 96,
    
    /* Modifier flags (combined with key codes) */
    KEY_MOD_SHIFT   = 1024,
    KEY_MOD_CONTROL = 2048,
    KEY_MOD_ALT     = 4096
} EngineKeyCode;

/** Mouse button constants */
typedef enum {
    MOUSE_LEFT = 0,
    MOUSE_MIDDLE = 1,
    MOUSE_RIGHT = 2,
    MOUSE_BUTTON_COUNT = 3
} EngineMouseButton;

/** Represents a single game event */
typedef struct {
    EngineEventType type;
    
    /* Keyboard data */
    int keyCode;
    int character;        /* ASCII character if applicable */
    int modifiers;        /* Shift, Ctrl, Alt flags */
    
    /* Mouse data */
    int mouseX;
    int mouseY;
    int mouseButton;
    
    /* Window data */
    int windowWidth;
    int windowHeight;
    
    /* Timestamp (milliseconds since engine start) */
    unsigned long timestamp;
} EngineEvent;

/** Game state callback types */
typedef void (*EngineUpdateFunc)(float deltaTime);
typedef void (*EngineRenderFunc)(void);
typedef void (*EngineCleanupFunc)(void);

/* ============================================================
 * GAME STATE MANAGER
 * ============================================================ */

/** Represents a single game state (scene, menu, etc.) */
typedef struct {
    EngineUpdateFunc update;
    EngineRenderFunc render;
    EngineCleanupFunc cleanup;
    char name[64];
} GameState;

/* ============================================================
 * PUBLIC API - INITIALIZATION & LIFECYCLE
 * ============================================================ */

/**
 * Initialize the game engine.
 * 
 * @param title         Window/terminal title
 * @param screenWidth   Terminal width in characters (0 = default 80)
 * @param screenHeight  Terminal height in characters (0 = default 24)
 */
void Engine_Init(const char* title, int screenWidth, int screenHeight);

/**
 * Shutdown the engine and clean up all resources.
 */
void Engine_Shutdown(void);

/* ============================================================
 * PUBLIC API - GAME LOOP CONTROL
 * ============================================================ */

/**
 * Start the main game loop. This function blocks until the game exits.
 */
void Engine_Run(void);

/**
 * Request the engine to stop running.
 * 
 * @param running 1 to continue, 0 to quit
 */
void Engine_SetRunning(int running);

/**
 * Check if the engine is currently running.
 * 
 * @return 1 if running, 0 otherwise
 */
int Engine_IsRunning(void);

/* ============================================================
 * PUBLIC API - EVENT SYSTEM
 * ============================================================ */

/**
 * Poll for the next event from the input queue.
 * 
 * @param event Pointer to an EngineEvent struct to fill
 * @return 1 if an event was available, 0 if queue is empty
 */
int Engine_PollEvent(EngineEvent* event);

/**
 * Clear all pending events from the queue.
 */
void Engine_ClearEvents(void);

/* ============================================================
 * PUBLIC API - STATE MANAGEMENT
 * ============================================================ */

/**
 * Push a new game state onto the stack.
 * 
 * @param name      Human-readable name for debugging
 * @param update    Update callback function (can be NULL)
 * @param render    Render callback function (required)
 * @param cleanup   Cleanup callback (can be NULL, called when popped)
 */
void Engine_PushState(const char* name, EngineUpdateFunc update, 
                      EngineRenderFunc render, EngineCleanupFunc cleanup);

/**
 * Pop the current game state from the stack.
 */
void Engine_PopState(void);

/**
 * Clear all game states and reset to empty.
 */
void Engine_ClearStates(void);

/**
 * Get the index of the currently active state.
 * 
 * @return Current state index, or -1 if no states exist
 */
int Engine_GetCurrentStateIndex(void);

/* ============================================================
 * PUBLIC API - UTILITY FUNCTIONS
 * ============================================================ */

/**
 * Get the current engine tick count (milliseconds approximation).
 * 
 * @return Milliseconds since engine started
 */
unsigned long Engine_GetTickCount(void);

/**
 * Print a formatted message to the console.
 */
void Engine_Log(const char* format, ...);

/* ============================================================
 * INTERNAL / PLATFORM-SPECIFIC FUNCTIONS
 * ============================================================ */

/* Platform-specific input initialization */
void Engine_Input_Init(int width, int height);

/* Platform-specific input update */
void Engine_Input_Update(void);

/* Platform-specific shutdown */
void Engine_Input_Shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_H */
