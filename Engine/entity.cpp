/**
 * @file entity.cpp
 * @brief Entity Management Implementation
 */

#define _CRT_SECURE_NO_WARNINGS

#include "entity.h"
#include "render.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* ============================================================
 * INTERNAL TYPE FOR SORTING
 * ============================================================ */

typedef struct {
    float x, y;
    char glyph;
    int fgColor, bgColor;
    int layer;
} RenderItem;

/* ============================================================
 * INTERNAL STATE
 * ============================================================ */

static Entity g_entities[MAX_ENTITIES];
static int g_entityCount = 0;

/** Initialize entity manager */
void EntityMgr_Init(void) {
    memset(g_entities, 0, sizeof(g_entities));
    g_entityCount = 0;
}

/** Shutdown entity manager and free all components */
void EntityMgr_Shutdown(void) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (g_entities[i].active) {
            for (int c = 0; c < COMPONENT_COUNT; c++) {
                if (g_entities[i].components[c]) {
                    free(g_entities[i].components[c]);
                    g_entities[i].components[c] = NULL;
                }
            }
            g_entities[i].active = 0;
        }
    }
    g_entityCount = 0;
}

/** Create a new entity, returns its ID or INVALID_ENTITY_ID if full */
EntityID EntityMgr_Create(void) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!g_entities[i].active) {
            memset(&g_entities[i], 0, sizeof(Entity));
            g_entities[i].id = (EntityID)i;
            g_entities[i].active = 1;
            g_entityCount++;
            return g_entities[i].id;
        }
    }
    return INVALID_ENTITY_ID; /* No free slots */
}

/** Destroy an entity and all its components */
void EntityMgr_Destroy(EntityID id) {
    if (id >= MAX_ENTITIES) return;
    
    Entity* ent = &g_entities[id];
    if (!ent->active) return;
    
    /* Free all components */
    for (int c = 0; c < COMPONENT_COUNT; c++) {
        if (ent->components[c]) {
            free(ent->components[c]);
            ent->components[c] = NULL;
        }
    }
    
    ent->active = 0;
    g_entityCount--;
}

int EntityMgr_IsActive(EntityID id) {
    if (id >= MAX_ENTITIES) return 0;
    return g_entities[id].active;
}

Entity* EntityMgr_Get(EntityID id) {
    if (id >= MAX_ENTITIES) return NULL;
    if (!g_entities[id].active) return NULL;
    return &g_entities[id];
}

/** Add a component to an entity */
void EntityMgr_AddComponent(EntityID id, ComponentType type, void* component) {
    if (id >= MAX_ENTITIES) return;
    
    Entity* ent = &g_entities[id];
    if (!ent->active || !component) return;
    
    /* Set owner reference */
    ((Component*)component)->owner = ent;
    ent->components[type] = (Component*)component;
}

/** Get a component from an entity (returns NULL if not found) */
void* EntityMgr_GetComponent(EntityID id, ComponentType type) {
    if (id >= MAX_ENTITIES) return NULL;
    
    Entity* ent = &g_entities[id];
    if (!ent->active) return NULL;
    
    return ent->components[type];
}

/** Remove a component from an entity (does NOT free memory) */
void EntityMgr_RemoveComponent(EntityID id, ComponentType type) {
    if (id >= MAX_ENTITIES) return;
    
    Entity* ent = &g_entities[id];
    if (!ent->active) return;
    
    ent->components[type] = NULL;
}

/** Update all active entities */
void EntityMgr_UpdateAll(float deltaTime) {
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!g_entities[i].active) continue;
        
        /* Update velocity components */
        VelocityComponent* vel = (VelocityComponent*)g_entities[i].components[COMPONENT_VELOCITY];
        TransformComponent* trans = (TransformComponent*)g_entities[i].components[COMPONENT_TRANSFORM];
        
        if (vel && trans) {
            Component_UpdateVelocity(vel, trans, deltaTime);
        }
        
        /* Update health components */
        HealthComponent* health = (HealthComponent*)g_entities[i].components[COMPONENT_HEALTH];
        if (health) {
            Component_UpdateHealth(health, deltaTime);
        }
        
        /* Update timer components */
        TimerComponent* timer = (TimerComponent*)g_entities[i].components[COMPONENT_TIMER];
        if (timer) {
            Component_UpdateTimer(timer, deltaTime);
        }
        
        /* Update animation components */
        AnimationComponent* anim = (AnimationComponent*)g_entities[i].components[COMPONENT_ANIMATION];
        SpriteComponent* sprite = (SpriteComponent*)g_entities[i].components[COMPONENT_SPRITE];
        
        if (anim && sprite) {
            Component_UpdateAnimation(anim, sprite);
        }
    }
}

/** Render all entities with visible sprites/text */
void EntityMgr_RenderAll(void) {
    /* Collect renderable items and sort by layer */
    RenderItem renderItems[MAX_ENTITIES];
    int itemCount = 0;
    
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!g_entities[i].active) continue;
        
        SpriteComponent* sprite = (SpriteComponent*)g_entities[i].components[COMPONENT_SPRITE];
        TransformComponent* trans = (TransformComponent*)g_entities[i].components[COMPONENT_TRANSFORM];
        
        if (sprite && trans && sprite->visible) {
            if (itemCount < MAX_ENTITIES) {
                renderItems[itemCount].x = trans->x;
                renderItems[itemCount].y = trans->y;
                renderItems[itemCount].glyph = sprite->glyph;
                renderItems[itemCount].fgColor = sprite->foregroundColor;
                renderItems[itemCount].bgColor = sprite->backgroundColor;
                renderItems[itemCount].layer = sprite->layer;
                itemCount++;
            }
        }
    }
    
    /* Simple bubble sort by layer */
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            if (renderItems[j].layer > renderItems[j + 1].layer) {
                RenderItem temp = renderItems[j];
                renderItems[j] = renderItems[j + 1];
                renderItems[j + 1] = temp;
            }
        }
    }
    
    /* Render all items */
    for (int i = 0; i < itemCount; i++) {
        int x = (int)renderItems[i].x;
        int y = (int)renderItems[i].y;
        
        if (x >= 0 && x < RENDER_MAX_WIDTH && y >= 0 && y < RENDER_MAX_HEIGHT) {
            /* Use ANSI escape codes to print colored character */
            printf("\033[%dm%c\033[0m", renderItems[i].fgColor, renderItems[i].glyph);
        }
    }
}
