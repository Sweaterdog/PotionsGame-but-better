/**
 * @file ui.h
 * @brief TUI Widget Framework Header
 * 
 * Provides composable UI widgets for terminal-based games:
 * buttons, menus, text input fields, panels, and nested layouts.
 */

#ifndef UI_H
#define UI_H

#include "engine.h"
#include "render.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * WIDGET TYPES
 * ============================================================ */

typedef enum {
    UI_WIDGET_BUTTON = 0,
    UI_WIDGET_LABEL,
    UI_WIDGET_TEXT_INPUT,
    UI_WIDGET_MENU_ITEM,
    UI_WIDGET_PANEL,
    UI_WIDGET_SLIDER,
    UI_WIDGET_CHECKBOX,
    UI_WIDGET_PROGRESS_BAR,
    UI_WIDGET_TAB,
    UI_WIDGET_COUNT
} UIWidgetType;

/* ============================================================
 * WIDGET STRUCTURE
 * ============================================================ */

/** Callback function type for widget interactions */
typedef void (*UIOnClickCallback)(void* userData);
typedef void (*UIOnUpdateCallback)(void* userData);

/** Base widget structure - all widgets inherit from this */
typedef struct UIWidget {
    /* Type and identity */
    UIWidgetType type;
    int id;
    
    /* Position and size (character coordinates) */
    int x, y;
    int width, height;
    
    /* Display properties */
    char text[128];
    RenderColor textColor;
    RenderColor bgColor;
    RenderColor borderColor;
    int bold;
    
    /* State */
    int enabled;
    int focused;
    int visible;
    int selected;  /* For menu items, checkboxes */
    
    /* Text input specific */
    char* inputBuffer;
    int inputMaxLength;
    int cursorPos;
    
    /* Slider/progress specific */
    float value;      /* 0.0 to 1.0 */
    float minValue;
    float maxValue;
    
    /* Callbacks */
    UIOnClickCallback onClick;
    UIOnUpdateCallback onUpdate;
    void* userData;
    
    /* Parent-child relationships (for nested menus/panels) */
    struct UIWidget* parent;
    struct UIWidget* nextSibling;
    struct UIWidget* firstChild;
} UIWidget;

/* ============================================================
 * WIDGET CREATION FUNCTIONS
 * ============================================================ */

/** Create a button widget */
UIWidget* UI_CreateButton(int x, int y, const char* text, 
                          UIOnClickCallback onClick, void* userData);

/** Create a label (static text) widget */
UIWidget* UI_CreateLabel(int x, int y, const char* text);

/** Create a text input field widget */
UIWidget* UI_CreateTextInput(int x, int y, int maxWidth, 
                             char* buffer, int bufferSize);

/** Create a menu item widget */
UIWidget* UI_CreateMenuItem(int x, int y, const char* text,
                            UIOnClickCallback onClick, void* userData);

/** Create a panel/container widget */
UIWidget* UI_CreatePanel(int x, int y, int width, int height);

/** Create a slider widget */
UIWidget* UI_CreateSlider(int x, int y, int width, 
                          float initialValue, float minVal, float maxVal);

/** Create a checkbox widget */
UIWidget* UI_CreateCheckbox(int x, int y, const char* text, int initialState);

/** Create a progress bar widget */
UIWidget* UI_CreateProgressBar(int x, int y, int width, float initialValue);

/* ============================================================
 * WIDGET MANAGEMENT
 * ============================================================ */

/** Add a child widget to a parent (panel/menu) */
void UI_AddChild(UIWidget* parent, UIWidget* child);

/** Remove a widget from its parent */
void UI_RemoveChild(UIWidget* parent, UIWidget* child);

/** Delete a widget and all its children recursively */
void UI_DeleteWidget(UIWidget* widget);

/** Find a widget by ID */
UIWidget* UI_FindWidgetByID(int id);

/* ============================================================
 * UPDATE & RENDER
 * ============================================================ */

/** Update all widgets (process input, call onUpdate callbacks) */
int UI_UpdateAll(UIWidget* root);  /* Returns focused widget or NULL */

/** Render all widgets starting from root */
void UI_RenderAll(UIWidget* root);

/* ============================================================
 * INPUT HANDLING
 * ============================================================ */

/** Handle keyboard input for the currently focused widget */
int UI_HandleInput(UIWidget* root, int keyCode);

/** Set focus to a specific widget */
void UI_SetFocus(UIWidget* widget);

/** Clear all focus */
void UI_ClearFocus(void);

/* ============================================================
 * UTILITY FUNCTIONS
 * ============================================================ */

/** Center a widget horizontally on screen */
void UI_CenterX(UIWidget* widget, int screenWidth);

/** Center a widget vertically on screen */
void UI_CenterY(UIWidget* widget, int screenHeight);

/** Center a widget both axes */
void UI_Center(UIWidget* widget, int screenWidth, int screenHeight);

/** Set widget colors */
void UI_SetColors(UIWidget* widget, RenderColor text, RenderColor bg, 
                  RenderColor border);

/** Enable/disable a widget and all children */
void UI_Enable(UIWidget* widget);
void UI_Disable(UIWidget* widget);

#ifdef __cplusplus
}
#endif

#endif /* UI_H */
