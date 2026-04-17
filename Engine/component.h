/**
 * @file component.h
 * @brief Component System Header
 * 
 * Defines the base component interface and all built-in component types.
 * Components are data containers that attach to entities to give them behavior.
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * COMPONENT TYPES
 * ============================================================ */

typedef enum {
    COMPONENT_NONE = 0,
    COMPONENT_TRANSFORM,      /* Position, rotation, scale */
    COMPONENT_SPRITE,         /* Visual representation */
    COMPONENT_TEXT,           /* Text display */
    COMPONENT_HEALTH,         /* Health/life points */
    COMPONENT_VELOCITY,       /* Movement velocity */
    COMPONENT_COLLIDER,       /* Collision detection */
    COMPONENT_INPUT,          /* Player input handling */
    COMPONENT_TIMER,          /* Countdown timer */
    COMPONENT_ANIMATION,      /* Frame animation */
    COMPONENT_CUSTOM,         /* User-defined component */
    COMPONENT_COUNT
} ComponentType;

/* ============================================================
 * BASE COMPONENT STRUCTURE
 * ============================================================ */

/** Base component - all components must start with these fields */
typedef struct {
    ComponentType type;
    int enabled;
    void* owner;  /* Pointer to owning entity */
} Component;

/* ============================================================
 * TRANSFORM COMPONENT
 * ============================================================ */

typedef struct {
    Component base;
    
    /* Position (character coordinates for TUI) */
    float x, y;
    
    /* Scale (1.0 = normal size) */
    float scaleX;
    float scaleY;
    
    /* Rotation (in degrees, 0-360) */
    float rotation;
} TransformComponent;

/* ============================================================
 * SPRITE COMPONENT
 * ============================================================ */

typedef struct {
    Component base;
    
    /* Character to display */
    char glyph;
    
    /* Color */
    int foregroundColor;
    int backgroundColor;
    
    /* Visibility */
    int visible;
    
    /* Layer (higher = drawn on top) */
    int layer;
} SpriteComponent;

/* ============================================================
 * TEXT COMPONENT
 * ============================================================ */

typedef struct {
    Component base;
    
    /* Text content */
    char text[256];
    
    /* Color */
    int color;
    
    /* Bold text */
    int bold;
} TextComponent;

/* ============================================================
 * HEALTH COMPONENT
 * ============================================================ */

typedef struct {
    Component base;
    
    /* Current and maximum health */
    float currentHealth;
    float maxHealth;
    
    /* Regeneration rate (per second) */
    float regenRate;
} HealthComponent;

/* ============================================================
 * VELOCITY COMPONENT
 * ============================================================ */

typedef struct {
    Component base;
    
    /* Velocity in character positions per second */
    float vx, vy;
    
    /* Maximum speed */
    float maxSpeed;
} VelocityComponent;

/* ============================================================
 * COLLIDER COMPONENT
 * ============================================================ */

typedef struct {
    Component base;
    
    /* Bounding box (in character units) */
    int width;
    int height;
    
    /* Collision enabled */
    int enabled;
} ColliderComponent;

/* ============================================================
 * INPUT COMPONENT
 * ============================================================ */

typedef struct {
    Component base;
    
    /* Key bindings */
    int keyUp;
    int keyDown;
    int keyLeft;
    int keyRight;
    int keyAction;
} InputComponent;

/* ============================================================
 * TIMER COMPONENT
 * ============================================================ */

typedef struct {
    Component base;
    
    /* Current time remaining (seconds) */
    float currentTime;
    
    /* Total duration (seconds) */
    float duration;
    
    /* Repeat timer */
    int loop;
    
    /* Timer completed callback */
    void (*onComplete)(void* userData);
    void* userData;
} TimerComponent;

/* ============================================================
 * ANIMATION COMPONENT
 * ============================================================ */

typedef struct {
    Component base;
    
    /* Frame characters */
    char frames[16];
    int frameCount;
    
    /* Current frame index */
    int currentFrame;
    
    /* Frames per second */
    float fps;
    
    /* Elapsed time for current frame */
    float frameTimer;
    
    /* Loop animation */
    int loop;
} AnimationComponent;

/* ============================================================
 * COMPONENT CREATION FUNCTIONS
 * ============================================================ */

TransformComponent*  Component_CreateTransform(float x, float y);
SpriteComponent*     Component_CreateSprite(char glyph, int fgColor, int bgColor, int layer);
TextComponent*       Component_CreateText(const char* text, int color, int bold);
HealthComponent*     Component_CreateHealth(float maxHP);
VelocityComponent*   Component_CreateVelocity(float vx, float vy);
ColliderComponent*   Component_CreateCollider(int width, int height);
InputComponent*      Component_CreateInput();
TimerComponent*      Component_CreateTimer(float duration, int loop);
AnimationComponent*  Component_CreateAnimation(const char* frames, int frameCount, float fps, int loop);

/* ============================================================
 * COMPONENT UPDATE FUNCTIONS
 * ============================================================ */

void Component_UpdateTransform(TransformComponent* comp, float deltaTime);
void Component_UpdateHealth(HealthComponent* comp, float deltaTime);
void Component_UpdateVelocity(VelocityComponent* comp, TransformComponent* transform, float deltaTime);
void Component_UpdateTimer(TimerComponent* comp, float deltaTime);
void Component_UpdateAnimation(AnimationComponent* comp, SpriteComponent* sprite);

/* ============================================================
 * UTILITY FUNCTIONS
 * ============================================================ */

/** Check if two colliders overlap */
int Component_Collides(ColliderComponent* a, TransformComponent* transA,
                       ColliderComponent* b, TransformComponent* transB);

#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_H */
