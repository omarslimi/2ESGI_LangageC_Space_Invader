# Space Invader

Un jeu Space Invader développé en C avec SDL 1.2.

## Prérequis

### Windows
1. Installer MSYS2 : https://www.msys2.org/
2. Ouvrir MSYS2 MinGW64 et installer SDL :
```bash
pacman -S mingw-w64-x86_64-SDL mingw-w64-x86_64-SDL_ttf mingw-w64-x86_64-SDL_gfx mingw-w64-x86_64-SDL_mixer
```

### Linux
```bash
sudo apt install libsdl1.2-dev libsdl-ttf2.0-dev libsdl-gfx1.2-dev libsdl-mixer1.2-dev
```

## Compilation et lancement

```bash
make
```

Le jeu se lance automatiquement après compilation.

## Structure du projet

- `src/` : Code source
- `include/` : Fichiers d'en-tête
- `assets/` : Resources (musiques, polices)