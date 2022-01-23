/* Beolvas szavakat a billentyuzetrol, es listat epit beloluk.
 * Utana torli a 4 betuseket, es kiirja a listat. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Lancolt lista eleme, amelyben maximum 50 betus szavak tarolhatoak. */
typedef struct Szo {
    char szo[50 + 1];
    struct Szo *kov;
} Szo;


/* Az adott lista vegehez fuz egy uj elemet, amelybe
 * a megadott szot masolja.
 * Visszater az uj lista eleje mutatoval, amely valtozhatott (ures
 * lista eseten.) Ezert igy kell hasznalni:
 *   szavak = lista_vegere(szavak, "korte");
 */
Szo *lista_vegere(Szo *lista, char *szo) {
    Szo *uj = (Szo*) malloc(sizeof(Szo));
    strcpy(uj->szo, szo);
    uj->kov = NULL;
    if (lista == NULL)
        return uj;
    else {
        Szo *iter;
        for (iter = lista; iter->kov != NULL; iter = iter->kov)
            ; /* üres */
        iter->kov = uj;
        return lista;
    }
}


/* Torli a negy betus szavakat a listabol. Visszater a lista
 * eleje mutatoval, amely valtozhatott, hiszen a lista elejen is
 * lehetett negy betus szo. Hasznalata:
 *   szavak = torol_negybetus(szavak);
 */
Szo *torol_negybetus(Szo *lista) {
    Szo *lemarado = NULL;
    Szo *mozgo = lista;
    
    while (mozgo != NULL) {
        if (strlen(mozgo->szo) == 4) { /* törölni? */
            Szo *torlendo = mozgo;
            mozgo = mozgo->kov;
            if (lemarado == NULL) {    /* első elem? */
                lista = torlendo->kov;
            } else {                   /* köztes elem */
                lemarado->kov = torlendo->kov;
            }
            free(torlendo);
        } else {                       /* nincs törlés */
            lemarado = mozgo;
            mozgo = mozgo->kov;
        }
    }
    return lista;
}


/* Kiirja a lista szavait, szokozzel elvalasztva. */
void lista_kiir(Szo *lista) {
    Szo *iter;
    for (iter = lista; iter != NULL; iter = iter->kov)
        printf("%s ", iter->szo);
    printf("\n");
}


/* Felszabaditja a parameterkent kapott listat. */
void felszabadit(Szo *lista) {
    while (lista != NULL) {
        Szo *kov = lista->kov;
        free(lista);
        lista = kov;
    }
}


int main(void) {
    Szo *lista = NULL;
    char szo[50 + 1];

    printf("Irj be szavakat! Az utolso szo legyen \"x\".\nBerakom oket egy listaba, utana kitorlom a 4 betuseket.\n");

    scanf("%50s", szo);
    while (strcmp("x", szo) != 0) {
        lista = lista_vegere(lista, szo);
        scanf("%50s", szo);
    }

    lista = torol_negybetus(lista);
    lista_kiir(lista);

    felszabadit(lista);

    return 0;
}
