/**
 * @file GameMain.cpp
 * @brief Potion Master - Demo Game Implementation
 * 
 * A potion shop management game that demonstrates the Universal TUI Engine.
 * Players buy potions, manage inventory, and try to amass wealth.
 */

#define _CRT_SECURE_NO_WARNINGS

#include "GameMain.h"
#include "../Engine/utils/mathutils.h"
#include "../Engine/utils/stringutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

/* Sound effect definitions (single sounds, not arrays) */
static const AudioSoundEffect SOUND_CLICK_EFFECT = {800, 50};
static const AudioSoundEffect SOUND_SELECT_EFFECT = {600, 100};
static const AudioSoundEffect SOUND_ERROR_EFFECT = {200, 300};
static const AudioSoundEffect SOUND_SUCCESS_EFFECT = {1000, 150};

/* ============================================================
 * GLOBAL STATE
 * ============================================================ */

static GameStateData g_gameState = {0};
static int g_running = 1;

/* Potion name pools */
static const char* potionPrefixes[] = {
    "Minor", "Greater", "Elder", "Arcane", "Mystic", 
    "Dragon", "Phoenix", "Shadow", "Crystal", "Iron"
};

static const char* potionNames[] = {
    "Healing", "Mana Restoration", "Strength", "Haste", 
    "Invisibility", "Fire Resistance", "Ice Shield", "Poison",
    "Paralysis", "Stone Skin", "Fly", "See Invisible",
    "Teleport", "Regeneration", "Invulnerability"
};

static const char* potionDescriptions[] = {
    "Restores health instantly",
    "Restores magical energy",
    "Increases physical power",
    "Speeds up movement",
    "Makes the user invisible",
    "Reduces fire damage taken",
    "Creates an icy barrier",
    "Deals poison over time",
    "Paralyzes enemies briefly",
    "Hardens skin like stone",
    "Allows short flight",
    "Reveals hidden things",
    "Teleports to target location",
    "Gradually heals wounds",
    "Provides temporary immunity"
};

/* ============================================================
 * GAME INITIALIZATION & SHUTDOWN
 * ============================================================ */

void Game_Init(void) {
    /* Initialize random seed */
    Math_RandomSeed((unsigned int)time(NULL));
    
    /* Initialize audio */
    Audio_Init();
    
    /* Initialize entity manager */
    EntityMgr_Init();
    
    /* Reset game state */
    memset(&g_gameState, 0, sizeof(GameStateData));
    
    g_gameState.goldCoins = 100;
    g_gameState.health = 50;
    g_gameState.maxHealth = 50;
    g_gameState.inventoryCount = 0;
    g_gameState.shopPotionCount = 0;
    g_gameState.currentScreen = 0; /* Main menu */
    
    /* Generate initial shop potions */
    for (int i = 0; i < 8 && i < MAX_POTIONS; i++) {
        g_gameState.shopPotions[i] = Game_GenerateRandomPotion();
        g_gameState.shopPotionCount++;
    }
    
    Engine_Log("Game initialized!");
}

void Game_Shutdown(void) {
    /* Shutdown entity manager */
    EntityMgr_Shutdown();
    
    /* Shutdown audio */
    Audio_Shutdown();
    
    Engine_Log("Game shutdown complete.");
}

/* ============================================================
 * POTION FUNCTIONS
 * ============================================================ */

Potion Game_GenerateRandomPotion(void) {
    Potion potion;
    memset(&potion, 0, sizeof(Potion));
    
    /* Random name */
    const char* prefix = potionPrefixes[Math_RandomInt(0, 
                    (int)(sizeof(potionPrefixes)/sizeof(potionPrefixes[0])) - 1)];
    int nameIdx = Math_RandomInt(0, 14);
    
    snprintf(potion.name, sizeof(potion.name), "%s %s", prefix, potionNames[nameIdx]);
    
    /* Random description */
    strncpy(potion.description, potionDescriptions[nameIdx], 
            sizeof(potion.description) - 1);
    
    /* Random type based on name index */
    potion.type = (PotionType)(nameIdx % POTION_TYPE_COUNT);
    
    /* Random price and potency */
    potion.price = Math_RandomInt(5, 50);
    potion.potency = Math_RandomInt(10, 100);
    
    /* Assign glyph based on type */
    switch (potion.type) {
        case POTION_HEALTH:     potion.glyph = '+'; break;
        case POTION_MANA:       potion.glyph = 'M'; break;
        case POTION_STRENGTH:   potion.glyph = '!'; break;
        case POTION_SPEED:      potion.glyph = '>>'; break;
        case POTION_INVISIBILITY: potion.glyph = '~'; break;
        default:                potion.glyph = '?'; break;
    }
    
    return potion;
}

int Game_FindPotionInInventory(const char* name) {
    for (int i = 0; i < g_gameState.inventoryCount; i++) {
        if (strcmp(g_gameState.inventory[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int Game_PurchasePotion(int shopIndex) {
    if (shopIndex < 0 || shopIndex >= g_gameState.shopPotionCount) return 0;
    
    Potion* potion = &g_gameState.shopPotions[shopIndex];
    
    /* Check if player can afford */
    if (g_gameState.goldCoins < potion->price) {
        Game_DisplayMessage("Not enough gold! Need %d, have %d.",
                          potion->price, g_gameState.goldCoins);
        Audio_PlaySound(&SOUND_ERROR_EFFECT);
        return 0;
    }
    
    /* Check if inventory is full */
    if (g_gameState.inventoryCount >= MAX_POTIONS) {
        Game_DisplayMessage("Inventory is full!");
        Audio_PlaySound(&SOUND_ERROR_EFFECT);
        return 0;
    }
    
    /* Purchase the potion */
    g_gameState.goldCoins -= potion->price;
    g_gameState.inventory[g_gameState.inventoryCount++] = *potion;
    
    Game_DisplayMessage("Purchased: %s", potion->name);
    Audio_PlaySound(&SOUND_SUCCESS_EFFECT);
    
    return 1;
}

int Game_UsePotion(int inventoryIndex) {
    if (inventoryIndex < 0 || inventoryIndex >= g_gameState.inventoryCount) return 0;
    
    Potion* potion = &g_gameState.inventory[inventoryIndex];
    
    /* Apply effect based on type */
    switch (potion->type) {
        case POTION_HEALTH:
            g_gameState.health += potion->potency;
            if (g_gameState.health > g_gameState.maxHealth) {
                g_gameState.health = g_gameState.maxHealth;
            }
            Game_DisplayMessage("Used %s! +%d health", potion->name, potion->potency);
            break;
            
        case POTION_MANA:
            Game_DisplayMessage("Used %s! Mana restored!", potion->name);
            break;
            
        case POTION_STRENGTH:
            g_gameState.maxHealth += potion->potency / 10;
            g_gameState.health += potion->potency / 10;
            Game_DisplayMessage("Used %s! Max health increased!", potion->name);
            break;
            
        case POTION_SPEED:
            Game_DisplayMessage("Used %s! Speed boosted!", potion->name);
            break;
            
        case POTION_INVISIBILITY:
            Game_DisplayMessage("Used %s! You are invisible!", potion->name);
            break;
            
        default:
            Game_DisplayMessage("Used %s!", potion->name);
            break;
    }
    
    /* Remove from inventory */
    for (int i = inventoryIndex; i < g_gameState.inventoryCount - 1; i++) {
        g_gameState.inventory[i] = g_gameState.inventory[i + 1];
    }
    g_gameState.inventoryCount--;
    
    Audio_PlaySound(&SOUND_SELECT_EFFECT);
    return 1;
}

/* ============================================================
 * MESSAGE DISPLAY
 * ============================================================ */

void Game_DisplayMessage(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(g_gameState.message, sizeof(g_gameState.message), format, args);
    va_end(args);
    
    g_gameState.messageTimer = Engine_GetTickCount() + 3000; /* Show for 3 seconds */
}

int Game_RandomInt(int minVal, int maxVal) {
    return Math_RandomInt(minVal, maxVal);
}

/* ============================================================
 * MAIN UPDATE & RENDER
 * ============================================================ */

void Game_Update(float deltaTime) {
    (void)deltaTime;
    
    /* Process engine events */
    EngineEvent event;
    while (Engine_PollEvent(&event)) {
        switch (event.type) {
            case ENGINE_EVENT_QUIT:
                g_running = 0;
                break;
                
            case ENGINE_EVENT_KEY_PRESS:
                switch (event.keyCode) {
                    case KEY_ESCAPE:
                        /* Navigate back or quit */
                        if (g_gameState.currentScreen == 0) {
                            g_running = 0;
                        } else {
                            g_gameState.currentScreen = 0;
                            Audio_PlaySound(&SOUND_CLICK_EFFECT);
                        }
                        break;
                        
                    case KEY_ENTER:
                        switch (g_gameState.currentScreen) {
                            case 0: /* Main menu */
                                if (g_gameState.selectedMenuItem == 1) {
                                    g_gameState.currentScreen = 1; /* Shop */
                                } else if (g_gameState.selectedMenuItem == 2) {
                                    g_gameState.currentScreen = 2; /* Inventory */
                                } else if (g_gameState.selectedMenuItem == 3) {
                                    g_running = 0;
                                }
                                Audio_PlaySound(&SOUND_CLICK_EFFECT);
                                break;
                                
                            case 1: /* Shop - buy selected potion */
                                Game_PurchasePotion(g_gameState.selectedMenuItem - 5);
                                break;
                                
                            case 2: /* Inventory - use selected potion */
                                if (g_gameState.selectedMenuItem >= 5) {
                                    Game_UsePotion(g_gameState.selectedMenuItem - 5);
                                }
                                break;
                        }
                        break;
                        
                    case KEY_UP:
                        if (g_gameState.selectedMenuItem > 0) {
                            g_gameState.selectedMenuItem--;
                        }
                        Audio_PlaySound(&SOUND_CLICK_EFFECT);
                        break;
                        
                    case KEY_DOWN:
                        switch (g_gameState.currentScreen) {
                            case 0: /* Main menu */
                                if (g_gameState.selectedMenuItem < 3) {
                                    g_gameState.selectedMenuItem++;
                                }
                                break;
                                
                            case 1: /* Shop */
                                if (g_gameState.selectedMenuItem < 4 + g_gameState.shopPotionCount) {
                                    g_gameState.selectedMenuItem++;
                                }
                                break;
                                
                            case 2: /* Inventory */
                                if (g_gameState.selectedMenuItem < 4 + g_gameState.inventoryCount) {
                                    g_gameState.selectedMenuItem++;
                                }
                                break;
                        }
                        Audio_PlaySound(&SOUND_CLICK_EFFECT);
                        break;
                }
                break;
                
            default:
                break;
        }
    }
    
    /* Clear message after timer expires */
    if (Engine_GetTickCount() > g_gameState.messageTimer && 
        strlen(g_gameState.message) > 0) {
        g_gameState.message[0] = '\0';
    }
}

void Game_Render(void) {
    /* Clear screen with dark background */
    Render_Clear(COLOR_BLACK);
    
    switch (g_gameState.currentScreen) {
        case 0:
            Screen_MainMenu();
            break;
        case 1:
            Screen_Shop();
            break;
        case 2:
            Screen_Inventory();
            break;
        default:
            Screen_MainMenu();
            break;
    }
    
    /* Display message if active */
    if (strlen(g_gameState.message) > 0) {
        Render_DrawPanelWithBorder(15, GAME_HEIGHT - 4, 50, 3, 
                                   COLOR_BLACK, COLOR_YELLOW);
        Render_SetPosition(17, GAME_HEIGHT - 3);
        Render_PrintColored(g_gameState.message, COLOR_WHITE);
    }
    
    /* Present the frame */
    Render_Present();
}

void Game_Cleanup(void) {
    /* Nothing special to clean up here */
}

/* ============================================================
 * SCREEN RENDERERS
 * ============================================================ */

void Screen_MainMenu(void) {
    /* Title banner */
    Render_DrawPanelWithBorder(5, 2, 70, 6, COLOR_BLACK, COLOR_CYAN);
    
    Render_SetPosition(15, 3);
    Render_PrintfBoldColored(COLOR_CYAN, "=== POTION MASTER ===");
    
    Render_SetPosition(18, 4);
    Render_PrintColored("A Terminal Potion Shop Game", COLOR_WHITE);
    
    /* Player stats */
    Render_DrawPanelWithBorder(5, 9, 30, 4, COLOR_BLACK, COLOR_GREEN);
    Render_SetPosition(7, 10);
    Render_PrintfColored(COLOR_YELLOW, "Gold: %d", g_gameState.goldCoins);
    
    Render_SetPosition(7, 11);
    Render_PrintfColored(COLOR_RED, "Health: %d/%d", 
                       g_gameState.health, g_gameState.maxHealth);
    
    /* Menu items */
    const char* menuItems[] = {"Shop", "Inventory", "Quit"};
    
    for (int i = 0; i < 3; i++) {
        int y = 15 + i;
        
        if (g_gameState.selectedMenuItem == i) {
            Render_PrintfColored(COLOR_YELLOW, "> %s <", menuItems[i]);
        } else {
            Render_Print("  ");
            Render_PrintColored(menuItems[i], COLOR_WHITE);
        }
    }
    
    /* Instructions */
    Render_SetPosition(10, GAME_HEIGHT - 2);
    Render_PrintfColored(COLOR_MAGENTA, "Use UP/DOWN to navigate, ENTER to select, ESC to go back");
}

void Screen_Shop(void) {
    /* Shop header */
    Render_DrawPanelWithBorder(5, 2, 70, 1, COLOR_BLACK, COLOR_YELLOW);
    Render_SetPosition(8, 2);
    Render_PrintfBoldColored(COLOR_YELLOW, "POTION SHOP");
    
    /* Player gold display */
    Render_SetPosition(55, 2);
    Render_PrintfColored(COLOR_GREEN, "Gold: %d", g_gameState.goldCoins);
    
    /* Shop potions list */
    Render_DrawPanelWithBorder(5, 4, 70, 3 + g_gameState.shopPotionCount, 
                               COLOR_BLACK, COLOR_CYAN);
    
    Render_SetPosition(7, 5);
    Render_PrintfBoldColored(COLOR_CYAN, "Name                    Price   Type");
    
    for (int i = 0; i < g_gameState.shopPotionCount && i < MAX_POTIONS; i++) {
        int y = 6 + i;
        Potion* potion = &g_gameState.shopPotions[i];
        
        if (g_gameState.selectedMenuItem == 5 + i) {
            Render_SetPosition(7, y);
            printf("\033[1m> \033[0m");
            Render_PrintfColored(COLOR_YELLOW, "%-21s %4d     ", 
                               potion->name, potion->price);
            
            switch (potion->type) {
                case POTION_HEALTH:     printf("\033[31mHealth\033[0m"); break;
                case POTION_MANA:       printf("\033[34mMana\033[0m"); break;
                case POTION_STRENGTH:   printf("\033[33mStrength\033[0m"); break;
                case POTION_SPEED:      printf("\033[32mSpeed\033[0m"); break;
                default:                printf("Unknown"); break;
            }
        } else {
            Render_SetPosition(9, y);
            Render_PrintfColored(COLOR_WHITE, "%-21s %4d     ", 
                               potion->name, potion->price);
            
            switch (potion->type) {
                case POTION_HEALTH:     printf("\033[31mHealth\033[0m"); break;
                case POTION_MANA:       printf("\033[34mMana\033[0m"); break;
                case POTION_STRENGTH:   printf("\033[33mStrength\033[0m"); break;
                case POTION_SPEED:      printf("\033[32mSpeed\033[0m"); break;
                default:                printf("Unknown"); break;
            }
        }
    }
    
    /* Instructions */
    Render_SetPosition(10, GAME_HEIGHT - 2);
    Render_PrintfColored(COLOR_MAGENTA, "UP/DOWN to browse, ENTER to buy, ESC to return");
}

void Screen_Inventory(void) {
    /* Inventory header */
    Render_DrawPanelWithBorder(5, 2, 70, 1, COLOR_BLACK, COLOR_GREEN);
    Render_SetPosition(8, 2);
    Render_PrintfBoldColored(COLOR_GREEN, "INVENTORY");
    
    /* Potion count */
    Render_SetPosition(55, 2);
    Render_PrintfColored(COLOR_WHITE, "%d/%d potions", 
                       g_gameState.inventoryCount, MAX_POTIONS);
    
    /* Inventory list */
    if (g_gameState.inventoryCount == 0) {
        Render_SetPosition(25, 6);
        Render_PrintColored("No potions in inventory!", COLOR_MAGENTA);
    } else {
        Render_DrawPanelWithBorder(5, 4, 70, 3 + g_gameState.inventoryCount, 
                                   COLOR_BLACK, COLOR_GREEN);
        
        Render_SetPosition(7, 5);
        Render_PrintfBoldColored(COLOR_CYAN, "Name                    Type      Potency");
        
        for (int i = 0; i < g_gameState.inventoryCount && i < MAX_POTIONS; i++) {
            int y = 6 + i;
            Potion* potion = &g_gameState.inventory[i];
            
            if (g_gameState.selectedMenuItem == 5 + i) {
                Render_SetPosition(7, y);
                printf("\033[1m> \033[0m");
                Render_PrintfColored(COLOR_YELLOW, "%-21s ", potion->name);
                
                switch (potion->type) {
                    case POTION_HEALTH:     printf("\033[31mHealth    \033[0m%d", potion->potency); break;
                    case POTION_MANA:       printf("\033[34mMana      \033[0m%d", potion->potency); break;
                    case POTION_STRENGTH:   printf("\033[33mStrength  \033[0m%d", potion->potency); break;
                    case POTION_SPEED:      printf("\033[32mSpeed     \033[0m%d", potion->potency); break;
                    default:                printf("Unknown   %d", potion->potency); break;
                }
            } else {
                Render_SetPosition(9, y);
                Render_PrintfColored(COLOR_WHITE, "%-21s ", potion->name);
                
                switch (potion->type) {
                    case POTION_HEALTH:     printf("\033[31mHealth    \033[0m%d", potion->potency); break;
                    case POTION_MANA:       printf("\033[34mMana      \033[0m%d", potion->potency); break;
                    case POTION_STRENGTH:   printf("\033[33mStrength  \033[0m%d", potion->potency); break;
                    case POTION_SPEED:      printf("\033[32mSpeed     \033[0m%d", potion->potency); break;
                    default:                printf("Unknown   %d", potion->potency); break;
                }
            }
        }
    }
    
    /* Instructions */
    Render_SetPosition(10, GAME_HEIGHT - 2);
    Render_PrintfColored(COLOR_MAGENTA, "UP/DOWN to browse, ENTER to use, ESC to return");
}

void Screen_GameOver(int won) {
    (void)won;
    
    Render_DrawPanelWithBorder(15, 8, 50, 8, COLOR_BLACK, COLOR_RED);
    
    Render_SetPosition(22, 10);
    if (won) {
        Render_PrintfBoldColored(COLOR_GREEN, "YOU WIN!");
    } else {
        Render_PrintfBoldColored(COLOR_RED, "GAME OVER");
    }
    
    Render_SetPosition(20, 13);
    Render_PrintfColored(COLOR_WHITE, "Final Gold: %d", g_gameState.goldCoins);
    
    Render_SetPosition(20, 15);
    Render_PrintColored("Press ESC to quit", COLOR_MAGENTA);
}

/* ============================================================
 * MAIN ENTRY POINT
 * ============================================================ */

int main() {
    /* Initialize engine (80x24 terminal) */
    Engine_Init("Potion Master", GAME_WIDTH, GAME_HEIGHT);
    
    /* Initialize game */
    Game_Init();
    
    /* Push game state to engine */
    Engine_PushState("Game", Game_Update, Game_Render, Game_Cleanup);
    
    /* Run the game loop */
    Engine_Run();
    
    /* Cleanup and shutdown */
    Game_Shutdown();
    Engine_Shutdown();
    
    return 0;
}
