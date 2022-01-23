#include <stdio.h>
#include <stdlib.h>

typedef struct Szam {
    int szam;
    struct Szam *kov;
} Szam;

Szam *lista_vegere(Szam *lista, int szam) {
    Szam *uj = (Szam*) malloc(sizeof(Szam));
    uj->szam = szam;
    uj->kov = NULL;
    if (lista == NULL)
        return uj;
    else {
        Szam *iter;
        for (iter = lista; iter->kov != NULL; iter = iter->kov)
            ; /* üres */
        iter->kov = uj;
        return lista;
    }
}


/* Megfordit egy parameterkent kapott listat, es visszater
 * a megforditott valtozattal.
 * Vigyazat, az eredeti lista elveszik! A fuggveny nem uj
 * listat epit, hanem az eredeti lista elemeinek felhasznalasaval
 * epiti az uj listat. Igy a kapott pointert legjobb az eredeti
 * lista eleje pointerbe visszairni:
 *   szamok = lista_megfordit(szamok);
 */
Szam *lista_megfordit(Szam *lista) {
    Szam *eredeti = lista;
    Szam *megforditott = NULL;

    while (eredeti != NULL) {
        Szam *atrakott = eredeti, *kovetkezo = eredeti->kov;
        atrakott->kov = megforditott;   /* uj elejere be */
        megforditott = atrakott;
        eredeti = kovetkezo;            /* regibol kihagy */
    }

    return megforditott;
}

void lista_kiir(Szam *lista) {
    Szam *iter;
    for (iter = lista; iter != NULL; iter = iter->kov)
        printf("%d ", iter->szam);
    printf("\n");
}

void felszabadit(Szam *lista) {
    while (lista != NULL) {
        Szam *kov = lista->kov;
        free(lista);
        lista = kov;
    }
}

int main(void) {
    Szam *lista = NULL;
    int beolv;

    printf("Irj be szamokat (0-ig)! Beteszem oket egy listaba.\n");
    printf("Utana kiirom a listat, megforditom, es kiirom ujra.\n");
    scanf("%d", &beolv);
    while (beolv != 0) {
        lista = lista_vegere(lista, beolv);
        scanf("%d", &beolv);
    }

    lista_kiir(lista);

    lista = lista_megfordit(lista);

    lista_kiir(lista);

    felszabadit(lista);

    return 0;
}
