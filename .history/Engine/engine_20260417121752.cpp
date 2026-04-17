/**
 * @file engine.cpp
 * @brief Universal Terminal-Based Game Engine - Core Implementation
 *
 * Implements the main game loop, event system, state management,
 * and cross-platform input handling.
 */

#define _CRT_SECURE_NO_WARNINGS
#define _POSIX_C_SOURCE 199309L

#include <time.h>

#include "engine.h"

/* ============================================================
 * PLATFORM-SPECIFIC INCLUDES
 * ============================================================ */

#if defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_MACOS) || defined(ENGINE_PLATFORM_UNIX)

    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <sys/select.h>
    #include <errno.h>
    
    /* Unix terminal state */
    static struct termios g_originalTermios;
    static int g_isTerminalRaw = 0;

#elif defined(ENGINE_PLATFORM_WINDOWS)

    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    
    /* Windows console handles */
    static HANDLE g_inputHandle = NULL;
    static HANDLE g_outputHandle = NULL;

#endif

/* ============================================================
 * INTERNAL STATE
 * ============================================================ */

/** Engine global state */
typedef struct {
    int running;
    char title[ENGINE_TITLE_LENGTH];
    int screenWidth;
    int screenHeight;
    
    /* Game states */
    GameState states[ENGINE_MAX_STATES];
    int stateCount;
    int currentStateIndex;
    
    /* Event queue */
    EngineEvent eventQueue[ENGINE_MAX_EVENTS];
    int eventHead;
    int eventTail;
    int eventCount;
    
    /* Timing */
    unsigned long startTime;
    float deltaTime;
    unsigned long lastFrameTime;
    
    /* Input state */
    int keyStates[1024];
    int prevKeyStates[1024];
    int mouseButtons[MOUSE_BUTTON_COUNT];
    int mouseX;
    int mouseY;
    int mouseWheel;
    
    /* ANSI support (Windows 10+ may need this enabled) */
    int supportsANSI;
} EngineState;

static EngineState g_engine = {0};

/* ============================================================
 * TIMING UTILITIES
 * ============================================================ */

/** Get current time in milliseconds (platform-independent) */
static unsigned long GetCurrentTimeMs(void) {
#if defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_MACOS) || defined(ENGINE_PLATFORM_UNIX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned long)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#elif defined(ENGINE_PLATFORM_WINDOWS)
    static LARGE_INTEGER freq = {0};
    if (freq.QuadPart == 0) {
        QueryPerformanceFrequency(&freq);
    }
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (unsigned long)((counter.QuadPart * 1000.0) / freq.QuadPart);
#else
    /* Fallback: use time() which has second precision */
    return (unsigned long)time(NULL) * 1000;
#endif
}

/* ============================================================
 * EVENT QUEUE MANAGEMENT
 * ============================================================ */

static void Engine_PushEvent(const EngineEvent* event) {
    if (g_engine.eventCount >= ENGINE_MAX_EVENTS) {
        /* Queue full - drop oldest events */
        g_engine.eventHead = (g_engine.eventHead + 1) % ENGINE_MAX_EVENTS;
        g_engine.eventCount--;
    }
    
    g_engine.eventQueue[g_engine.eventTail] = *event;
    g_engine.eventTail = (g_engine.eventTail + 1) % ENGINE_MAX_EVENTS;
    g_engine.eventCount++;
}

/* ============================================================
 * PLATFORM-SPECIFIC INPUT: UNIX (Linux, macOS)
 * ============================================================ */

#if defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_MACOS) || defined(ENGINE_PLATFORM_UNIX)

/** Set terminal to raw mode for immediate key input */
static void SetRawMode(void) {
    if (g_isTerminalRaw) return;
    
    tcgetattr(STDIN_FILENO, &g_originalTermios);
    struct termios raw = g_originalTermios;
    
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_cflag |= (CS8);
    raw.c_cc[VMIN] = 0;   /* Non-blocking */
    raw.c_cc[VTIME] = 0;
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) >= 0) {
        g_isTerminalRaw = 1;
    }
}

/** Restore terminal to original mode */
static void RestoreMode(void) {
    if (!g_isTerminalRaw) return;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &g_originalTermios);
    g_isTerminalRaw = 0;
}

/** Signal handler for clean shutdown on Ctrl+C */
static void SignalHandler(int signal) {
    (void)signal;
    Engine_SetRunning(0);
    RestoreMode();
}

/** Parse ANSI escape sequence for special keys and mouse */
static int ParseANSISequence(char* buffer, int bufferSize, EngineEvent* event) {
    /* We already consumed the ESC character */
    if (bufferSize < 1) return 0;
    
    char second = buffer[0];
    
    /* Check for mouse event: ESC [ M ... */
    if (second == '[' && bufferSize >= 3) {
        char third = buffer[1];
        if (third == 'M') {
            /* X10 mouse encoding: ESC [ M Cb Cx Cy */
            if (bufferSize >= 4) {
                int cb = buffer[2] - 32;
                int cx = buffer[3] - 32;
                int cy = buffer[4] - 32;
                
                event->type = ENGINE_EVENT_MOUSE_BUTTON_DOWN;
                event->mouseX = cx - 1;
                event->mouseY = cy - 1;
                
                /* Decode button */
                if (cb & 0x03) {
                    event->mouseButton = MOUSE_LEFT;
                } else if (cb & 0x04) {
                    event->mouseButton = MOUSE_MIDDLE;
                } else {
                    event->mouseButton = MOUSE_RIGHT;
                }
                
                /* Scroll up/down */
                if (cb & 0x40) {
                    event->type = ENGINE_EVENT_MOUSE_BUTTON_UP;
                }
                
                return 5; /* Total bytes consumed: ESC + [ + M + Cb + Cx + Cy */
            }
        }
    }
    
    /* Arrow keys and special keys: ESC [ A/B/C/D etc. */
    if (second == '[') {
        if (bufferSize >= 1) {
            char third = buffer[1];
            switch (third) {
                case 'A': event->keyCode = KEY_UP; break;
                case 'B': event->keyCode = KEY_DOWN; break;
                case 'C': event->keyCode = KEY_RIGHT; break;
                case 'D': event->keyCode = KEY_LEFT; break;
                case 'F': event->keyCode = KEY_END; break;
                case 'H': event->keyCode = KEY_HOME; break;
                case '2': event->keyCode = KEY_INSERT; break;
                case '3': event->keyCode = KEY_DELETE; break;
                case '5': event->keyCode = KEY_PAGE_UP; break;
                case '6': event->keyCode = KEY_PAGE_DOWN; break;
                default: return 0; /* Unknown sequence */
            }
            event->type = ENGINE_EVENT_KEY_PRESS;
            return 3; /* ESC + [ + key char */
        }
    }
    
    return 0;
}

/** Read a single input event on Unix */
static int Unix_ReadEvent(EngineEvent* event) {
    fd_set fds;
    struct timeval timeout;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    
    timeout.tv_sec = 0;
    timeout.tv_usec = 1; /* 1ms timeout for responsive input */
    
    int ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout);
    if (ret <= 0) return 0; /* No data or error */
    
    char buffer[64];
    int bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer));
    if (bytesRead <= 0) return 0;
    
    event->timestamp = Engine_GetTickCount();
    event->windowWidth = g_engine.screenWidth;
    event->windowHeight = g_engine.screenHeight;
    
    /* Check for escape sequence */
    if (buffer[0] == '\x1B' || buffer[0] == '\e') {
        return ParseANSISequence(buffer, bytesRead, event);
    }
    
    /* Regular character */
    unsigned char ch = (unsigned char)buffer[0];
    
    switch (ch) {
        case 0x03: /* Ctrl+C */
            event->type = ENGINE_EVENT_QUIT;
            event->keyCode = KEY_ESCAPE;
            return 1;
        
        case 0x0D: /* Enter */
        case 0x0A: /* Newline */
            event->type = ENGINE_EVENT_KEY_PRESS;
            event->keyCode = KEY_ENTER;
            event->character = '\n';
            break;
        
        case 0x08: /* Backspace */
        case 0x7F: /* Delete */
            event->type = ENGINE_EVENT_KEY_PRESS;
            event->keyCode = KEY_BACKSPACE;
            event->character = ch;
            break;
        
        case '\t': /* Tab */
            event->type = ENGINE_EVENT_KEY_PRESS;
            event->keyCode = KEY_TAB;
            event->character = ch;
            break;
        
        default:
            if (ch < 128) {
                event->type = ENGINE_EVENT_KEY_PRESS;
                event->keyCode = ch;
                event->character = ch;
            }
            break;
    }
    
    return bytesRead;
}

#elif defined(ENGINE_PLATFORM_WINDOWS)

/** Read a single input event on Windows */
static int Windows_ReadEvent(EngineEvent* event) {
    INPUT_RECORD record;
    DWORD eventsRead;
    
    if (!PeekConsoleInput(g_inputHandle, &record, 1, &eventsRead)) {
        return 0;
    }
    
    if (eventsRead == 0) return 0;
    
    if (ReadConsoleInput(g_inputHandle, &record, 1, &eventsRead)) {
        event->timestamp = Engine_GetTickCount();
        event->windowWidth = g_engine.screenWidth;
        event->windowHeight = g_engine.screenHeight;
        
        if (record.EventType == KEY_EVENT) {
            KEY_EVENT_RECORD keyEvent = record.Event.KeyEvent;
            
            /* Skip repeat events for cleaner input handling */
            if (keyEvent.bKeyDown) {
                switch (keyEvent.wVirtualKeyCode) {
                    case VK_ESCAPE:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_ESCAPE;
                        break;
                    
                    case VK_RETURN:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_ENTER;
                        event->character = '\n';
                        break;
                    
                    case VK_TAB:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_TAB;
                        break;
                    
                    case VK_BACK:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_BACKSPACE;
                        event->character = 8;
                        break;
                    
                    case VK_DELETE:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_DELETE;
                        break;
                    
                    case VK_INSERT:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_INSERT;
                        break;
                    
                    case VK_HOME:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_HOME;
                        break;
                    
                    case VK_END:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_END;
                        break;
                    
                    case VK_PRIOR:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_PAGE_UP;
                        break;
                    
                    case VK_NEXT:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_PAGE_DOWN;
                        break;
                    
                    case VK_UP:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_UP;
                        break;
                    
                    case VK_DOWN:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_DOWN;
                        break;
                    
                    case VK_LEFT:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_LEFT;
                        break;
                    
                    case VK_RIGHT:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_RIGHT;
                        break;
                    
                    case VK_F1: case VK_F2: case VK_F3: case VK_F4:
                    case VK_F5: case VK_F6: case VK_F7: case VK_F8:
                    case VK_F9: case VK_F10: case VK_F11: case VK_F12:
                        event->type = ENGINE_EVENT_KEY_PRESS;
                        event->keyCode = KEY_F1 + (keyEvent.wVirtualKeyCode - VK_F1);
                        break;
                    
                    default: {
                        /* Convert virtual key to ASCII */
                        BYTE keystate[256];
                        GetKeyboardState(keystate);
                        WCHAR wideChar;
                        UINT scanCode = MapVirtualKey(keyEvent.wVirtualKeyCode, MAPVK_VK_TO_VST);
                        
                        if (ToUnicode(keyEvent.wVirtualKeyCode, scanCode, keystate, &wideChar, 1, 0) > 0) {
                            event->type = ENGINE_EVENT_KEY_PRESS;
                            event->keyCode = (int)wideChar;
                            event->character = (int)wideChar;
                        }
                        break;
                    }
                }
            } else {
                /* Key release */
                event->type = ENGINE_EVENT_KEY_RELEASE;
                event->keyCode = event->keyCode; /* Preserve from keydown if needed */
            }
            
            return 1;
        }
        
        if (record.EventType == MOUSE_EVENT) {
            MOUSE_EVENT_RECORD mouseEvent = record.Event.MouseEvent;
            
            switch (mouseEvent.dwEventFlags) {
                case 0: /* Regular mouse click */
                    event->type = ENGINE_EVENT_MOUSE_BUTTON_DOWN;
                    event->mouseX = mouseEvent.dwMousePosition.X;
                    event->mouseY = mouseEvent.dwMousePosition.Y;
                    
                    if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                        event->mouseButton = MOUSE_LEFT;
                    } else if (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
                        event->mouseButton = MOUSE_RIGHT;
                    }
                    break;
                
                case MOUSE_WHEELED:
                    if (mouseEvent.dwButtonState > 0) {
                        event->type = ENGINE_EVENT_MOUSE_BUTTON_DOWN;
                        event->mouseButton = MOUSE_MIDDLE;
                        /* Scroll up */
                    } else {
                        event->type = ENGINE_EVENT_MOUSE_BUTTON_UP;
                        event->mouseButton = MOUSE_MIDDLE;
                        /* Scroll down */
                    }
                    break;
                
                case MOUSE_MOVED:
                    event->type = ENGINE_EVENT_MOUSE_MOVE;
                    event->mouseX = mouseEvent.dwMousePosition.X;
                    event->mouseY = mouseEvent.dwMousePosition.Y;
                    break;
            }
            
            return 1;
        }
    }
    
    return 0;
}

#endif /* Platform-specific input */

/* ============================================================
 * ENGINE INITIALIZATION & SHUTDOWN
 * ============================================================ */

void Engine_Init(const char* title, int screenWidth, int screenHeight) {
    /* Store engine parameters */
    if (title) {
        strncpy(g_engine.title, title, ENGINE_TITLE_LENGTH - 1);
        g_engine.title[ENGINE_TITLE_LENGTH - 1] = '\0';
    } else {
        strncpy(g_engine.title, "Game Engine", ENGINE_TITLE_LENGTH - 1);
    }
    
    g_engine.screenWidth = (screenWidth > 0) ? screenWidth : ENGINE_DEFAULT_WIDTH;
    g_engine.screenHeight = (screenHeight > 0) ? screenHeight : ENGINE_DEFAULT_HEIGHT;
    
    /* Initialize state */
    g_engine.running = 1;
    g_engine.stateCount = 0;
    g_engine.currentStateIndex = -1;
    
    /* Initialize event queue */
    g_engine.eventHead = 0;
    g_engine.eventTail = 0;
    g_engine.eventCount = 0;
    
    /* Initialize timing */
    g_engine.startTime = GetCurrentTimeMs();
    g_engine.lastFrameTime = g_engine.startTime;
    g_engine.deltaTime = 0.0f;
    
    /* Initialize input state */
    memset(g_engine.keyStates, 0, sizeof(g_engine.keyStates));
    memset(g_engine.prevKeyStates, 0, sizeof(g_engine.prevKeyStates));
    memset(g_engine.mouseButtons, 0, sizeof(g_engine.mouseButtons));
    g_engine.mouseX = 0;
    g_engine.mouseY = 0;
    g_engine.mouseWheel = 0;
    
    /* Platform-specific initialization */
#if defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_MACOS) || defined(ENGINE_PLATFORM_UNIX)
    /* Set up signal handler for clean shutdown */
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    
    /* Enable ANSI colors on modern terminals */
    g_engine.supportsANSI = 1;
    
    /* Flush any stale input from terminal buffer (important for VSCode/IDE terminals) */
    tcflush(STDIN_FILENO, TCIFLUSH);
    
    /* Set terminal to raw mode */
    SetRawMode();
    
#elif defined(ENGINE_PLATFORM_WINDOWS)
    /* Get console handles */
    g_inputHandle = GetStdHandle(STD_INPUT_HANDLE);
    g_outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    
    /* Enable VT100 escaping for ANSI colors on Windows 10+ */
    DWORD mode;
    GetConsoleMode(g_outputHandle, &mode);
    SetConsoleMode(g_outputHandle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    
    g_engine.supportsANSI = 1;
#endif
    
    Engine_Log("Engine initialized: %dx%d - '%s'", 
               g_engine.screenWidth, g_engine.screenHeight, g_engine.title);
}

void Engine_Shutdown(void) {
    /* Clean up all states */
    Engine_ClearStates();
    
    /* Platform-specific cleanup */
#if defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_MACOS) || defined(ENGINE_PLATFORM_UNIX)
    RestoreMode();
#elif defined(ENGINE_PLATFORM_WINDOWS)
    if (g_inputHandle) {
        /* Restore console mode */
        DWORD mode;
        GetConsoleMode(g_inputHandle, &mode);
        SetConsoleMode(g_inputHandle, mode & ~ENABLE_QUICK_EDIT_MODE);
    }
#endif
    
    Engine_Log("Engine shutdown complete.");
}

/* ============================================================
 * GAME LOOP
 * ============================================================ */

/** Flush all pending input from stdin */
static void Engine_FlushInput(void) {
#if defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_MACOS) || defined(ENGINE_PLATFORM_UNIX)
    tcflush(STDIN_FILENO, TCIFLUSH);
#endif
}

void Engine_Run(void) {
    /* Flush any input that might have accumulated during startup */
    Engine_FlushInput();
    
    while (g_engine.running) {
        /* Calculate delta time */
        unsigned long currentTime = GetCurrentTimeMs();
        g_engine.deltaTime = (float)(currentTime - g_engine.lastFrameTime) / 1000.0f;
        g_engine.lastFrameTime = currentTime;
        
        /* Cap delta time to prevent spiral of death */
        if (g_engine.deltaTime > 0.1f) {
            g_engine.deltaTime = 0.016f; /* ~60fps default */
        }
        
        /* Process events */
        EngineEvent event;
        while (Engine_PollEvent(&event)) {
            switch (event.type) {
                case ENGINE_EVENT_QUIT:
                case ENGINE_EVENT_KEY_PRESS:
                    if (event.keyCode == KEY_ESCAPE) {
                        g_engine.running = 0;
                    }
                    break;
                    
                case ENGINE_EVENT_MOUSE_BUTTON_DOWN:
                    g_engine.mouseButtons[event.mouseButton] = 1;
                    break;
                    
                case ENGINE_EVENT_MOUSE_BUTTON_UP:
                    g_engine.mouseButtons[event.mouseButton] = 0;
                    break;
                    
                case ENGINE_EVENT_MOUSE_MOVE:
                    g_engine.mouseX = event.mouseX;
                    g_engine.mouseY = event.mouseY;
                    break;
                    
                default:
                    break;
            }
        }
        
        /* Update current state */
        if (g_engine.currentStateIndex >= 0 && 
            g_engine.currentStateIndex < g_engine.stateCount) {
            
            GameState* currentState = &g_engine.states[g_engine.currentStateIndex];
            
            if (currentState->update) {
                currentState->update(g_engine.deltaTime);
            }
        }
        
        /* Render current state */
        if (g_engine.currentStateIndex >= 0 && 
            g_engine.currentStateIndex < g_engine.stateCount) {
            
            GameState* currentState = &g_engine.states[g_engine.currentStateIndex];
            
            if (currentState->render) {
                currentState->render();
            }
        }
        
        /* Target ~60 FPS */
        unsigned long frameTime = (unsigned long)(1000.0f / ENGINE_TICKS_PER_SECOND);
        unsigned long elapsed = GetCurrentTimeMs() - currentTime;
        if (elapsed < frameTime) {
#if defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_MACOS) || defined(ENGINE_PLATFORM_UNIX)
            usleep((frameTime - elapsed) * 1000);
#elif defined(ENGINE_PLATFORM_WINDOWS)
            Sleep(frameTime - elapsed);
#endif
        }
    }
}

void Engine_SetRunning(int running) {
    g_engine.running = (running != 0) ? 1 : 0;
}

int Engine_IsRunning(void) {
    return g_engine.running;
}

/* ============================================================
 * EVENT SYSTEM
 * ============================================================ */

int Engine_PollEvent(EngineEvent* event) {
    if (g_engine.eventCount <= 0) {
        /* Try to read from input */
#if defined(ENGINE_PLATFORM_LINUX) || defined(ENGINE_PLATFORM_MACOS) || defined(ENGINE_PLATFORM_UNIX)
        int bytesRead = Unix_ReadEvent(event);
#elif defined(ENGINE_PLATFORM_WINDOWS)
        int bytesRead = Windows_ReadEvent(event);
#else
        int bytesRead = 0;
#endif
        
        if (bytesRead > 0) {
            /* Update key states */
            if (event->type == ENGINE_EVENT_KEY_PRESS || 
                event->type == ENGINE_EVENT_KEY_RELEASE) {
                
                g_engine.prevKeyStates[event->keyCode] = g_engine.keyStates[event->keyCode];
                g_engine.keyStates[event->keyCode] = (event->type == ENGINE_EVENT_KEY_PRESS) ? 1 : 0;
            }
            
            /* Push to queue */
            Engine_PushEvent(event);
            return 1;
        }
        
        return 0;
    }
    
    /* Get from queue */
    *event = g_engine.eventQueue[g_engine.eventHead];
    g_engine.eventHead = (g_engine.eventHead + 1) % ENGINE_MAX_EVENTS;
    g_engine.eventCount--;
    
    return 1;
}

void Engine_ClearEvents(void) {
    g_engine.eventHead = g_engine.eventTail;
    g_engine.eventCount = 0;
}

/* ============================================================
 * STATE MANAGEMENT
 * ============================================================ */

void Engine_PushState(const char* name, EngineUpdateFunc update, 
                      EngineRenderFunc render, EngineCleanupFunc cleanup) {
    
    if (g_engine.stateCount >= ENGINE_MAX_STATES) {
        Engine_Log("WARNING: Maximum state count reached (%d)", ENGINE_MAX_STATES);
        return;
    }
    
    int index = g_engine.stateCount++;
    GameState* state = &g_engine.states[index];
    
    strncpy(state->name, name ? name : "unnamed", 63);
    state->name[63] = '\0';
    state->update = update;
    state->render = render;
    state->cleanup = cleanup;
    
    g_engine.currentStateIndex = index;
    
    Engine_Log("Pushed state: '%s' (index %d)", state->name, index);
}

void Engine_PopState(void) {
    if (g_engine.stateCount <= 0) return;
    
    int index = --g_engine.stateCount;
    GameState* state = &g_engine.states[index];
    
    /* Call cleanup function */
    if (state->cleanup) {
        state->cleanup();
    }
    
    Engine_Log("Popped state: '%s' (index %d)", state->name, index);
    
    /* Set current state to the one below */
    g_engine.currentStateIndex = (g_engine.stateCount > 0) ? (g_engine.stateCount - 1) : -1;
}

void Engine_ClearStates(void) {
    /* Clean up all states */
    for (int i = 0; i < g_engine.stateCount; i++) {
        if (g_engine.states[i].cleanup) {
            g_engine.states[i].cleanup();
        }
    }
    
    g_engine.stateCount = 0;
    g_engine.currentStateIndex = -1;
}

int Engine_GetCurrentStateIndex(void) {
    return g_engine.currentStateIndex;
}

/* ============================================================
 * UTILITY FUNCTIONS
 * ============================================================ */

unsigned long Engine_GetTickCount(void) {
    return GetCurrentTimeMs() - g_engine.startTime;
}

void Engine_Log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "[Engine] ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
}
