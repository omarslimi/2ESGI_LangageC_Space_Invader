#include "../include/score.h"
#include <stdio.h>


int load_best_score()
{
    FILE *file = fopen("assets/score/best_score.txt", "r");
    int score = 0;
    if (file != NULL)
    {
        fscanf(file, "%d", &score);
        fclose(file);
    }
    return score;
}

void save_best_score(int score)
{
    FILE *file = fopen("assets/score/best_score.txt", "w");
    if (file != NULL)
    {
        fprintf(file, "%d", score);
        fclose(file);
    }
}
