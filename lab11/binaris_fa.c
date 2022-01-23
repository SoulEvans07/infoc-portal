#include <stdio.h>
#include <stdlib.h>


/* Az adatszerkezet */
typedef struct BiFa {
    int ertek; /* Érték */
    struct BiFa *bal, *jobb; /* Pointerek */
} BiFa;


/* Beszúró függvény (sorrendben) */
BiFa *beszur(BiFa *gyoker, int ertek) {
    if (gyoker == NULL) {
        BiFa *uj=(BiFa*) malloc(sizeof(BiFa));
        uj->ertek=ertek;
        uj->bal = uj->jobb = NULL;
        return uj;
    }
    if (ertek < gyoker->ertek) {        /* balra szur */
        gyoker->bal = beszur(gyoker->bal, ertek);
    }
    else if (ertek > gyoker->ertek) {   /* jobbra szur */
        gyoker->jobb = beszur(gyoker->jobb, ertek);
    }
    else {
        /* mar benne van */
    }
    return gyoker;
}


/* Fa felszabadítása */
void torol(BiFa *gyoker) {
    if (gyoker != NULL) { /* Ha nem NULL */
        torol(gyoker->jobb); /* Töröljük a jobb részfát */
        torol(gyoker->bal);  /* Töröljük a bal részfát */
        free(gyoker);        /* Ezután már felszabadíthatjuk az aktuális elemet */
    }
}


/* Inorder bejárás */
void inorder(BiFa *gy) {
    if (gy!=NULL) { /* Ha nem NULL */
        inorder(gy->bal); /* Bejárjuk a bal részfát */
        printf("%d ", gy->ertek); /* Feldolgozzuk az aktuális elemet */
        inorder(gy->jobb); /* Bejárjuk a jobb részfát */
    }
}


/* Elemszám */
int elemszam(BiFa *gy) {
    if (gy==NULL)
        return 0; /* Üres (rész)fa */
    else
        return elemszam(gy->bal) + elemszam(gy->jobb) + 1; /* Összeadjuk a részfák elemszámát + 1 az aktuális elem */
}


/* Elemek összege */
int elemosszeg(BiFa *gy) {
    if (gy==NULL)
        return 0; /* Üres (rész)fa */
    else
        return elemosszeg(gy->bal) + elemosszeg(gy->jobb) + gy->ertek; /* Összeadjuk a részfák elemeit + az aktuális értéket */
}


/* Elem keresése */
BiFa* keres(BiFa *gy, int mit) {
    if (gy!=NULL) { /* Ha nem üres */
        if (gy->ertek==mit) return gy; /* Ha ez az az elem akkor megvan */
        if (gy->ertek>mit) return keres(gy->bal, mit); /* Ha kisebb az aktuálisnál, akkor balra folytatjuk a keresést */
        if (gy->ertek<mit) return keres(gy->jobb, mit); /* Ha nagyobb, akkor jobbra */
    }
    return NULL; /* Különben nincs meg */
}


/* Főprogram */
int main(void) {
    int i;
    BiFa *gyoker=NULL;
    int minta[]={15, 96, 34, 12, 14, 56, 21, 11, 10, 9, 78, 43, 0};
    BiFa *ker;

    /* Fa felépítése */
    for (i=0; minta[i]>0; i++)
        gyoker=beszur(gyoker, minta[i]);

    inorder(gyoker); /* Bejárás */
    printf("\nElemszam: %d\n", elemszam(gyoker)); /* Elemszám */
    printf("Elemek osszege: %d\n", elemosszeg(gyoker)); /* Elemösszeg */

    ker = keres(gyoker, 56);
    printf("Kereses: %d\n", ker->ertek); /* Keresés */

    torol(gyoker); /* A dinamikus adatszerkezetet fel kell szabadítani! */

    return 0;
}
