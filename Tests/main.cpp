#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;

#define max 20

typedef struct 
{
    int etat_dep;
    char sym;
    int etat_arr;
}transition;                

typedef struct 
{
    int nb_etats;
    char *alphabet;
    int etat_init;
    int nb_etats_fin;
    int *etats_fin;
    int nb_transitions;
    transition *liste_transition;
}afd;

afd *read(string namefile)
{
    ifstream fichier;
    fichier.open(namefile);

    afd *automate = new afd;
    automate->alphabet = new char[max];
    automate->etats_fin = new int[max];
    automate->liste_transition = new transition[max];

    int ligne = 1, 
        colonne = 0, save_colonne = 0, 
        indice_alphabet = 0, indice_etats_finaux = 0, indice_liste_transitions = 0;
    char lettre = '.';
    
    while(!fichier.fail())
    {
        fichier.get(lettre);

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

    automate->alphabet = (char *)realloc(automate->alphabet, sizeof(char) * indice_alphabet);
    automate->etats_fin = (int *)realloc(automate->etats_fin, sizeof(int) * automate->nb_etats_fin);
    automate->liste_transition = (transition *)realloc(automate->liste_transition, sizeof(transition)* automate->nb_transitions);

    fichier.close();
    
    return automate;
}

void print(afd *automate)
{
    system("cls");
    cout << endl << endl;

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
    delete [] automate->liste_transition;
    delete [] automate->etats_fin;
    delete [] automate->alphabet;
    delete automate;
}

bool accept(afd *automate, string mot)
{
    char lettre = '.';
    bool appartenance_alphabet = false, etat_est_final = false, mot_est_fini = false, etat_avec_transition = true;

    for(int i = 0; i < mot.length(); i ++)
    {
        appartenance_alphabet = false;

        for(int j = 0; j < sizeof(automate->alphabet); j ++)
            if(mot[i] == automate->alphabet[j])
            {
                appartenance_alphabet = true;
                break;
            }  

        if(appartenance_alphabet == false)
        {
            cout << "\nLe mot n'est pas defini sur l'alphabet.\n";
            return false;  
        }
    }
                
    int etat_courant = automate->etat_init,
        sauvegarde_indice_transition = 0,
        i = 0;

    cout << "\n----------------------\n\nExecution de '" << mot << "':\n";

    while(i < mot.length() && etat_avec_transition == true)
    {
        if(sauvegarde_indice_transition == automate->nb_transitions - 1)
            sauvegarde_indice_transition = 0;
        
        // Vérification si la transition existe et on affecte la nouvelle position.
        etat_avec_transition = false;
        etat_est_final = false;

        for(int j = sauvegarde_indice_transition; j < automate->nb_transitions; j ++)
        {
            if(etat_courant == automate->liste_transition[j].etat_dep && mot[i] == automate->liste_transition[j].sym)
            {
                printf("t(%d, %c) = %d\n", etat_courant, mot[i], automate->liste_transition[j].etat_arr);
                etat_courant = automate->liste_transition[j].etat_arr;
                etat_avec_transition = true;
                break;
            }
        }

        // Vérification si l'etat est final ou non.

        for(int j = 0; j < automate->nb_etats_fin; j ++)
            if(etat_courant == automate->etats_fin[j])
            {
                etat_est_final = true;
                break;
            }
            
        i ++;
    }

    if(i < mot.length() || (i < mot.length() && etat_avec_transition == false))
        mot_est_fini = false;
    else
        mot_est_fini = true;
    
    if(mot_est_fini == true && etat_est_final == true)
    {
        cout << "\nL'etat (" << etat_courant << ") est final et le mot est fini.\n";
        return true;
    }
    if(mot_est_fini == false && etat_est_final == true)
        cout << "\nL'etat (" << etat_courant << ") est final mais le mot n'est pas fini.\n";
    else if(mot_est_fini == true && etat_est_final == false)
        cout << "\nLe mot est fini mais l'etat (" << etat_courant << ") n'est pas final.\n";
    else if(mot_est_fini == false && etat_avec_transition == false)
        cout << "\nLe mot n'est pas fini car l'etat (" << etat_courant << ") n'a pas de transition avec '" << mot[i-1] << "'.\n";  

    return false;
}

int main()
{   
    system("cls");
    string mot;

    // Demande du nom du fichier.

    cout << "Entrer le nom du fichier: ";
    cin >> mot;

    // Retour de la structure de l'afd.

    afd *automate = read(mot);

    // Description de l'afd.

    print(automate);

    // Saisie du mot.

    cout << "Entrer un mot: ";
    cin >> mot;
    
    // Traitement du mot et réponse.

    int reponse_acceptation = accept(automate, mot);
    if(reponse_acceptation)
        cout << "\nLe mot '" << mot << "' est valide ? Oui.\n\n";
    else
        cout << "\nLe mot '" << mot << "' est valide ? Non.\n\n";

    // Libération de mémoire.

    Detruire_Automate(automate);

    return 0;
}