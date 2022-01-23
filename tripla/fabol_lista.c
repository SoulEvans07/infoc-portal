#include <stdio.h>
#include <stdlib.h>

/* adatszerkezetek */
typedef struct Fa {
    int adat;
    struct Fa *bal, *jobb;
} Fa;

typedef struct Lista {
    int adat;
    struct Lista *kov;
} Lista;

/* rekurziv fuggveny, amelyik bejarja a fat, es listat epit belole.
 * onmagaban nem hasznalhato, csak a kovetkezo fuggvennyel egyutt. */
void fat_bejar_hozzafuz(Fa *gyoker, Lista ***pphova) {
    Lista *uj;

    if (gyoker == NULL)
        return;

    fat_bejar_hozzafuz(gyoker->bal, pphova);

    uj = (Lista*) malloc(sizeof(Lista));
    uj->adat = gyoker->adat;
    uj->kov = NULL;
    **pphova = uj;
    *pphova = &uj->kov;

    fat_bejar_hozzafuz(gyoker->jobb, pphova);
}

/* ez atvesz egy fat a gyokerpointerevel, es felepit egy listat, amely
 * ugyanazokat a szamokat tartalmazza. ezzel a listaval ter vissza.
 * ugyanakkor feladata az is, hogy futasi kornyezetet teremtsen a fenti
 * fuggvenynek, amely munkavaltozonak hasznalja az itteni phova valtozot. */
Lista *fabol_lista(Fa *gyoker) {
    Lista *lista;
    Lista **phova = &lista;

    fat_bejar_hozzafuz(gyoker, &phova);
    *phova = NULL;

    return lista;
}

/* a fa letrehozasahoz segedfuggveny, csak hogy ki lehessen
 * probalni a fentieket. adott szamot tarolo levelet hoz letre. */
Fa *ujfa(int szam) {
    Fa *uj = (Fa*) malloc(sizeof(Fa));
    uj->adat = szam;
    uj->bal = uj->jobb = NULL;
    return uj;
}

int main(void) {
    Fa *gyoker;
    Lista *lista, *iter;

    /* a rajzon lathato fa */
    gyoker = ujfa(5);
    gyoker->bal = ujfa(3);
    gyoker->bal->bal = ujfa(1);
    gyoker->bal->jobb = ujfa(4);
    gyoker->jobb = ujfa(8);
    gyoker->jobb->bal = ujfa(6);

    lista = fabol_lista(gyoker);

    for (iter = lista; iter != NULL; iter = iter->kov)
        printf("%d ", iter->adat);
    printf("\n");

    /* felszabadítások helye */

    return 0;
}
