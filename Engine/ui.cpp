/**
 * @file ui.cpp
 * @brief TUI Widget Framework Implementation
 * 
 * Implements buttons, menus, text inputs, panels, sliders, checkboxes,
 * and progress bars for terminal-based games.
 */

#define _CRT_SECURE_NO_WARNINGS
#include <math.h>

#include "ui.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/* Inline fmax/fmin for C compatibility */
static inline double inline_fmax(double a, double b) { return (a > b) ? a : b; }
static inline double inline_fmin(double a, double b) { return (a < b) ? a : b; }

/* ============================================================
 * INTERNAL STATE
 * ============================================================ */

static int g_nextWidgetId = 1;
static UIWidget* g_focusedWidget = NULL;

/** Generate a unique widget ID */
static int Widget_GenerateID(void) {
    return g_nextWidgetId++;
}

/* ============================================================
 * WIDGET CREATION
 * ============================================================ */

UIWidget* UI_CreateButton(int x, int y, const char* text, 
                          UIOnClickCallback onClick, void* userData) {
    UIWidget* widget = (UIWidget*)calloc(1, sizeof(UIWidget));
    if (!widget) return NULL;
    
    widget->id = Widget_GenerateID();
    widget->type = UI_WIDGET_BUTTON;
    widget->x = x;
    widget->y = y;
    widget->width = (int)strlen(text) + 4;
    widget->height = 3;
    widget->textColor = COLOR_WHITE;
    widget->bgColor = COLOR_BLACK;
    widget->borderColor = COLOR_CYAN;
    widget->bold = 1;
    widget->enabled = 1;
    widget->focused = 0;
    widget->visible = 1;
    widget->onClick = onClick;
    widget->userData = userData;
    
    if (text) {
        strncpy(widget->text, text, sizeof(widget->text) - 1);
    }
    
    return widget;
}

UIWidget* UI_CreateLabel(int x, int y, const char* text) {
    UIWidget* widget = (UIWidget*)calloc(1, sizeof(UIWidget));
    if (!widget) return NULL;
    
    widget->id = Widget_GenerateID();
    widget->type = UI_WIDGET_LABEL;
    widget->x = x;
    widget->y = y;
    widget->width = text ? (int)strlen(text) : 0;
    widget->height = 1;
    widget->textColor = COLOR_WHITE;
    widget->bgColor = COLOR_BLACK;
    widget->enabled = 1;
    widget->visible = 1;
    
    if (text) {
        strncpy(widget->text, text, sizeof(widget->text) - 1);
    }
    
    return widget;
}

UIWidget* UI_CreateTextInput(int x, int y, int maxWidth, 
                             char* buffer, int bufferSize) {
    UIWidget* widget = (UIWidget*)calloc(1, sizeof(UIWidget));
    if (!widget) return NULL;
    
    widget->id = Widget_GenerateID();
    widget->type = UI_WIDGET_TEXT_INPUT;
    widget->x = x;
    widget->y = y;
    widget->width = maxWidth + 2;
    widget->height = 1;
    widget->textColor = COLOR_CYAN;
    widget->bgColor = COLOR_BLACK;
    widget->borderColor = COLOR_WHITE;
    widget->enabled = 1;
    widget->focused = 0;
    widget->visible = 1;
    
    widget->inputBuffer = buffer;
    widget->inputMaxLength = bufferSize - 1;
    widget->cursorPos = 0;
    
    if (buffer) {
        buffer[0] = '\0';
    }
    
    return widget;
}

UIWidget* UI_CreateMenuItem(int x, int y, const char* text,
                            UIOnClickCallback onClick, void* userData) {
    UIWidget* widget = (UIWidget*)calloc(1, sizeof(UIWidget));
    if (!widget) return NULL;
    
    widget->id = Widget_GenerateID();
    widget->type = UI_WIDGET_MENU_ITEM;
    widget->x = x;
    widget->y = y;
    widget->width = text ? (int)strlen(text) + 2 : 0;
    widget->height = 1;
    widget->textColor = COLOR_WHITE;
    widget->bgColor = COLOR_BLACK;
    widget->enabled = 1;
    widget->focused = 0;
    widget->visible = 1;
    widget->selected = 0;
    widget->onClick = onClick;
    widget->userData = userData;
    
    if (text) {
        strncpy(widget->text, text, sizeof(widget->text) - 1);
    }
    
    return widget;
}

UIWidget* UI_CreatePanel(int x, int y, int width, int height) {
    UIWidget* widget = (UIWidget*)calloc(1, sizeof(UIWidget));
    if (!widget) return NULL;
    
    widget->id = Widget_GenerateID();
    widget->type = UI_WIDGET_PANEL;
    widget->x = x;
    widget->y = y;
    widget->width = width;
    widget->height = height;
    widget->bgColor = COLOR_BLACK;
    widget->borderColor = COLOR_WHITE;
    widget->enabled = 1;
    widget->visible = 1;
    
    return widget;
}

UIWidget* UI_CreateSlider(int x, int y, int width, 
                          float initialValue, float minVal, float maxVal) {
    UIWidget* widget = (UIWidget*)calloc(1, sizeof(UIWidget));
    if (!widget) return NULL;
    
    widget->id = Widget_GenerateID();
    widget->type = UI_WIDGET_SLIDER;
    widget->x = x;
    widget->y = y;
    widget->width = width;
    widget->height = 1;
    widget->textColor = COLOR_YELLOW;
    widget->bgColor = COLOR_BLACK;
    widget->borderColor = COLOR_CYAN;
    widget->enabled = 1;
    widget->focused = 0;
    widget->visible = 1;
    
    widget->minValue = minVal;
    widget->maxValue = maxVal;
    widget->value = (float)inline_fmax((double)minVal, 
                    inline_fmin((double)maxVal, (double)initialValue));
    
    return widget;
}

UIWidget* UI_CreateCheckbox(int x, int y, const char* text, int initialState) {
    UIWidget* widget = (UIWidget*)calloc(1, sizeof(UIWidget));
    if (!widget) return NULL;
    
    widget->id = Widget_GenerateID();
    widget->type = UI_WIDGET_CHECKBOX;
    widget->x = x;
    widget->y = y;
    widget->width = text ? (int)strlen(text) + 4 : 4;
    widget->height = 1;
    widget->textColor = COLOR_WHITE;
    widget->bgColor = COLOR_BLACK;
    widget->enabled = 1;
    widget->focused = 0;
    widget->visible = 1;
    widget->selected = initialState ? 1 : 0;
    
    if (text) {
        strncpy(widget->text, text, sizeof(widget->text) - 1);
    }
    
    return widget;
}

UIWidget* UI_CreateProgressBar(int x, int y, int width, float initialValue) {
    UIWidget* widget = (UIWidget*)calloc(1, sizeof(UIWidget));
    if (!widget) return NULL;
    
    widget->id = Widget_GenerateID();
    widget->type = UI_WIDGET_PROGRESS_BAR;
    widget->x = x;
    widget->y = y;
    widget->width = width;
    widget->height = 1;
    widget->textColor = COLOR_GREEN;
    widget->bgColor = COLOR_BLACK;
    widget->enabled = 1;
    widget->visible = 1;
    
    widget->value = (float)inline_fmax(0.0, inline_fmin(1.0, (double)initialValue));
    
    return widget;
}

/* ============================================================
 * WIDGET MANAGEMENT
 * ============================================================ */

void UI_AddChild(UIWidget* parent, UIWidget* child) {
    if (!parent || !child) return;
    
    child->parent = parent;
    child->nextSibling = NULL;
    
    if (parent->firstChild == NULL) {
        parent->firstChild = child;
    } else {
        UIWidget* current = parent->firstChild;
        while (current->nextSibling != NULL) {
            current = current->nextSibling;
        }
        current->nextSibling = child;
    }
}

void UI_RemoveChild(UIWidget* parent, UIWidget* child) {
    if (!parent || !child) return;
    
    UIWidget* current = parent->firstChild;
    UIWidget* previous = NULL;
    
    while (current != NULL) {
        if (current == child) {
            if (previous) {
                previous->nextSibling = current->nextSibling;
            } else {
                parent->firstChild = current->nextSibling;
            }
            child->parent = NULL;
            child->nextSibling = NULL;
            return;
        }
        previous = current;
        current = current->nextSibling;
    }
}

void UI_DeleteWidget(UIWidget* widget) {
    if (!widget) return;
    
    UIWidget* child = widget->firstChild;
    while (child) {
        UIWidget* next = child->nextSibling;
        UI_DeleteWidget(child);
        child = next;
    }
    
    free(widget);
}

UIWidget* UI_FindWidgetByID(int id) {
    (void)id;
    return NULL;
}

/* ============================================================
 * UPDATE & RENDER
 * ============================================================ */

static UIWidget* FindFirstFocusable(UIWidget* widget) {
    if (!widget || !widget->enabled || !widget->visible) return NULL;
    
    if (widget->type == UI_WIDGET_PANEL) {
        return FindFirstFocusable(widget->firstChild);
    }
    
    if (widget->type == UI_WIDGET_BUTTON || 
        widget->type == UI_WIDGET_MENU_ITEM ||
        widget->type == UI_WIDGET_TEXT_INPUT ||
        widget->type == UI_WIDGET_SLIDER ||
        widget->type == UI_WIDGET_CHECKBOX) {
        return widget;
    }
    
    return NULL;
}

static UIWidget* GetNextFocusable(UIWidget* widget) {
    if (!widget) return NULL;
    
    if (widget->type == UI_WIDGET_PANEL) {
        return FindFirstFocusable(widget->firstChild);
    }
    
    UIWidget* next = widget->nextSibling;
    while (next) {
        UIWidget* focusable = FindFirstFocusable(next);
        if (focusable) return focusable;
        next = next->nextSibling;
    }
    
    if (widget->parent) {
        return GetNextFocusable(widget->parent);
    }
    
    return NULL;
}

int UI_UpdateAll(UIWidget* root) {
    if (!root) return 0;
    
    EngineEvent event;
    while (Engine_PollEvent(&event)) {
        switch (event.type) {
            case ENGINE_EVENT_KEY_PRESS:
                UI_HandleInput(root, event.keyCode);
                break;
            default:
                break;
        }
    }
    
    return (g_focusedWidget != NULL);
}

void UI_RenderAll(UIWidget* root) {
    if (!root) return;
    
    /* Render children for panels first */
    if (root->type == UI_WIDGET_PANEL && root->firstChild) {
        UIWidget* child = root->firstChild;
        while (child) {
            if (child->visible) {
                UI_RenderAll(child);
            }
            child = child->nextSibling;
        }
    }
    
    /* Render this widget */
    switch (root->type) {
        case UI_WIDGET_PANEL:
            Render_DrawPanelWithBorder(root->x, root->y, 
                                       root->width, root->height,
                                       root->bgColor, root->borderColor);
            break;
            
        case UI_WIDGET_BUTTON: {
            Render_DrawBox(root->x, root->y, root->width, root->height, 
                          root->focused ? COLOR_YELLOW : root->borderColor);
            
            int textX = root->x + (root->width - (int)strlen(root->text)) / 2;
            int textY = root->y + 1;
            
            if (textX >= 0 && textX < RENDER_MAX_WIDTH && 
                textY >= 0 && textY < RENDER_MAX_HEIGHT) {
                Render_SetPosition(textX, textY);
                if (root->focused) {
                    Render_PrintBoldColored(root->text, COLOR_YELLOW);
                } else {
                    Render_PrintColored(root->text, root->textColor);
                }
            }
            break;
        }
        
        case UI_WIDGET_LABEL: {
            if (root->x >= 0 && root->x < RENDER_MAX_WIDTH) {
                Render_SetPosition(root->x, root->y);
                if (root->bold) {
                    Render_PrintBoldColored(root->text, root->textColor);
                } else {
                    Render_PrintColored(root->text, root->textColor);
                }
            }
            break;
        }
        
        case UI_WIDGET_MENU_ITEM: {
            if (root->x >= 0 && root->x < RENDER_MAX_WIDTH) {
                Render_SetPosition(root->x, root->y);
                if (root->focused) {
                    Render_PrintColored(">", COLOR_YELLOW);
                    Render_PrintfColored(COLOR_CYAN, " %s", root->text);
                } else {
                    Render_Print("  ");
                    Render_PrintColored(root->text, root->textColor);
                }
            }
            break;
        }
        
        case UI_WIDGET_TEXT_INPUT: {
            Render_DrawBox(root->x, root->y, root->width, root->height, 
                          root->focused ? COLOR_YELLOW : root->borderColor);
            
            int textX = root->x + 1;
            if (textX >= 0 && textX < RENDER_MAX_WIDTH) {
                Render_SetPosition(textX, root->y);
                Render_PrintColored(root->inputBuffer ? root->inputBuffer : "", 
                                   root->focused ? COLOR_YELLOW : root->textColor);
                
                if (root->focused && root->inputBuffer) {
                    int cursorScreenX = textX + (int)strlen(root->inputBuffer);
                    Render_SetPosition(cursorScreenX, root->y);
                    printf("\033[7m \033[0m");
                    fflush(stdout);
                }
            }
            break;
        }
        
        case UI_WIDGET_SLIDER: {
            int filledWidth = (int)(root->value * root->width);
            
            Render_SetPosition(root->x, root->y);
            printf("[");
            for (int i = 0; i < root->width - 2; i++) {
                if (i < filledWidth) {
                    printf("\033[%dm=\033[0m", root->textColor);
                } else {
                    printf("-");
                }
            }
            printf("]");
            
            Render_PrintfColored(COLOR_WHITE, " %.1f", 
                                root->minValue + root->value * (root->maxValue - root->minValue));
            break;
        }
        
        case UI_WIDGET_CHECKBOX: {
            if (root->x >= 0 && root->x < RENDER_MAX_WIDTH) {
                Render_SetPosition(root->x, root->y);
                
                if (root->focused) {
                    Render_PrintColored("[", COLOR_YELLOW);
                } else {
                    printf("[");
                }
                
                printf("%s", root->selected ? "X" : " ");
                
                if (root->focused) {
                    Render_PrintColored("]", COLOR_YELLOW);
                } else {
                    printf("]");
                }
                
                Render_PrintfColored(COLOR_WHITE, " %s", root->text);
            }
            break;
        }
        
        case UI_WIDGET_PROGRESS_BAR: {
            int filledWidth = (int)(root->value * (root->width - 2));
            
            Render_SetPosition(root->x, root->y);
            printf("[");
            for (int i = 0; i < root->width - 2; i++) {
                if (i < filledWidth) {
                    printf("\033[%dm#\033[0m", root->textColor);
                } else {
                    printf("-");
                }
            }
            printf("] %d%%", (int)(root->value * 100));
            break;
        }
        
        default:
            break;
    }
}

/* ============================================================
 * INPUT HANDLING
 * ============================================================ */

int UI_HandleInput(UIWidget* root, int keyCode) {
    if (!g_focusedWidget || !g_focusedWidget->enabled) {
        g_focusedWidget = FindFirstFocusable(root);
        
        if (g_focusedWidget) {
            g_focusedWidget->focused = 1;
        }
        return (g_focusedWidget != NULL);
    }
    
    UIWidget* focused = g_focusedWidget;
    
    switch (keyCode) {
        case KEY_ESCAPE:
            if (focused->type == UI_WIDGET_TEXT_INPUT && focused->inputBuffer) {
                focused->inputBuffer[0] = '\0';
                focused->cursorPos = 0;
            }
            focused->focused = 0;
            g_focusedWidget = NULL;
            return 0;
            
        case KEY_TAB:
        case KEY_DOWN: {
            focused->focused = 0;
            g_focusedWidget = GetNextFocusable(focused);
            if (g_focusedWidget) {
                g_focusedWidget->focused = 1;
            }
            return (g_focusedWidget != NULL);
        }
        
        case KEY_UP: {
            focused->focused = 0;
            g_focusedWidget = FindFirstFocusable(root);
            if (g_focusedWidget) {
                g_focusedWidget->focused = 1;
            }
            return (g_focusedWidget != NULL);
        }
        
        case KEY_ENTER:
        case KEY_RIGHT: {
            if (focused->onClick) {
                focused->onClick(focused->userData);
            }
            
            if (focused->type == UI_WIDGET_MENU_ITEM || 
                focused->type == UI_WIDGET_BUTTON) {
                focused->focused = 0;
                g_focusedWidget = GetNextFocusable(focused);
                if (g_focusedWidget) {
                    g_focusedWidget->focused = 1;
                }
            }
            return (g_focusedWidget != NULL);
        }
        
        case KEY_SPACE: {
            if (focused->type == UI_WIDGET_CHECKBOX) {
                focused->selected = !focused->selected;
            }
            return 1;
        }
        
        default: {
            if (focused->type == UI_WIDGET_TEXT_INPUT && 
                focused->inputBuffer && focused->enabled) {
                
                unsigned char ch = (unsigned char)keyCode;
                
                if (keyCode == KEY_BACKSPACE) {
                    if (focused->cursorPos > 0) {
                        int len = (int)strlen(focused->inputBuffer);
                        if (focused->cursorPos < len) {
                            memmove(focused->inputBuffer + focused->cursorPos - 1,
                                   focused->inputBuffer + focused->cursorPos,
                                   len - focused->cursorPos + 1);
                        } else {
                            focused->inputBuffer[focused->cursorPos - 1] = '\0';
                        }
                        focused->cursorPos--;
                    }
                    return 1;
                }
                
                if (keyCode == KEY_ENTER) {
                    focused->focused = 0;
                    g_focusedWidget = GetNextFocusable(focused);
                    if (g_focusedWidget) {
                        g_focusedWidget->focused = 1;
                    }
                    return (g_focusedWidget != NULL);
                }
                
                if (ch >= 32 && ch < 127 && focused->cursorPos < focused->inputMaxLength) {
                    int len = (int)strlen(focused->inputBuffer);
                    
                    if (focused->cursorPos < len) {
                        memmove(focused->inputBuffer + focused->cursorPos + 1,
                               focused->inputBuffer + focused->cursorPos,
                               len - focused->cursorPos);
                    }
                    
                    focused->inputBuffer[focused->cursorPos] = (char)ch;
                    focused->cursorPos++;
                    focused->inputBuffer[focused->cursorPos] = '\0';
                }
            }
            
            /* Handle slider/progress bar with left/right arrows */
            if ((keyCode == 257 || keyCode == 261) && 
                (focused->type == UI_WIDGET_SLIDER || 
                 focused->type == UI_WIDGET_PROGRESS_BAR)) {
                if (keyCode == 257) { /* LEFT */
                    focused->value = (float)inline_fmax((double)focused->minValue, 
                                                        (double)focused->value - 0.05);
                } else { /* RIGHT */
                    focused->value = (float)inline_fmin((double)focused->maxValue, 
                                                        (double)focused->value + 0.05);
                }
            }
            break;
        }
    }
    
    return 1;
}

void UI_SetFocus(UIWidget* widget) {
    if (g_focusedWidget && g_focusedWidget != widget) {
        g_focusedWidget->focused = 0;
    }
    
    if (widget && widget->enabled && widget->visible) {
        widget->focused = 1;
        g_focusedWidget = widget;
    }
}

void UI_ClearFocus(void) {
    if (g_focusedWidget) {
        g_focusedWidget->focused = 0;
    }
    g_focusedWidget = NULL;
}

/* ============================================================
 * UTILITY FUNCTIONS
 * ============================================================ */

void UI_CenterX(UIWidget* widget, int screenWidth) {
    if (!widget) return;
    widget->x = (screenWidth - widget->width) / 2;
}

void UI_CenterY(UIWidget* widget, int screenHeight) {
    if (!widget) return;
    widget->y = (screenHeight - widget->height) / 2;
}

void UI_Center(UIWidget* widget, int screenWidth, int screenHeight) {
    if (!widget) return;
    widget->x = (screenWidth - widget->width) / 2;
    widget->y = (screenHeight - widget->height) / 2;
}

void UI_SetColors(UIWidget* widget, RenderColor text, RenderColor bg, 
                  RenderColor border) {
    if (!widget) return;
    
    widget->textColor = text;
    widget->bgColor = bg;
    widget->borderColor = border;
}

void UI_Enable(UIWidget* widget) {
    if (!widget) return;
    
    widget->enabled = 1;
    
    UIWidget* child = widget->firstChild;
    while (child) {
        UI_Enable(child);
        child = child->nextSibling;
    }
}

void UI_Disable(UIWidget* widget) {
    if (!widget) return;
    
    widget->enabled = 0;
    
    UIWidget* child = widget->firstChild;
    while (child) {
        UI_Disable(child);
        child = child->nextSibling;
    }
}
