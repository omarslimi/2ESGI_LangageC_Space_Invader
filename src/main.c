#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "../include/structs.h"
#include "../include/constantes.h"
#include "../include/game.h"
#include "../include/score.h"
#include "../include/vague.h"

SDL_Surface *ecran = NULL;
int largeur = 800, hauteur = 600;
int niveau = 0, boss_mode = 0;
int total_vagues = 0, scenarisation_active = 0;

Player player;
Minion *minions;
Projectile projectile;
Projectile *minion_projectiles;

Boss boss;
BossProjectile boss_projectile;

Vague vagues[MAX_VAGUES];

Minion *minions;
int nb_minions;

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() != 0)
    {
        fprintf(stderr, "Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    if ((ecran = SDL_SetVideoMode(largeur, hauteur, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
    {
        fprintf(stderr, "Erreur SDL_SetVideoMode : %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        fprintf(stderr, "Erreur Mix_OpenAudio : %s\n", Mix_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    Mix_Music *menuMusic = Mix_LoadMUS("../assets/music/menu_music.mp3");
    Mix_Music *gameMusic = Mix_LoadMUS("../assets/music/game_music.mp3");
    Mix_Music *bossMusic = Mix_LoadMUS("../assets/music/boss_music.mp3");

    if (!menuMusic || !gameMusic || !bossMusic)
    {
        fprintf(stderr, "Erreur Mix_LoadMUS : %s\n", Mix_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    TTF_Font *TitlePolice = NULL;
    TTF_Font *SubTitlePolice = NULL;
    TTF_Font *TextPolice = NULL;
    TTF_Font *TextLevel = NULL;
    const char *police_path = "../assets/fonts/arial.ttf";

    if ((TitlePolice = TTF_OpenFont(police_path, 30)) == NULL)
    {
        fprintf(stderr, "Erreur d'ouverture de la police \"%s\"\n", police_path);
        TTF_Quit();
    }

    if ((SubTitlePolice = TTF_OpenFont(police_path, 25)) == NULL)
    {
        fprintf(stderr, "Erreur d'ouverture de la police \"%s\"\n", police_path);
        TTF_Quit();
    }

    if ((TextPolice = TTF_OpenFont(police_path, 10)) == NULL)
    {
        fprintf(stderr, "Erreur d'ouverture de la police \"%s\"\n", police_path);
        TTF_Quit();
    }

    if ((TextLevel = TTF_OpenFont(police_path, 15)) == NULL)
    {
        fprintf(stderr, "Erreur d'ouverture de la police \"%s\"\n", police_path);
        TTF_Quit();
    }

    SDL_WM_SetCaption("Space Invader", NULL);

    Game_init(niveau);


    int active = 1;
    int selection = 0;
    int etat = 0;
    int musique_actuelle = 0;
    Mix_PlayMusic(menuMusic, -1);
    musique_actuelle = 0;
    int move_left = 0;
    int move_right = 0;
    int shooting = 0;
    int best_level = niveau > load_best_score() ? niveau : load_best_score();
    char niveau_text[32];
    SDL_Event event;
    int restart_frame = 0;
    int volume = MIX_MAX_VOLUME;
    int largeur = 800;
    int hauteur = 600;


    load_vagues("../assets/config/vagues.txt");

    int steps = 5;
    while (active)
    {

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                active = 0;
                niveau = 0;
            }

            if (event.type == SDL_KEYDOWN)
            {
                if (etat == 0)
                {
                    if (event.key.keysym.sym == SDLK_DOWN)
                    {
                        selection = (selection + 1) % 3;
                    }
                    if (event.key.keysym.sym == SDLK_UP)
                    {
                        selection = (selection - 1 + 3) % 3;
                    }
                    if (event.key.keysym.sym == SDLK_RETURN)
                    {
                        if (selection == 0)
                        {
                            etat = 1;
                            Game_init(niveau);
                        }
                        else if (selection == 1)
                        {
                            etat = 2;
                        }
                        else if (selection == 2)
                        {
                            active = 0;
                        }
                    }
                }

                if (etat == 1 && event.key.keysym.sym == SDLK_SPACE)
                {
                    shooting = 1;
                }

                if (etat == 1 && event.key.keysym.sym == SDLK_LEFT)
                {
                    move_left = 1;
                }

                if (etat == 1 && event.key.keysym.sym == SDLK_RIGHT)
                {
                    move_right = 1;
                }

                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    etat = 0;
                    move_left = 0;
                    move_right = 0;
                    shooting = 0;

                    if (musique_actuelle != 0)
                    {
                        Mix_HaltMusic();
                        Mix_PlayMusic(menuMusic, -1);
                        musique_actuelle = 0;
                    }
                }
                if (etat == 2)
                {
                    if (event.key.keysym.sym == SDLK_LEFT)
                    {
                        volume = (volume > 0) ? volume - 13 : 0;
                        Mix_VolumeMusic(volume);
                    }
                    if (event.key.keysym.sym == SDLK_RIGHT)
                    {
                        volume = (volume < MIX_MAX_VOLUME) ? volume + 13 : MIX_MAX_VOLUME;
                        Mix_VolumeMusic(volume);
                    }
                }
            }

            if (event.type == SDL_KEYUP && etat == 1)
            {
                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    move_left = 0;
                }
                if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    move_right = 0;
                }
                if (event.key.keysym.sym == SDLK_SPACE)
                {
                    shooting = 0;
                }
            }
        }

        if (etat == 1)
        {
            if (move_left)
            {
                player.position.x -= 5;
            }
            if (move_right)
            {
                player.position.x += 5;
            }
            if (shooting)
            {
                player_shoot();
            }

            if (player.position.x < 20)
            {
                player.position.x = 20;
            }
            if (player.position.x > largeur - 20)
            {
                player.position.x = largeur - 20;
            }

            if (player.vie <= 0)
            {
                etat = 0;
                niveau = 0;
                move_right = 0;
                move_left = 0;
                shooting = 0;
                Mix_PauseMusic();
                Mix_PlayMusic(menuMusic, -1);
                musique_actuelle = 1;

                if (!boss_mode)
                {
                    free(minions);
                    free(minion_projectiles);
                }
            }
        }

        for (int s = 0; s < steps; s++)
        {
            if (projectile.active)
            {
                projectile.position.x += projectile.speed.x / steps;
                projectile.position.y += projectile.speed.y / steps;
                if (projectile.position.y < 0)
                    projectile.active = 0;
            }
        }

        SDL_Surface *textWelcome = NULL;
        SDL_Surface *textPlay = NULL;
        SDL_Surface *textSettings = NULL;
        SDL_Surface *textLeave = NULL;

        SDL_Color blanc = {255, 255, 255, 0};
        SDL_Color gris = {150, 150, 150, 0};

        if (etat == 0)
        {

            if (musique_actuelle != 0)
            {
                Mix_ResumeMusic();
                musique_actuelle = 0;
            }

            SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 64, 118, 173));

            int largeurTexte, hauteurTexte;
            TTF_SizeText(TitlePolice, "Space Invader !", &largeurTexte, &hauteurTexte);

            SDL_Rect positionTitleHome;
            positionTitleHome.x = largeur / 2 - largeurTexte / 2;
            positionTitleHome.y = hauteur / 10;

            textWelcome = TTF_RenderText_Solid(TitlePolice, "Space Invader !", blanc);
            SDL_BlitSurface(textWelcome, NULL, ecran, &positionTitleHome);

            int largeurBtn, hauteurBtn;

            TTF_SizeText(SubTitlePolice, "Jouer", &largeurBtn, &hauteurBtn);

            SDL_Rect positionPlayHome;
            positionPlayHome.x = largeur / 2 - largeurBtn / 2;
            positionPlayHome.y = hauteur / 2 - 60;

            textPlay = (selection == 0) ? TTF_RenderText_Shaded(SubTitlePolice, "Jouer", blanc, gris) : TTF_RenderText_Solid(SubTitlePolice, "Jouer", blanc);
            SDL_BlitSurface(textPlay, NULL, ecran, &positionPlayHome);

            TTF_SizeText(SubTitlePolice, "Options", &largeurBtn, &hauteurBtn);

            SDL_Rect positionSettingsHome;
            positionSettingsHome.x = largeur / 2 - largeurBtn / 2;
            positionSettingsHome.y = hauteur / 2;

            textSettings = (selection == 1) ? TTF_RenderText_Shaded(SubTitlePolice, "Options", blanc, gris) : TTF_RenderText_Solid(SubTitlePolice, "Options", blanc);
            SDL_BlitSurface(textSettings, NULL, ecran, &positionSettingsHome);

            TTF_SizeText(SubTitlePolice, "Quitter", &largeurBtn, &hauteurBtn);

            SDL_Rect positionLeaveHome;
            positionLeaveHome.x = largeur / 2 - largeurBtn / 2;
            positionLeaveHome.y = hauteur / 2 + 60;

            textLeave = (selection == 2) ? TTF_RenderText_Shaded(SubTitlePolice, "Quitter", blanc, gris) : TTF_RenderText_Solid(SubTitlePolice, "Quitter", blanc);
            SDL_BlitSurface(textLeave, NULL, ecran, &positionLeaveHome);

            sprintf(niveau_text, "Meilleur Niveau : %d", best_level + 1);

            SDL_Surface *textNiveau = TTF_RenderText_Solid(TextLevel, niveau_text, blanc);

            SDL_Rect positionBestLevel;
            positionBestLevel.x = 650;
            positionBestLevel.y = 20;

            SDL_BlitSurface(textNiveau, NULL, ecran, &positionBestLevel);
            SDL_FreeSurface(textNiveau);

            SDL_FreeSurface(textWelcome);
            SDL_FreeSurface(textPlay);
            SDL_FreeSurface(textSettings);
            SDL_FreeSurface(textLeave);
        }
        else if (etat == 1)
        {
            affichage();

            sprintf(niveau_text, "Niveau %d", niveau + 1);

            SDL_Surface *textNiveau = TTF_RenderText_Solid(TextLevel, niveau_text, blanc);

            SDL_Rect positionNiveau;
            positionNiveau.x = largeur / 2 - textNiveau->w / 2;
            positionNiveau.y = 10;

            SDL_BlitSurface(textNiveau, NULL, ecran, &positionNiveau);
            SDL_FreeSurface(textNiveau);

            if (boss_mode && boss.alive) {
                if (musique_actuelle != 2)
                {
                    Mix_PauseMusic();
                    Mix_PlayMusic(bossMusic, -1);
                    musique_actuelle = 2;
                }
                boss.position.x += boss.speed.x;
                if (boss.position.x < 40 || boss.position.x > largeur - 40)
                {
                    boss.speed.x *= -1;
                }

                if (!boss_projectile.active && rand() % (60 - niveau * 5) == 0) {
                    boss_projectile.active = 1;
                    boss_projectile.position.x = boss.position.x;
                    boss_projectile.position.y = boss.position.y + 40;
                    boss_projectile.speed.x = 0;
                    boss_projectile.speed.y = 5 + niveau;
                }

                if (boss_projectile.active)
                {
                    boss_projectile.position.y += boss_projectile.speed.y;

                    double dx_proj = boss_projectile.position.x - player.position.x;
                    double dy_proj = boss_projectile.position.y - player.position.y;
                    double dist_proj = sqrt(dx_proj * dx_proj + dy_proj * dy_proj);
                    if (dist_proj < 20 + 8)
                    {
                        player.vie -= 20;
                        boss_projectile.active = 0;
                        if (player.vie < 0)
                            player.vie = 0;
                    }

                    if (boss_projectile.position.y > hauteur)
                    {
                        boss_projectile.active = 0;
                    }
                }

                filledCircleRGBA(ecran, boss.position.x, boss.position.y, 40, 255, 50, 50, 255);

                int bar_width = 200;
                int bar_height = 20;
                int bar_x = largeur - bar_width - 10;
                ;
                int bar_y = 10;
                boxRGBA(ecran, bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, 80, 80, 80, 255);
                int vie_width = (boss.vie > 0 ? boss.vie : 0) * bar_width / (100 + niveau * 20);
                boxRGBA(ecran, bar_x, bar_y, bar_x + vie_width, bar_y + bar_height, 255, 0, 0, 255);

                double dx = boss.position.x - player.position.x;
                double dy = boss.position.y - player.position.y;
                double distance = sqrt(dx * dx + dy * dy);
                if (distance < 40 + 20)
                {
                    player.vie -= 5 + niveau * 2;
                    if (player.vie < 0)
                        player.vie = 0;
                }

                double dxp = boss.position.x - projectile.position.x;
                double dyp = boss.position.y - projectile.position.y;
                double distp = sqrt(dxp * dxp + dyp * dyp);
                if (projectile.active && distp < 40 + 5)
                {
                    boss.vie -= 20;
                    projectile.active = 0;
                    if (boss.vie <= 0)
                    {
                        boss.alive = 0;
                        niveau++;
                        if (niveau > best_level)
                        {
                            best_level = niveau;
                            save_best_score(best_level);
                        }
                        Game_init(niveau);
                        projectile.active = 0;
                        continue;
                    }

                }
            }
            else if (!boss_mode)
            {
                if (musique_actuelle != 1)
                {
                    Mix_PauseMusic();
                    Mix_PlayMusic(gameMusic, -1);
                    musique_actuelle = 1;
                }
                for (int i = 0; i < nb_minions; i++)
                {
                    if (!minions[i].alive)
                        continue;
                    double dx = minions[i].position.x - projectile.position.x;
                    double dy = minions[i].position.y - projectile.position.y;
                    double distance = sqrt(dx * dx + dy * dy);
                    if (projectile.active && distance < 25)
                    {
                        if (niveau < total_vagues)
                        {
                            minions[i].vie -= 20;
                            projectile.active = 0;

                            if (minions[i].vie <= 0)
                            {
                                minions[i].alive = 0;
                                minion_projectiles[i].active = 0;
                            }
                        }
                        else
                        {
                            minions[i].alive = 0;
                            minion_projectiles[i].active = 0;
                            projectile.active = 0;
                        }
                    }
                }

                int tous_morts = 1;
                for (int i = 0; i < nb_minions; i++)
                {
                    if (minions[i].alive)
                    {
                        tous_morts = 0;
                        break;
                    }
                }
                if (tous_morts)
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
                    niveau++;
                    if (niveau > best_level)
                    {
                        best_level = niveau;
                        save_best_score(best_level);
                    }
                    printf("niveau=%d boss_mode=%d\n", niveau, boss_mode);
                    Game_init(niveau);
                    projectile.active = 0;
                    restart_frame = 1;
                    continue;
                }

                if (projectile.active && projectile.position.y < 0)
                {
                    projectile.active = 0;
                }
            }

            for (int i = 0; i < nb_minions; i++)
            {
                if (!minions[i].alive)
                {
                    minion_projectiles[i].active = 0;
                    continue;
                }
                minions[i].position.x += minions[i].speed.x;
                if (minions[i].position.x < 30 || minions[i].position.x > largeur - 30)
                {
                    minions[i].speed.x *= -1;
                }

                if (minion_projectiles[i].active)
                {
                    minion_projectiles[i].position.x += minion_projectiles[i].speed.x;
                    minion_projectiles[i].position.y += minion_projectiles[i].speed.y;

                    if (
                        minion_projectiles[i].position.x >= player.position.x - 20 &&
                        minion_projectiles[i].position.x <= player.position.x + 20 &&
                        minion_projectiles[i].position.y >= player.position.y - 20 &&
                        minion_projectiles[i].position.y <= player.position.y + 20)
                    {
                        minion_projectiles[i].active = 0;
                        player.vie -= 20;
                        if (player.vie < 0)
                        {
                            player.vie = 0;
                        }
                    }

                    if (minion_projectiles[i].position.y > hauteur)
                    {
                        minion_projectiles[i].active = 0;
                    }
                }
            }

            for (int i = 0; i < nb_minions; i++)
            {
                if (!minion_projectiles[i].active)
                {
                    if (rand() % 50 == 0)
                    {
                        minion_projectiles[i].active = 1;
                        minion_projectiles[i].position = minions[i].position;
                        minion_projectiles[i].speed.x = 0;
                        minion_projectiles[i].speed.y = 10;
                    }
                }
            }
        }
        else
        {

            SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 64, 118, 173));

            SDL_Rect positionTitleOptions;
            positionTitleOptions.x = 300;
            positionTitleOptions.y = 80;

            textWelcome = TTF_RenderText_Solid(TitlePolice, "OPTIONS !", blanc);
            SDL_BlitSurface(textWelcome, NULL, ecran, &positionTitleOptions);

            char volume_text[20];
            sprintf(volume_text, "Volume : %d%%", (volume * 100) / MIX_MAX_VOLUME);
            SDL_Surface *textVolume = TTF_RenderText_Solid(SubTitlePolice, volume_text, blanc);
            SDL_Rect positionVolume;
            positionVolume.x = largeur / 2 - textVolume->w / 2;
            positionVolume.y = hauteur / 2;
            SDL_BlitSurface(textVolume, NULL, ecran, &positionVolume);

            SDL_Surface *textInstructions = TTF_RenderText_Solid(TextLevel,
            "Utilisez les fleches GAUCHE/DROITE pour ajuster le volume", blanc);
            SDL_Rect positionInstructions;
            positionInstructions.x = largeur / 2 - textInstructions->w / 2;
            positionInstructions.y = hauteur / 2 + 50;
            SDL_BlitSurface(textInstructions, NULL, ecran, &positionInstructions);

            SDL_FreeSurface(textVolume);
            SDL_FreeSurface(textInstructions);
        }
        if (restart_frame)
        {
            restart_frame = 0;
            continue;
        }
        SDL_Flip(ecran);

        SDL_Delay(1000 / 60);
    }

    Mix_FreeMusic(menuMusic);
    Mix_CloseAudio();
    if (minions)
        free(minions);
    if (minion_projectiles)
        free(minion_projectiles);
    save_best_score(best_level);
    SDL_FreeSurface(ecran);
    TTF_Quit();
    SDL_Quit();
    exit(EXIT_SUCCESS);
}