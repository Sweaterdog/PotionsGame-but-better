/**
 * @file render.cpp
 * @brief Terminal Rendering System Implementation
 * 
 * Implements double-buffered terminal rendering using ANSI escape codes.
 * Works on all modern terminals (Linux, macOS, Windows 10+).
 */

#define _CRT_SECURE_NO_WARNINGS

#include "render.h"
#include <stdarg.h>

/* ============================================================
 * INTERNAL STATE
 * ============================================================ */

static RenderScreen g_screen = {0};
static int g_renderInitialized = 0;

/* Box drawing Unicode characters */
#define BOX_TOP_LEFT     "\u250C"   /* ┌ */
#define BOX_TOP_RIGHT    "\u2510"   /* ┐ */
#define BOX_BOTTOM_LEFT  "\u2514"   /* └ */
#define BOX_BOTTOM_RIGHT "\u2518"   /* ┘ */
#define BOX_HORIZ        "\u2500"   /* ─ */
#define BOX_VERT         "\u2502"   /* │ */
#define BOX_CROSS        "\u253C"   /* ┼ */

/* ============================================================
 * INTERNAL HELPERS
 * ============================================================ */

/** Clamp value to range [min, max] */
static inline int Clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

/** Initialize a single cell */
static void Cell_Init(RenderCell* cell) {
    cell->character = ' ';
    cell->foregroundColor = COLOR_DEFAULT;
    cell->backgroundColor = COLOR_BLACK;
    cell->bold = 0;
    cell->underline = 0;
}

/* ============================================================
 * INITIALIZATION & SHUTDOWN
 * ============================================================ */

void Render_Init(int width, int height) {
    g_screen.width = Clamp(width, 10, RENDER_MAX_WIDTH);
    g_screen.height = Clamp(height, 5, RENDER_MAX_HEIGHT);
    g_screen.cursorX = 0;
    g_screen.cursorY = 0;
    
    /* Initialize all cells */
    for (int y = 0; y < g_screen.height; y++) {
        for (int x = 0; x < g_screen.width; x++) {
            Cell_Init(&g_screen.cells[y][x]);
        }
    }
    
    g_renderInitialized = 1;
    
    /* Hide cursor and clear screen */
    printf("\033[?25l");   /* Hide cursor */
    printf("\033[2J");     /* Clear screen */
    printf("\033[H");      /* Move cursor to home */
    fflush(stdout);
}

void Render_Shutdown(void) {
    if (g_renderInitialized) {
        printf("\033[?25h");   /* Show cursor */
        printf("\033[0m");     /* Reset all attributes */
        fflush(stdout);
        g_renderInitialized = 0;
    }
}

/* ============================================================
 * CLEAR & PRESENT
 * ============================================================ */

void Render_Clear(RenderColor bgColor) {
    for (int y = 0; y < g_screen.height; y++) {
        for (int x = 0; x < g_screen.width; x++) {
            Cell_Init(&g_screen.cells[y][x]);
            g_screen.cells[y][x].backgroundColor = (int)bgColor;
        }
    }
    
    g_screen.cursorX = 0;
    g_screen.cursorY = 0;
}

void Render_Present(void) {
    if (!g_renderInitialized) return;
    
    /* Move cursor to home position */
    printf("\033[H");
    
    int lastColor = -1;
    int lastBgColor = -1;
    int lastBold = -1;
    
    for (int y = 0; y < g_screen.height; y++) {
        /* Start of line - reset and set background */
        printf("\033[0m");
        
        if (g_screen.cells[y][0].backgroundColor != COLOR_BLACK || lastBgColor != 0) {
            printf("\033[%dm", g_screen.cells[y][0].backgroundColor + 10); /* Background */
            lastBgColor = g_screen.cells[y][0].backgroundColor;
        }
        
        for (int x = 0; x < g_screen.width; x++) {
            RenderCell* cell = &g_screen.cells[y][x];
            
            /* Skip empty cells with default colors (optimization) */
            if (cell->character == ' ' && 
                cell->foregroundColor == COLOR_DEFAULT &&
                cell->backgroundColor == COLOR_BLACK &&
                !cell->bold && !cell->underline) {
                continue;
            }
            
            /* Set foreground color if changed */
            if (cell->foregroundColor != lastColor) {
                printf("\033[%dm", cell->foregroundColor);
                lastColor = cell->foregroundColor;
            }
            
            /* Set bold if changed */
            if (cell->bold != lastBold) {
                printf("%s", cell->bold ? "\033[1m" : "");
                lastBold = cell->bold;
            }
            
            /* Print character */
            if (cell->character != ' ') {
                printf("%s", &cell->character);
            }
        }
        
        /* Newline at end of row */
        printf("\n");
    }
    
    fflush(stdout);
}

/* ============================================================
 * CURSOR CONTROL
 * ============================================================ */

void Render_SetPosition(int x, int y) {
    x = Clamp(x, 0, g_screen.width - 1);
    y = Clamp(y, 0, g_screen.height - 1);
    
    printf("\033[%d;%df", y + 1, x + 1);
    fflush(stdout);
    
    g_screen.cursorX = x;
    g_screen.cursorY = y;
}

void Render_HideCursor(void) {
    printf("\033[?25l");
    fflush(stdout);
}

void Render_ShowCursor(void) {
    printf("\033[?25h");
    fflush(stdout);
}

/* ============================================================
 * TEXT OUTPUT
 * ============================================================ */

static void SetCellChar(int x, int y, char c, RenderColor color, int bold) {
    if (x < 0 || x >= g_screen.width || y < 0 || y >= g_screen.height) return;
    
    RenderCell* cell = &g_screen.cells[y][x];
    cell->character = c;
    cell->foregroundColor = (int)color;
    cell->bold = bold;
}

void Render_Print(const char* text) {
    if (!text || !g_renderInitialized) return;
    
    int x = g_screen.cursorX;
    int y = g_screen.cursorY;
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (x >= g_screen.width) {
            x = 0;
            y++;
            if (y >= g_screen.height) break;
        }
        
        SetCellChar(x, y, text[i], COLOR_WHITE, 0);
        x++;
    }
    
    g_screen.cursorX = x;
    g_screen.cursorY = y;
}

void Render_PrintColored(const char* text, RenderColor color) {
    if (!text || !g_renderInitialized) return;
    
    int x = g_screen.cursorX;
    int y = g_screen.cursorY;
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (x >= g_screen.width) {
            x = 0;
            y++;
            if (y >= g_screen.height) break;
        }
        
        SetCellChar(x, y, text[i], color, 0);
        x++;
    }
    
    g_screen.cursorX = x;
    g_screen.cursorY = y;
}

void Render_PrintBold(const char* text) {
    if (!text || !g_renderInitialized) return;
    
    int x = g_screen.cursorX;
    int y = g_screen.cursorY;
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (x >= g_screen.width) {
            x = 0;
            y++;
            if (y >= g_screen.height) break;
        }
        
        SetCellChar(x, y, text[i], COLOR_WHITE, 1);
        x++;
    }
    
    g_screen.cursorX = x;
    g_screen.cursorY = y;
}

void Render_PrintBoldColored(const char* text, RenderColor color) {
    if (!text || !g_renderInitialized) return;
    
    int x = g_screen.cursorX;
    int y = g_screen.cursorY;
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (x >= g_screen.width) {
            x = 0;
            y++;
            if (y >= g_screen.height) break;
        }
        
        SetCellChar(x, y, text[i], color, 1);
        x++;
    }
    
    g_screen.cursorX = x;
    g_screen.cursorY = y;
}

void Render_Printf(const char* format, ...) {
    if (!format || !g_renderInitialized) return;
    
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Render_Print(buffer);
}

void Render_PrintfColored(RenderColor color, const char* format, ...) {
    if (!format || !g_renderInitialized) return;
    
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Render_PrintColored(buffer, color);
}

void Render_PrintfBoldColored(RenderColor color, const char* format, ...) {
    if (!format || !g_renderInitialized) return;
    
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    Render_PrintBoldColored(buffer, color);
}

/* ============================================================
 * BOX/PANEL DRAWING
 * ============================================================ */

void Render_DrawBox(int x, int y, int width, int height, RenderColor borderColor) {
    if (width < 2 || height < 2) return;
    
    /* Draw corners */
    SetCellChar(x, y, BOX_TOP_LEFT[0], borderColor, 0);
    SetCellChar(x + width - 1, y, BOX_TOP_RIGHT[0], borderColor, 0);
    SetCellChar(x, y + height - 1, BOX_BOTTOM_LEFT[0], borderColor, 0);
    SetCellChar(x + width - 1, y + height - 1, BOX_BOTTOM_RIGHT[0], borderColor, 0);
    
    /* Draw horizontal lines */
    for (int i = 1; i < width - 1; i++) {
        SetCellChar(x + i, y, BOX_HORIZ[0], borderColor, 0);
        SetCellChar(x + i, y + height - 1, BOX_HORIZ[0], borderColor, 0);
    }
    
    /* Draw vertical lines */
    for (int i = 1; i < height - 1; i++) {
        SetCellChar(x, y + i, BOX_VERT[0], borderColor, 0);
        SetCellChar(x + width - 1, y + i, BOX_VERT[0], borderColor, 0);
    }
}

void Render_DrawPanel(int x, int y, int width, int height, RenderColor bgColor) {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (y + row >= 0 && y + row < g_screen.height &&
                x + col >= 0 && x + col < g_screen.width) {
                
                RenderCell* cell = &g_screen.cells[y + row][x + col];
                cell->character = ' ';
                cell->backgroundColor = (int)bgColor;
            }
        }
    }
}

void Render_DrawPanelWithBorder(int x, int y, int width, int height, 
                                 RenderColor bgColor, RenderColor borderColor) {
    /* Fill background */
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (y + row >= 0 && y + row < g_screen.height &&
                x + col >= 0 && x + col < g_screen.width) {
                
                RenderCell* cell = &g_screen.cells[y + row][x + col];
                cell->character = ' ';
                cell->backgroundColor = (int)bgColor;
            }
        }
    }
    
    /* Draw border */
    for (int i = 0; i < width; i++) {
        if (y >= 0 && y < g_screen.height && x + i >= 0 && x + i < g_screen.width) {
            SetCellChar(x + i, y, BOX_HORIZ[0], borderColor, 0);
        }
        if (y + height - 1 >= 0 && y + height - 1 < g_screen.height && 
            x + i >= 0 && x + i < g_screen.width) {
            SetCellChar(x + i, y + height - 1, BOX_HORIZ[0], borderColor, 0);
        }
    }
    
    for (int i = 0; i < height; i++) {
        if (x >= 0 && x < g_screen.width && y + i >= 0 && y + i < g_screen.height) {
            SetCellChar(x, y + i, BOX_VERT[0], borderColor, 0);
        }
        if (x + width - 1 >= 0 && x + width - 1 < g_screen.width &&
            y + i >= 0 && y + i < g_screen.height) {
            SetCellChar(x + width - 1, y + i, BOX_VERT[0], borderColor, 0);
        }
    }
    
    /* Corners */
    if (y >= 0 && x >= 0 && y < g_screen.height && x < g_screen.width)
        SetCellChar(x, y, BOX_TOP_LEFT[0], borderColor, 0);
    if (y >= 0 && x + width - 1 >= 0 && y < g_screen.height && x + width - 1 < g_screen.width)
        SetCellChar(x + width - 1, y, BOX_TOP_RIGHT[0], borderColor, 0);
    if (y + height - 1 >= 0 && x >= 0 && y + height - 1 < g_screen.height && x < g_screen.width)
        SetCellChar(x, y + height - 1, BOX_BOTTOM_LEFT[0], borderColor, 0);
    if (y + height - 1 >= 0 && x + width - 1 >= 0 && y + height - 1 < g_screen.height && 
        x + width - 1 < g_screen.width)
        SetCellChar(x + width - 1, y + height - 1, BOX_BOTTOM_RIGHT[0], borderColor, 0);
}

void Render_DrawChar(int x, int y, const char* boxChar, RenderColor color) {
    if (boxChar && strlen(boxChar) > 0) {
        SetCellChar(x, y, boxChar[0], color, 0);
    }
}

void Render_DrawHLine(int x1, int x2, int y, RenderColor color) {
    if (y < 0 || y >= g_screen.height) return;
    
    int start = Clamp(x1, 0, g_screen.width - 1);
    int end = Clamp(x2, 0, g_screen.width - 1);
    
    for (int x = start; x <= end; x++) {
        SetCellChar(x, y, BOX_HORIZ[0], color, 0);
    }
}

void Render_DrawVLine(int y1, int y2, int x, RenderColor color) {
    if (x < 0 || x >= g_screen.width) return;
    
    int start = Clamp(y1, 0, g_screen.height - 1);
    int end = Clamp(y2, 0, g_screen.height - 1);
    
    for (int y = start; y <= end; y++) {
        SetCellChar(x, y, BOX_VERT[0], color, 0);
    }
}

/* ============================================================
 * UTILITY
 * ============================================================ */

int Render_GetCursorX(void) {
    return g_screen.cursorX;
}

int Render_GetCursorY(void) {
    return g_screen.cursorY;
}

void Render_ScrollUp(int lines) {
    if (lines <= 0 || lines >= g_screen.height) {
        /* Full clear */
        Render_Clear(COLOR_BLACK);
        return;
    }
    
    /* Shift all rows up by 'lines' */
    for (int y = 0; y < g_screen.height - lines; y++) {
        for (int x = 0; x < g_screen.width; x++) {
            g_screen.cells[y][x] = g_screen.cells[y + lines][x];
        }
    }
    
    /* Clear the bottom 'lines' rows */
    for (int y = g_screen.height - lines; y < g_screen.height; y++) {
        for (int x = 0; x < g_screen.width; x++) {
            Cell_Init(&g_screen.cells[y][x]);
        }
    }
}
