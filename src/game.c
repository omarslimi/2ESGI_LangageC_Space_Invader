#include "../include/game.h"
#include "../include/structs.h"
#include "../include/constantes.h"
#include "../include/vague.h"

#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

extern SDL_Surface *ecran;
extern Player player;
extern Projectile projectile;
extern Minion *minions;
extern Projectile *minion_projectiles;
extern int nb_minions;
extern int niveau;
extern Vague vagues[];
extern int total_vagues;
extern int boss_mode;
extern Boss boss;
extern BossProjectile boss_projectile;
extern int scenarisation_active;


void Game_init(int niveau)
{
    srand(time(NULL));

    boss_mode = ((niveau + 1) % 5 == 0);

    if (boss_mode)
    {
        if (minions)
        {
            free(minions);
            minions = NULL;
        }
        if (minion_projectiles)
        {
            free(minion_projectiles);
            minion_projectiles = NULL;
        }
        nb_minions = 0;

        boss.position.x = largeur / 2;
        boss.position.y = hauteur / 6;
        boss.speed.x = 2 + niveau;
        boss.speed.y = 0;
        boss.vie = 100 + niveau * 20;
        boss.alive = 1;
        boss_projectile.active = 0;
        return;
    }

    if (niveau < total_vagues)
    {
        nb_minions = vagues[niveau].nb_minions;
        if (nb_minions <= 0)
        {
            niveau++;
            Game_init(niveau);
            return;
        }
    }
    else {
        nb_minions = 3 + niveau;
    }

    player.position.x = largeur / 2;
    player.position.y = hauteur / 1.1;
    player.vie = 100;

    minions = malloc(nb_minions * sizeof(Minion));
    minion_projectiles = malloc(nb_minions * sizeof(Projectile));

    for (int i = 0; i < nb_minions; i++)
    {
        minions[i].vie = (niveau < total_vagues) ? vagues[niveau].vie_minion : 100;
        minions[i].position.x = (i + 1) * hauteur / (nb_minions + 1);
        minions[i].position.y = hauteur / 6;
        int vitesse = (niveau < total_vagues) ? vagues[niveau].vitesse_minion : (rand() % 3 + 1);
        minions[i].speed.x = vitesse * ((rand() % 2) ? 1 : -1);
        minions[i].speed.y = 0;
        minions[i].alive = 1;
        minion_projectiles[i].active = 0;
    }

    projectile.active = 0;
}

void draw_player(double x, double y)
{
    filledCircleRGBA(ecran, x, y, 20, 255, 255, 255, 255);
}

void draw_minion(double x, double y)
{
    filledCircleRGBA(ecran, x, y, 20, 255, 0, 0, 255);
}

void player_shoot()
{
    if (projectile.active == 0)
    {
        projectile.active = 1;
        projectile.position.x = player.position.x;
        projectile.position.y = player.position.y;
        projectile.speed.x = 0;
        projectile.speed.y = -20;
    }
}

void affichage()
{
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));

    boxRGBA(ecran, 10, 10, 210, 30, 80, 80, 80, 255);
    boxRGBA(ecran, 10, 10, 10 + player.vie * 2, 30, 0, 255, 0, 255);

    if (!boss_mode)
    {
        for (int i = 0; i < nb_minions; i++)
        {
            if (minions[i].alive)
            {
                draw_minion(minions[i].position.x, minions[i].position.y);
                if (niveau < total_vagues)
                {
                    int bar_width = 40;
                    int bar_height = 4;
                    int bar_y = minions[i].position.y - 30;
                    int bar_x = minions[i].position.x - bar_width / 2;

                    boxRGBA(ecran, bar_x, bar_y,
                            bar_x + bar_width, bar_y + bar_height,
                            80, 80, 80, 255);

                    int vie_width = (minions[i].vie * bar_width) / vagues[niveau].vie_minion;
                    boxRGBA(ecran, bar_x, bar_y,
                            bar_x + vie_width, bar_y + bar_height,
                            255 * (1 - minions[i].vie / (float)vagues[niveau].vie_minion),
                            255 * (minions[i].vie / (float)vagues[niveau].vie_minion),
                            0, 255);
                }
            }
        }
    }

    if (boss_mode && boss_projectile.active)
    {
        filledCircleRGBA(ecran, boss_projectile.position.x, boss_projectile.position.y, 8, 255, 0, 0, 255);
    }

    draw_player(player.position.x, player.position.y);

    if (projectile.active)
    {
        filledCircleRGBA(ecran, projectile.position.x, projectile.position.y, 5, 0, 255, 0, 255);
    }

    if (!boss_mode)
    {
        for (int i = 0; i < nb_minions; i++)
        {
            if (minion_projectiles[i].active && minions[i].alive)
            {
                filledCircleRGBA(ecran,
                                 minion_projectiles[i].position.x,
                                 minion_projectiles[i].position.y,
                                 5, 255, 0, 0, 255);
            }
        }
    }
}
