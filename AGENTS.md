# AGENTS

## C++ Naming Conventions

Use these naming rules unless a local file or subsystem already has a stronger established pattern.

- Prefer simple names over ornamental ones.
- Prefer one-word names when the file or type already gives enough context.
- Avoid unnecessary prefixes like `k` for constants.
- Avoid unnecessary namespaces for game-specific code unless there is a real collision risk.

### Types

- Use `PascalCase` for structs and classes.
- Examples: `Level`, `Position`, `Window`, `Scene`

### Members And Variables

- Use `camelCase` for fields, locals, parameters, and functions.
- Examples: `playerSpawn`, `enemySpawns`, `hasBonusLife`, `keyGolds`, `setPosition`

### Booleans

- Prefer names that read naturally as booleans.
- Examples: `hasBonusLife`, `isVisible`, `isFullscreen`

### Constants

- Prefer scoped constants instead of generic global constants.
- For type-specific dimensions, prefer `static constexpr` inside the type.
- Use uppercase names for constants.
- Example:

```cpp
struct Level {
    static constexpr int WIDTH = 32;
    static constexpr int HEIGHT = 22;
};
```

- Access them as `Level::WIDTH` and `Level::HEIGHT`.

### Level Data Naming

For `klad1` level data, prefer:

- `Level`
- `Position`
- `id`
- `exit`
- `playerSpawn`
- `enemySpawns`
- `hasBonusLife`
- `keyGolds`
- `unknown`
- `tiles`

### General Style Preference

- Choose clarity first.
- Keep names short when context already makes them clear.
- Do not introduce more abstraction or ceremony than the code actually needs.
