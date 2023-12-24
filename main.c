#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define maxMot 20

typedef struct element
{
    char *mot;
    struct element *next;
}liste, element;


typedef struct 
{
    liste *motcles;
    liste *identificateurs;
    liste *entiers;
    liste *operateursLogiques;
    liste *operateursRelationnel;
    liste *operateursAffectations;
}afd;

typedef enum
{
    false,
    true
}bool;



afd *read(char *namefile)
{
    FILE *fichier = fopen(namefile, "r");

    afd *automate = malloc(sizeof(afd));
    automate->alphabet = malloc(sizeof(char) * max);
    automate->etats_fin = malloc(sizeof(int) * max);
    automate->liste_transition = malloc(sizeof(transition) * max);

    int ligne = 1, 
        colonne = 0, save_colonne = 0, 
        indice_alphabet = 0, indice_etats_finaux = 0, indice_liste_transitions = 0;
    char lettre = '.';
    
    while((lettre = fgetc(fichier)) != EOF)
    {
        if(lettre == '\n')
            ligne ++;
        
        if(lettre != '\n' && lettre != ' ')
        {
            if(ligne >= 6)
                colonne ++;
                
            const char *key = &lettre;

            switch(ligne)
            {
                case 1:
                    automate->nb_etats = atoi(key);
                    break;  
                case 2:
                    automate->alphabet[indice_alphabet ++] = lettre;
                    break;      
                case 3:
                    automate->etat_init = atoi(key);
                    break;
                case 4:
                    automate->nb_etats_fin = atoi(key);
                    break;
                case 5:
                    automate->etats_fin[indice_etats_finaux ++] = atoi(key);
                    break;
                default:
                    break;
            }

            if(ligne >= 6)
            {
                save_colonne = colonne;
                    switch(colonne)
                    {
                        case 1: 
                            automate->liste_transition[indice_liste_transitions].etat_dep = atoi(key);
                            break;
                        case 2:
                            automate->liste_transition[indice_liste_transitions].sym = lettre;
                            break;
                        case 3:
                            automate->liste_transition[indice_liste_transitions].etat_arr = atoi(key);
                            colonne = 0;
                            break;
                        default:
                            break;
                    }
                    if(save_colonne == 3)
                        indice_liste_transitions ++;
            }
        }
}

    automate->alphabet[indice_alphabet] = '\0';
    automate->nb_transitions = indice_liste_transitions;

    automate->alphabet = realloc(automate->alphabet, sizeof(char) * indice_alphabet);
    automate->etats_fin = realloc(automate->etats_fin, sizeof(int) * automate->nb_etats_fin);
    automate->liste_transition = realloc(automate->liste_transition, sizeof(transition)* automate->nb_transitions);

    fclose(fichier);
    
    return automate;
}

void print(afd *automate)
{
    system("cls");
    printf("\n");

    printf("E = { ");
    for(int i = 0; i < automate->nb_etats; i ++)
        printf("%d ", i);
    printf("}\n\nA = { ");
    int i = 0; 
    while(automate->alphabet[i] != '\0')
        printf("%c ", automate->alphabet[i ++]);
        
    printf("}\n\nTransitions:\n");
    for(int i = 0; i < automate->nb_transitions; i ++)
        printf("t(%d, %c) = %d\n", automate->liste_transition[i].etat_dep, 
                                   automate->liste_transition[i].sym, 
                                   automate->liste_transition[i].etat_arr);
    printf("\nI = %d\n", automate->etat_init);
    printf("\nF = { ");
    for(int i = 0; i < automate->nb_etats_fin; i ++)
        printf("%d ", automate->etats_fin[i]);
    printf("}\n");

    printf("\n");
}

void Detruire_Automate(afd *automate)
{
    free(automate->liste_transition);
    free(automate->etats_fin);
    free(automate->alphabet);
    free(automate);
}

bool accept(afd *automate,  char *mot)
{
    char lettre = '.';
    bool apprtenance_alphabet = false;

    for(int i = 0; i < sizeof(mot); i ++)
    {
        apprtenance_alphabet = false;

        for(int j = 0; j < sizeof(automate->alphabet); j ++)
        {
            if(mot[i] == automate->alphabet[j])
            {
                apprtenance_alphabet = true;
                break;
            }      
        }

        if(apprtenance_alphabet  == false)
        {
            printf("\nLe mot n'appartient pas a l'alphabet.\n");
            return false;
        }
    }
                
    int etat_initial = automate->etat_init,
        etat_courant = etat_initial,
        sauvegarde_indice_transition = 0,
        i = 0;

    printf("\n----------------------\n\nExecution de '%s':\n", mot);

    bool etat_est_final = false;

    do
    {
        if(sauvegarde_indice_transition == automate->nb_transitions - 1)
            sauvegarde_indice_transition = 0;
        
        for(int k = sauvegarde_indice_transition; k < automate->nb_transitions; k ++)
            if(etat_courant == automate->liste_transition[k].etat_dep && mot[i] == automate->liste_transition[k].sym)
            {
                printf("t(%d, %c) = %d\n", etat_courant, mot[i], automate->liste_transition[k].etat_arr);
                etat_courant = automate->liste_transition[k].etat_arr;
                sauvegarde_indice_transition = k;
                break;
            }    
        i ++;
    }
    while(i < sizeof(mot));

    bool mot_est_fini = false;

    for(int i = 0; i < automate->nb_etats_fin; i ++)
        if(etat_courant == automate->etats_fin[i])
            etat_est_final = true;

    if(mot[i] == '\0')
        mot_est_fini = true;
    
    if(mot_est_fini == true && etat_est_final == true)
    {
        printf("\nEtat final du mot '%s': (%d)\nLe mot est fini.\n", mot, etat_courant);
        return true;
    }
    else if(mot_est_fini == false && etat_est_final == true)
        printf("\nL'etat (%d) est un etat final mais le mot n'est pas fini.\n", etat_courant);
    else
        printf("\nLe mot est fini mais l'etat (%d) n'est pas un etat final.\n", etat_courant);
        
    return false;
}

void saisie_fichier(char fichier[])
{
    char * temp = malloc(sizeof(char) * max);

    printf("Entrer le nom du fichier: ");
    scanf("%s", temp);

    int i = 0;
    for(i; i < max; i ++)
    {
        if(temp[i] !='\0')
            fichier[i] = temp[i];
        else
            break;
    }
        
    fichier[i] = '\0';

    free(temp);
}

int main()
{
    system("cls");

    // Demande du nom du fichier.

    char fichier[max];
    saisie_fichier(fichier);

    // Retour de la structure de l'afd.

    afd *automate = read(fichier);

    // Description de l'afd.

    print(automate);

    for(int i = 0; i < 5; i ++){
    // Saisie du mot.
    char * temp = malloc(sizeof(char) * max);
    char mot[max];

    printf("Entrer le mot %d: ", i + 1);
    scanf("%s", temp);

    int i = 0;
    for(i; i < max; i ++)
    {
        if(temp[i] !='\0')
            mot[i] = temp[i];
        else
            break;
    }
        
    mot[i] = '\0';

    free(temp);
    
    // Traitement du mot et réponse.

    int reponse_acceptation = accept(automate, mot);
    if(reponse_acceptation)
        printf("\nLe mot '%s' est valide ? Oui.\n\n", mot);
    else
        printf("\nLe mot '%s' est valide ? Non.\n\n", mot);
    }

    // Libération de mémoire.

    Detruire_Automate(automate);

    return 0;
}