/**
 * @file GameMain.h
 * @brief Potion Master - Demo Game Header
 * 
 * A potion shop management game built with the Universal TUI Engine.
 * Demonstrates all engine features: UI, entities, components, audio.
 */

#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "../Engine/engine.h"
#include "../Engine/render.h"
#include "../Engine/ui.h"
#include "../Engine/entity.h"
#include "../Engine/component.h"
#include "../Engine/audio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * GAME CONSTANTS
 * ============================================================ */

#define MAX_POTIONS     50
#define GAME_WIDTH      80
#define GAME_HEIGHT     24

/* Potion types */
typedef enum {
    POTION_HEALTH = 0,
    POTION_MANA,
    POTION_STRENGTH,
    POTION_SPEED,
    POTION_INVISIBILITY,
    POTION_TYPE_COUNT
} PotionType;

/* ============================================================
 * POTION STRUCTURE
 * ============================================================ */

typedef struct {
    char name[64];
    char description[128];
    PotionType type;
    int price;        /* Price in gold coins */
    int potency;      /* Effect strength (1-100) */
    char glyph;       /* Display character */
} Potion;

/* ============================================================
 * GAME STATE STRUCTURE
 * ============================================================ */

typedef struct {
    /* Player state */
    int goldCoins;
    int health;
    int maxHealth;
    
    /* Inventory */
    Potion inventory[MAX_POTIONS];
    int inventoryCount;
    
    /* Shop potions (available for purchase) */
    Potion shopPotions[MAX_POTIONS];
    int shopPotionCount;
    
    /* UI state */
    int selectedMenuItem;
    int currentScreen;  /* 0 = main menu, 1 = shop, 2 = inventory, 3 = game over */
    
    /* Message display */
    char message[256];
    unsigned long messageTimer;
} GameStateData;

/* ============================================================
 * GAME FUNCTIONS
 * ============================================================ */

/** Initialize the game (called once at startup) */
void Game_Init(void);

/** Shutdown the game (called before exit) */
void Game_Shutdown(void);

/** Main update function - called every frame */
void Game_Update(float deltaTime);

/** Main render function - called every frame */
void Game_Render(void);

/** Cleanup function - called when leaving this state */
void Game_Cleanup(void);

/* ============================================================
 * POTION FUNCTIONS
 * ============================================================ */

/** Generate a random potion for the shop */
Potion Game_GenerateRandomPotion(void);

/** Find a potion by name in inventory */
int Game_FindPotionInInventory(const char* name);

/** Purchase a potion from the shop */
int Game_PurchasePotion(int shopIndex);

/** Use a potion from inventory */
int Game_UsePotion(int inventoryIndex);

/* ============================================================
 * SCREEN FUNCTIONS
 * ============================================================ */

/** Render the main menu screen */
void Screen_MainMenu(void);

/** Render the shop screen */
void Screen_Shop(void);

/** Render the inventory screen */
void Screen_Inventory(void);

/** Render the game over screen */
void Screen_GameOver(int won);

/* ============================================================
 * UTILITY FUNCTIONS
 * ============================================================ */

/** Display a temporary message */
void Game_DisplayMessage(const char* format, ...);

/** Generate random number in range [min, max] */
int Game_RandomInt(int minVal, int maxVal);

#ifdef __cplusplus
}
#endif

#endif /* GAME_MAIN_H */
