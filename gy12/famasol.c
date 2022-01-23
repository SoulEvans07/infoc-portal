#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct Fa {
    int szam;                   /* double, float, char, ... */
    struct Fa *bal, *jobb;
} Fa;

/* ezt mindegyik keri */
void torol(Fa *gy) {
    if (gy == NULL)
        return;
    torol(gy->bal);
    torol(gy->jobb);
    free(gy);
}

/* ezt semelyik nem keri, csak nekem kellett */
Fa *uj(int szam) {
    Fa *uj = malloc(sizeof(Fa));
    uj->szam = szam;
    uj->bal = uj->jobb = NULL;
    return uj;
}
/* ezt sem kerte egyik feladat sem */
/* ez a fuggveny arra kepes, hogy kirajzolja
 * a fat - tobbe-kevesbe kovetheto modon.
 * a kirajzolt fa 90 fokkal el van forgatva;
 * bal oldalon van a gyoker, tole jobbra indulnak
 * a leszarmazottak. nem kerte a feladat. */
void kirajzol_eltolva(Fa *gyoker, int szint) {
    if (gyoker == NULL)
        return;
    kirajzol_eltolva(gyoker->jobb, szint + 1);
    /* %*d: a * miatt a szelesseget a kov. parameterbol olvassa */
    printf("%*d\n", szint * 5, gyoker->szam);
    kirajzol_eltolva(gyoker->bal, szint + 1);
}

/* ez a fuggveny csak azert van, hogy az igazi
 * kirajzolo fuggvenynek megadja a kezdeti 0-s
 * parametert. nem kerte a feladatkiiras! */
void kirajzol(Fa *gyoker) {
    kirajzol_eltolva(gyoker, 0);
}

/* A */
Fa *masol(Fa *gy) {
    Fa *uj;
    if (gy == NULL)
        return NULL;
    uj = malloc(sizeof(Fa));
    uj->szam = gy->szam;
    uj->bal = masol(gy->bal);
    uj->jobb = masol(gy->jobb);
    return uj;
}

/* B */
Fa *tukormasol(Fa *gy) {
    Fa *uj;
    if (gy == NULL)
        return NULL;
    uj = malloc(sizeof(Fa));
    uj->szam = gy->szam;
    uj->jobb = tukormasol(gy->bal);  /* itt a kutya elasva */
    uj->bal = tukormasol(gy->jobb);  /* bal <- jobb, jobb <- bal */
    return uj;
}

/* C */
bool egyforma_e(Fa *egyik, Fa *masik) {
    if (egyik == NULL && masik == NULL) /* ket ures fa egyforma */
        return true;
    if (egyik != NULL && masik == NULL) /* ures fa es nem ures fa -> nem egyf */
        return false;
    if (egyik == NULL && masik != NULL) /* detto */
        return false;

    return egyik->szam == masik->szam                   /* egyforma szam a gyokerben */
           && egyforma_e(egyik->bal, masik->bal)        /* es egyformak a bal reszfak */
           && egyforma_e(egyik->jobb, masik->jobb);     /* es a jobb reszfak */
}

/* D */
bool tukros_e(Fa *egyik, Fa *masik) {
    if (egyik == NULL && masik == NULL) /* ket ures fa egyforma */
        return true;
    if (egyik != NULL && masik == NULL) /* ures fa es nem ures fa -> nem egyf */
        return false;
    if (egyik == NULL && masik != NULL) /* detto */
        return false;

    return egyik->szam == masik->szam                /* egyforma szam a gyokerben */
           && tukros_e(egyik->bal, masik->jobb)      /* es bal reszfa ~ jobb reszfa */
           && tukros_e(egyik->jobb, masik->bal);     /* illetve jobb reszfa ~ bal reszfa */
}

int main(void) {
    Fa *eredeti;
    Fa *masolat;
    Fa *tukormasolat;

    /* hogy legyen min kiprobalni, nem feladat */
    eredeti = uj(5);
    eredeti->bal = uj(7);
    eredeti->bal->bal = uj(6);
    eredeti->bal->jobb = uj(8);
    eredeti->jobb = uj(9);

    /* A */
    masolat = masol(eredeti);
    /* B */
    tukormasolat = tukormasol(eredeti);

    puts("dontsd a fejed 90 fokkal balra :)");
    puts("eredeti");
    kirajzol(eredeti);
    puts("masolat");
    kirajzol(masolat);
    puts("tukros masolat");
    kirajzol(tukormasolat);

    /* C */
    printf("egyforma eredeti,masolat: %s\n", egyforma_e(eredeti, masolat) ? "egyforma" : "nem az");
    printf("egyforma eredeti,tukormasolat: %s\n", egyforma_e(eredeti, tukormasolat) ? "egyforma" : "nem az");

    /* D */
    printf("tukros eredeti,masolat: %s\n", tukros_e(eredeti, masolat) ? "tukros" : "nem az");
    printf("tukros eredeti,tukormasolat: %s\n", tukros_e(eredeti, tukormasolat) ? "tukros" : "nem az");

    /* mind keri */
    torol(eredeti);
    torol(masolat);
    torol(tukormasolat);

    return 0;
}
