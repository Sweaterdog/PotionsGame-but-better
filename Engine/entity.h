/**
 * @file entity.h
 * @brief Entity Management Header
 * 
 * Simple entity system for managing game objects with component attachments.
 */

#ifndef ENTITY_H
#define ENTITY_H

#include "component.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * CONSTANTS
 * ============================================================ */

#define MAX_ENTITIES      1024
#define INVALID_ENTITY_ID 0xFFFFFFFFU

/* ============================================================
 * ENTITY TYPE
 * ============================================================ */

typedef unsigned int EntityID;

/** Game entity - a container for components */
typedef struct {
    EntityID id;
    int active;
    Component* components[COMPONENT_COUNT];
} Entity;

/* ============================================================
 * ENTITY MANAGER FUNCTIONS
 * ============================================================ */

/** Initialize the entity manager */
void EntityMgr_Init(void);

/** Shutdown the entity manager */
void EntityMgr_Shutdown(void);

/** Create a new entity, returns its ID or INVALID_ENTITY_ID if full */
EntityID EntityMgr_Create(void);

/** Destroy an entity and all its components */
void EntityMgr_Destroy(EntityID id);

/** Check if an entity is active */
int EntityMgr_IsActive(EntityID id);

/** Get pointer to an entity (returns NULL if invalid) */
Entity* EntityMgr_Get(EntityID id);

/* ============================================================
 * COMPONENT ATTACHMENT
 * ============================================================ */

/** Add a component to an entity */
void EntityMgr_AddComponent(EntityID id, ComponentType type, void* component);

/** Get a component from an entity (returns NULL if not found) */
void* EntityMgr_GetComponent(EntityID id, ComponentType type);

/** Remove a component from an entity (does NOT free memory) */
void EntityMgr_RemoveComponent(EntityID id, ComponentType type);

/* ============================================================
 * ENTITY UPDATE & RENDER
 * ============================================================ */

/** Update all active entities */
void EntityMgr_UpdateAll(float deltaTime);

/** Render all entities with visible sprites/text */
void EntityMgr_RenderAll(void);

#ifdef __cplusplus
}
#endif

#endif /* ENTITY_H */
