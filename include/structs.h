#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    double x;
    double y;
} Vec2;

typedef struct {
    Vec2 position;
    int vie;
} Player;

typedef struct {
    Vec2 position;
    Vec2 speed;
    int alive;
    int vie;
} Minion;

typedef struct {
    Vec2 position;
    Vec2 speed;
    int active;
} Projectile;

typedef struct {
    Vec2 position;
    Vec2 speed;
    int vie;
    int alive;
} Boss;

typedef struct {
    Vec2 position;
    Vec2 speed;
    int active;
} BossProjectile;

typedef struct {
    int nb_minions;
    int vitesse_minion;
    int vie_minion;
} Vague;

#endif
