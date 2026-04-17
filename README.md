# Universal TUI Game Engine

A **pure C/C++/H-only** cross-platform terminal-based game engine. Build any terminal game with zero external dependencies.

## Features

- **Zero Dependencies**: Only uses `stdio.h`, `stdlib.h`, and standard library headers
- **Cross-Platform**: Works on Linux, macOS, Windows (CMD/PowerShell/WSL)
- **Terminal Graphics**: ANSI colors, box-drawing characters, double-buffered output
- **Input System**: Keyboard and mouse support via terminal escape sequences
- **UI Framework**: Buttons, menus, text inputs, panels, sliders, checkboxes
- **Entity System**: Component-based architecture for game objects
- **Audio System**: Terminal beeps and sound effects (pure stdio)
- **Simple API**: 4 lines to create a working game

## Quick Start

### Prerequisites

- A C/C++ compiler (gcc, clang, or MinGW)
- A terminal that supports ANSI escape codes (most modern terminals)

### Building

```bash
# Build the Potion Master demo game
make

# Or build just the engine
make engine_demo

# Clean build artifacts
make clean

# Run the game
make run
```

### Your First Game (3 lines!)

```c
#include "Engine/engine.h"

int main() {
    Engine_Init("My Game", 80, 24);
    Engine_PushState("Main", NULL, my_render_function, NULL);
    Engine_Run();
    return 0;
}
```

## Project Structure

```
PotionGame/
├── Engine/                    # Core engine files
│   ├── engine.h/.cpp          # Main loop, state management, events
│   ├── render.h/.cpp          # Terminal rendering with ANSI colors
│   ├── ui.h/.cpp              # Widget framework (buttons, menus, etc.)
│   ├── component.h/.cpp       # Component types (Transform, Sprite, etc.)
│   ├── entity.h/.cpp          # Entity management system
│   ├── audio.h/.cpp           # Terminal audio/beeps
│   └── utils/                 # Utility libraries
│       ├── mathutils.h/.cpp   # Vec2, random numbers, math helpers
│       ├── stringutils.h/.cpp # String manipulation utilities
│       └── arrayutils.h/.cpp  # Generic dynamic arrays
├── Games/                     # Game implementations
│   └── PotionGame/            # Demo: Potion Master game
│       ├── GameMain.h/.cpp    # Main game code
├── Makefile                   # Cross-platform build system
├── .gitignore                 # Git ignore rules
└── README.md                  # This file
```

## API Reference

### Engine Lifecycle

```c
// Initialize the engine
Engine_Init(const char* title, int screenWidth, int screenHeight);

// Push a game state (scene/menu)
Engine_PushState(const char* name, 
                 void (*update)(float deltaTime),
                 void (*render)(void),
                 void (*cleanup)(void));

// Run the main loop (blocks until quit)
Engine_Run();

// Shutdown
Engine_Shutdown();
```

### Rendering

```c
// Colors
Render_Clear(COLOR_BLACK);
Render_Print("Hello World");
Render_PrintColored("Red Text", COLOR_RED);
Render_PrintfBoldColored(COLOR_YELLOW, "Score: %d", 100);

// Box drawing
Render_DrawBox(x, y, width, height, COLOR_CYAN);
Render_DrawPanelWithBorder(x, y, w, h, bgColor, borderColor);
```

### UI Widgets

```c
// Create widgets
UIWidget* btn = UI_CreateButton(10, 5, "Start Game", on_click, NULL);
UIWidget* menu = UI_CreateMenu("Main Menu");
UI_AddMenuItem(menu, "New Game", on_new_game);

// Update and render
UI_UpdateAll(root_widget);
UI_RenderAll(root_widget);
```

### Entities & Components

```c
// Create an entity
EntityID player = EntityMgr_Create();

// Add components
TransformComponent* trans = Component_CreateTransform(10.0f, 10.0f);
SpriteComponent* sprite = Component_CreateSprite('@', COLOR_GREEN, COLOR_BLACK, 1);

EntityMgr_AddComponent(player, COMPONENT_TRANSFORM, trans);
EntityMgr_AddComponent(player, COMPONENT_SPRITE, sprite);

// Update all entities
EntityMgr_UpdateAll(deltaTime);
```

## Building the Demo: Potion Master

Potion Master is a potion shop management game that demonstrates all engine features:

- **Shop Screen**: Browse and buy potions with your gold
- **Inventory System**: Use purchased potions for various effects
- **UI Navigation**: Menu system with keyboard controls
- **Audio Feedback**: Sound effects for interactions

### Controls

| Key | Action |
|-----|--------|
| UP/DOWN | Navigate menus |
| ENTER | Select/Buy/Use |
| ESC | Go back / Quit |

## Platform Support

| Platform | Compiler | Status |
|----------|----------|--------|
| Linux | gcc/clang | Fully supported |
| macOS | clang/gcc | Fully supported |
| Windows (MinGW) | gcc | Fully supported |
| Windows (WSL) | gcc | Fully supported |
| Windows 10+ CMD | gcc | Supported with ANSI enabled |

## Extending the Engine

### Adding a New Component

```c
// In component.h
typedef struct {
    Component base;
    float customValue;
} CustomComponent;

CustomComponent* Component_CreateCustom(float value);
```

```c
// In component.cpp
CustomComponent* Component_CreateCustom(float value) {
    CustomComponent* comp = calloc(1, sizeof(CustomComponent));
    comp->base.type = COMPONENT_CUSTOM;
    comp->customValue = value;
    return comp;
}
```

### Creating a New Game State

```c
void MyGame_Update(float dt) {
    // Update game logic
    EngineEvent e;
    while (Engine_PollEvent(&e)) {
        if (e.type == ENGINE_EVENT_QUIT) {
            Engine_SetRunning(0);
        }
    }
}

void MyGame_Render(void) {
    Render_Clear(COLOR_BLACK);
    Render_Print("My Game!");
    Render_Present();
}

// Register the state:
Engine_PushState("MyGame", MyGame_Update, MyGame_Render, NULL);
```

## License

This project is open source. Use it to build anything you want!

## Contributing

Contributions welcome! Please ensure your code compiles cleanly with `-Wall -Wextra`.

---

**Made with the Universal TUI Game Engine** - Pure C/C++, Zero Dependencies, Any Platform.
