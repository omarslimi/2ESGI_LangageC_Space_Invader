#include "../include/vague.h"
#include "../include/structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Vague vagues[];
extern int total_vagues;
extern int scenarisation_active;

void load_vagues(const char *chemin_fichier)
{
    FILE *f = fopen(chemin_fichier, "r");
    if (!f)
    {
        f = fopen(chemin_fichier, "w");
        if (!f)
        {
            fprintf(stderr, "Impossible de créer le fichier %s\n", chemin_fichier);
            return;
        }

        fprintf(f,
                "### Format : nb_minions vitesse_minion vie_minion ###\n"
                "### Chaque ligne = une vague (niveau) ###\n"
                "### Exemple : 5 3 120 → 5 ennemis, vitesse 3, vie 120 ###\n");
        fclose(f);
        printf("Fichier %s créé. Aucun scénario défini.\n", chemin_fichier);
        return;
    }

    char ligne[128];
    while (fgets(ligne, sizeof(ligne), f))
    {
        if (ligne[0] == '#' || ligne[0] == '\n')
            continue;

        Vague v;
        if (sscanf(ligne, "%d %d %d", &v.nb_minions, &v.vitesse_minion, &v.vie_minion) == 3)
        {
            if (v.nb_minions > 0 && v.vitesse_minion >= 0 && v.vie_minion > 0)
            {
                vagues[total_vagues++] = v;
            }
        }
    }

    fclose(f);

    if (total_vagues > 0) {
        scenarisation_active = 1;
    }
}
