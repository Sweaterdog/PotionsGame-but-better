/**
 * @file component.cpp
 * @brief Component System Implementation
 * 
 * Implements all built-in components: Transform, Sprite, Text, Health,
 * Velocity, Collider, Input, Timer, and Animation.
 */

#define _CRT_SECURE_NO_WARNINGS

#include "component.h"
#include <string.h>
#include <stdlib.h>

/* ============================================================
 * TRANSFORM COMPONENT
 * ============================================================ */

TransformComponent* Component_CreateTransform(float x, float y) {
    TransformComponent* comp = (TransformComponent*)calloc(1, sizeof(TransformComponent));
    if (!comp) return NULL;
    
    comp->base.type = COMPONENT_TRANSFORM;
    comp->base.enabled = 1;
    comp->x = x;
    comp->y = y;
    comp->scaleX = 1.0f;
    comp->scaleY = 1.0f;
    comp->rotation = 0.0f;
    
    return comp;
}

void Component_UpdateTransform(TransformComponent* comp, float deltaTime) {
    (void)deltaTime;
    /* Transform updates are typically driven by other components */
}

/* ============================================================
 * SPRITE COMPONENT
 * ============================================================ */

SpriteComponent* Component_CreateSprite(char glyph, int fgColor, int bgColor, int layer) {
    SpriteComponent* comp = (SpriteComponent*)calloc(1, sizeof(SpriteComponent));
    if (!comp) return NULL;
    
    comp->base.type = COMPONENT_SPRITE;
    comp->base.enabled = 1;
    comp->glyph = glyph;
    comp->foregroundColor = fgColor;
    comp->backgroundColor = bgColor;
    comp->visible = 1;
    comp->layer = layer;
    
    return comp;
}

/* ============================================================
 * TEXT COMPONENT
 * ============================================================ */

TextComponent* Component_CreateText(const char* text, int color, int bold) {
    TextComponent* comp = (TextComponent*)calloc(1, sizeof(TextComponent));
    if (!comp) return NULL;
    
    comp->base.type = COMPONENT_TEXT;
    comp->base.enabled = 1;
    comp->color = color;
    comp->bold = bold;
    
    if (text) {
        strncpy(comp->text, text, sizeof(comp->text) - 1);
        comp->text[sizeof(comp->text) - 1] = '\0';
    } else {
        comp->text[0] = '\0';
    }
    
    return comp;
}

/* ============================================================
 * HEALTH COMPONENT
 * ============================================================ */

HealthComponent* Component_CreateHealth(float maxHP) {
    HealthComponent* comp = (HealthComponent*)calloc(1, sizeof(HealthComponent));
    if (!comp) return NULL;
    
    comp->base.type = COMPONENT_HEALTH;
    comp->base.enabled = 1;
    comp->maxHealth = maxHP;
    comp->currentHealth = maxHP;
    comp->regenRate = 0.0f;
    
    return comp;
}

void Component_UpdateHealth(HealthComponent* comp, float deltaTime) {
    if (!comp || !comp->base.enabled) return;
    
    /* Regenerate health */
    if (comp->regenRate > 0.0f && comp->currentHealth < comp->maxHealth) {
        comp->currentHealth += comp->regenRate * deltaTime;
        if (comp->currentHealth > comp->maxHealth) {
            comp->currentHealth = comp->maxHealth;
        }
    }
}

/* ============================================================
 * VELOCITY COMPONENT
 * ============================================================ */

VelocityComponent* Component_CreateVelocity(float vx, float vy) {
    VelocityComponent* comp = (VelocityComponent*)calloc(1, sizeof(VelocityComponent));
    if (!comp) return NULL;
    
    comp->base.type = COMPONENT_VELOCITY;
    comp->base.enabled = 1;
    comp->vx = vx;
    comp->vy = vy;
    comp->maxSpeed = 10.0f;
    
    return comp;
}

void Component_UpdateVelocity(VelocityComponent* comp, TransformComponent* transform, float deltaTime) {
    if (!comp || !transform || !comp->base.enabled) return;
    
    /* Apply velocity */
    transform->x += comp->vx * deltaTime;
    transform->y += comp->vy * deltaTime;
}

/* ============================================================
 * COLLIDER COMPONENT
 * ============================================================ */

ColliderComponent* Component_CreateCollider(int width, int height) {
    ColliderComponent* comp = (ColliderComponent*)calloc(1, sizeof(ColliderComponent));
    if (!comp) return NULL;
    
    comp->base.type = COMPONENT_COLLIDER;
    comp->base.enabled = 1;
    comp->width = width;
    comp->height = height;
    
    return comp;
}

int Component_Collides(ColliderComponent* a, TransformComponent* transA,
                       ColliderComponent* b, TransformComponent* transB) {
    if (!a || !transA || !b || !transB) return 0;
    
    /* AABB collision detection */
    float leftA = transA->x;
    float rightA = transA->x + a->width;
    float topA = transA->y;
    float bottomA = transA->y + a->height;
    
    float leftB = transB->x;
    float rightB = transB->x + b->width;
    float topB = transB->y;
    float bottomB = transB->y + b->height;
    
    return !(rightA <= leftB || leftA >= rightB || 
             bottomA <= topB || topA >= bottomB);
}

/* ============================================================
 * INPUT COMPONENT
 * ============================================================ */

InputComponent* Component_CreateInput() {
    InputComponent* comp = (InputComponent*)calloc(1, sizeof(InputComponent));
    if (!comp) return NULL;
    
    comp->base.type = COMPONENT_INPUT;
    comp->base.enabled = 1;
    
    /* Default key bindings */
    comp->keyUp = 259;     /* KEY_UP */
    comp->keyDown = 258;   /* KEY_DOWN */
    comp->keyLeft = 257;   /* KEY_LEFT */
    comp->keyRight = 261;  /* KEY_RIGHT */
    comp->keyAction = 257; /* Enter by default */
    
    return comp;
}

/* ============================================================
 * TIMER COMPONENT
 * ============================================================ */

TimerComponent* Component_CreateTimer(float duration, int loop) {
    TimerComponent* comp = (TimerComponent*)calloc(1, sizeof(TimerComponent));
    if (!comp) return NULL;
    
    comp->base.type = COMPONENT_TIMER;
    comp->base.enabled = 1;
    comp->duration = duration;
    comp->currentTime = duration;
    comp->loop = loop;
    comp->onComplete = NULL;
    comp->userData = NULL;
    
    return comp;
}

void Component_UpdateTimer(TimerComponent* comp, float deltaTime) {
    if (!comp || !comp->base.enabled) return;
    
    comp->currentTime -= deltaTime;
    
    if (comp->currentTime <= 0.0f) {
        if (comp->onComplete) {
            comp->onComplete(comp->userData);
        }
        
        if (comp->loop) {
            comp->currentTime = comp->duration;
        }
    }
}

/* ============================================================
 * ANIMATION COMPONENT
 * ============================================================ */

AnimationComponent* Component_CreateAnimation(const char* frames, int frameCount, float fps, int loop) {
    AnimationComponent* comp = (AnimationComponent*)calloc(1, sizeof(AnimationComponent));
    if (!comp) return NULL;
    
    comp->base.type = COMPONENT_ANIMATION;
    comp->base.enabled = 1;
    comp->frameCount = frameCount;
    comp->fps = fps;
    comp->currentFrame = 0;
    comp->frameTimer = 0.0f;
    comp->loop = loop;
    
    if (frames && frameCount > 0) {
        int copyCount = frameCount < 16 ? frameCount : 16;
        for (int i = 0; i < copyCount; i++) {
            comp->frames[i] = frames[i];
        }
    }
    
    return comp;
}

void Component_UpdateAnimation(AnimationComponent* comp, SpriteComponent* sprite) {
    if (!comp || !sprite || !comp->base.enabled) return;
    
    comp->frameTimer += 1.0f / 60.0f; /* Assume ~60fps */
    
    float frameDuration = 1.0f / comp->fps;
    
    if (comp->frameTimer >= frameDuration) {
        comp->frameTimer -= frameDuration;
        comp->currentFrame++;
        
        if (comp->currentFrame >= comp->frameCount) {
            if (comp->loop) {
                comp->currentFrame = 0;
            } else {
                comp->currentFrame = comp->frameCount - 1; /* Stay on last frame */
            }
        }
        
        /* Update sprite glyph */
        if (sprite && sprite->visible) {
            sprite->glyph = comp->frames[comp->currentFrame];
        }
    }
}
