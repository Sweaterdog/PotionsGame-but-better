/**
 * @file render.h
 * @brief Terminal Rendering System Header
 * 
 * Provides ASCII/Unicode rendering with ANSI color support,
 * double-buffered screen output, and box-drawing utilities.
 */

#ifndef RENDER_H
#define RENDER_H

#include "engine.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * COLOR DEFINITIONS (ANSI)
 * ============================================================ */

typedef enum {
    COLOR_BLACK = 30,
    COLOR_RED = 31,
    COLOR_GREEN = 32,
    COLOR_YELLOW = 33,
    COLOR_BLUE = 34,
    COLOR_MAGENTA = 35,
    COLOR_CYAN = 36,
    COLOR_WHITE = 37,
    COLOR_DEFAULT = 39
} RenderColor;

typedef enum {
    COLOR_BRIGHT_BLACK = 90,
    COLOR_BRIGHT_RED = 91,
    COLOR_BRIGHT_GREEN = 92,
    COLOR_BRIGHT_YELLOW = 93,
    COLOR_BRIGHT_BLUE = 94,
    COLOR_BRIGHT_MAGENTA = 95,
    COLOR_BRIGHT_CYAN = 96,
    COLOR_BRIGHT_WHITE = 97
} RenderBrightColor;

/* ============================================================
 * SCREEN BUFFER
 * ============================================================ */

#define RENDER_MAX_WIDTH  200
#define RENDER_MAX_HEIGHT 100

/** Single cell in the screen buffer */
typedef struct {
    char character;
    int foregroundColor;
    int backgroundColor;
    int bold;
    int underline;
} RenderCell;

/** Double-buffered screen buffer */
typedef struct {
    RenderCell cells[RENDER_MAX_HEIGHT][RENDER_MAX_WIDTH];
    int width;
    int height;
    int cursorX;
    int cursorY;
} RenderScreen;

/* ============================================================
 * PUBLIC API
 * ============================================================ */

/** Initialize the rendering system */
void Render_Init(int width, int height);

/** Shutdown the rendering system */
void Render_Shutdown(void);

/** Clear the screen with optional color */
void Render_Clear(RenderColor bgColor);

/** Present (flush) the screen buffer to terminal */
void Render_Present(void);

/* --- Cursor Control --- */

/** Set cursor position (0-indexed, character coordinates) */
void Render_SetPosition(int x, int y);

/** Hide/show cursor */
void Render_HideCursor(void);
void Render_ShowCursor(void);

/* --- Text Output --- */

/** Print text at current cursor position */
void Render_Print(const char* text);

/** Print colored text */
void Render_PrintColored(const char* text, RenderColor color);

/** Print bold text */
void Render_PrintBold(const char* text);

/** Print bold colored text */
void Render_PrintBoldColored(const char* text, RenderColor color);

/** Printf-style formatted output */
void Render_Printf(const char* format, ...);

/** Printf with color */
void Render_PrintfColored(RenderColor color, const char* format, ...);

/** Printf with bold colored text */
void Render_PrintfBoldColored(RenderColor color, const char* format, ...);

/* --- Box/Panel Drawing --- */

/** Draw a box outline at position with given dimensions */
void Render_DrawBox(int x, int y, int width, int height, RenderColor borderColor);

/** Draw a filled panel background */
void Render_DrawPanel(int x, int y, int width, int height, RenderColor bgColor);

/** Draw a box with colored background and border */
void Render_DrawPanelWithBorder(int x, int y, int width, int height, 
                                 RenderColor bgColor, RenderColor borderColor);

/* --- Unicode Box Drawing Characters --- */

/** Print a single box-drawing character */
void Render_DrawChar(int x, int y, const char* boxChar, RenderColor color);

/** Draw a horizontal line */
void Render_DrawHLine(int x1, int x2, int y, RenderColor color);

/** Draw a vertical line */
void Render_DrawVLine(int y1, int y2, int x, RenderColor color);

/* --- Utility --- */

/** Get current cursor position */
int  Render_GetCursorX(void);
int  Render_GetCursorY(void);

/** Scroll the screen buffer up by lines count */
void Render_ScrollUp(int lines);

#ifdef __cplusplus
}
#endif

#endif /* RENDER_H */
