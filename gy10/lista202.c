/* Madárnyelv számokkal, láncolt listában.
 * Minden szám után beszúr egy nullát, és még egyszer a számot.
 * Vagyis 1 2 3 4 listából 1 2 0 2 3 4 0 4 lista lesz.
 */
#include <stdio.h>
#include <stdlib.h>

/* Számok láncolt listában. */
typedef struct Szam {
    int szam;
    struct Szam *kov;
} Szam;


/* A paraméterként kapott lista végéhez fűz egy új elemet,
 * amelyben a szintén paraméterként kapott szám van.
 * Visszatér a lista eleje pointerrel, ami lehet, hogy változott.
 * Használat:
 *   l = lista_vegere(l, 5);
 */
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


/* A listában minden páros szám után beszúr egy nullát,
 * és még egyszer magát a számot.
 */
void lista_202(Szam *lista) {
    Szam *iter;

    for (iter = lista; iter != NULL; iter = iter->kov)
        if (iter->szam % 2 == 0) {
            Szam *ujszam, *ujnulla;
            ujszam = (Szam*) malloc(sizeof(Szam));  /* 1 */
            ujszam->szam = iter->szam;
            ujszam->kov = iter->kov;                /* 2 */
            ujnulla = (Szam*) malloc(sizeof(Szam)); /* 3 */
            ujnulla->szam = 0;
            ujnulla->kov = ujszam;                  /* 4 */
            iter->kov = ujnulla;                    /* 5 */
            /* 2-t ugrunk, es utana meg jon a 3. ugras */
            iter = iter->kov->kov;                  /* 6 */
        }
}


/* Kiírja a lista számait a képernyőre, szóközzel elválasztva. */
void lista_kiir(Szam *lista) {
    Szam *iter;
    for (iter = lista; iter != NULL; iter = iter->kov)
        printf("%d ", iter->szam);
    printf("\n");
}


/* Felszabadítja a listát. */
void felszabadit(Szam *lista) {
    while (lista != NULL) {
        Szam *kov = lista->kov;
        free(lista);
        lista = kov;
    }
}


/* Főprogram. */
int main(void) {
    Szam *lista = NULL;
    int beolv;

    printf("Irj be szamokat (vege: 0), es en a parosakat paros0parosozom!\n");
    scanf("%d", &beolv);
    while (beolv != 0) {
        lista = lista_vegere(lista, beolv);
        scanf("%d", &beolv);
    }

    lista_kiir(lista);
    lista_202(lista);
    lista_kiir(lista);

    felszabadit(lista);

    return 0;
}
