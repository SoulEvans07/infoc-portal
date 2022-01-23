#include <stdio.h>
#include <stdlib.h>

/* a kifejezes tipusat megado felsorolt tipus */
typedef enum {
    Konstans,
    Osszeg,
    Szorzat,
    Valtozo
} KifejezesTipus;


/* egy kifejezes, ami a fentiek kozul valamelyik
 * tipusu. ha osszeg vagy szorzat, akkor gyerekei is vannak. */
typedef struct Kifejezes {
    KifejezesTipus tipus;
    double szam;                   /* csak konstanshoz */
    struct Kifejezes *bal, *jobb;    /* csak muvelethez */
} Kifejezes;


/* letrehoz egy uj szorzat kifejezest, amelyben eltarolja
 * a parameterkent kapott ket reszkifejezest. */
Kifejezes *uj_szorzat(Kifejezes *bal, Kifejezes *jobb) {
    Kifejezes *uj = (Kifejezes *) malloc(sizeof(Kifejezes));
    uj->tipus = Szorzat;
    uj->bal = bal;
    uj->jobb = jobb;
    return uj;
}


/* letrehoz egy uj osszeg kifejezest, amelyben eltarolja
 * a parameterkent kapott ket reszkifejezest. */
Kifejezes *uj_osszeg(Kifejezes *bal, Kifejezes *jobb) {
    Kifejezes *uj = (Kifejezes *) malloc(sizeof(Kifejezes));
    uj->tipus = Osszeg;
    uj->bal = bal;
    uj->jobb = jobb;
    return uj;
}


/* letrehoz egy uj konstanst, amelynek erteke a parameterkent
 * kapott szam. */
Kifejezes *uj_konstans(double szam) {
    Kifejezes *uj = (Kifejezes *) malloc(sizeof(Kifejezes));
    uj->tipus = Konstans;
    uj->szam = szam;
    return uj;
}


/* letrehoz egy uj valtozo kifejezest (x). */
Kifejezes *uj_valtozo() {
    Kifejezes *uj = (Kifejezes *) malloc(sizeof(Kifejezes));
    uj->tipus = Valtozo;
    return uj;
}


/* rekurzivan kitorli a memoriabol a teljes kifejezest */
void torol(Kifejezes *gy) {
    /* a konstansnak nincsenek gyerekei */
    /* nem koveteljuk meg, hogy azok a pointerek NULL-ok
       legyenek, hanem a muvelet tipusa alapjan dontunk */
    switch (gy->tipus) {
      case Valtozo:
      case Konstans:
        break;
      case Szorzat:
      case Osszeg:
        torol(gy->bal);
        torol(gy->jobb);
        break;
    }
    free(gy);
}


/* Kiirunk egy muveletet. Inorder bejarast kell hasznalni,
 * mivel a muveleti jel a ket operandus koze kerul. */
void kiir(Kifejezes *gy) {
    switch (gy->tipus) {
      case Konstans:    /* egy szam - kiirjuk a szamot */
        printf("%g", gy->szam);
        break;
      case Valtozo:     /* valtozo - kiirunk egy x-et */
        printf("x");
        break;
      case Osszeg:
        kiir(gy->bal); /* osszeg - tag, '+', tag */
        printf(" + ");
        kiir(gy->jobb);
        break;
      case Szorzat:     /* szorzat: ha a gyerek osszeg, akkor
                           be kell zarojelezni (a gyereket) */
        if (gy->bal->tipus == Osszeg) {
            printf("(");
            kiir(gy->bal);
            printf(")");
        }
        else
            kiir(gy->bal);
        printf("*");
        if (gy->jobb->tipus == Osszeg) {
            printf("(");
            kiir(gy->jobb);
            printf(")");
        }
        else
            kiir(gy->jobb);
        break;
    }
}


/* kiertekeljuk az adott muveletet. konstans es valtozo
 * eseten egyertelmu. osszeg es szorzat eseten rekurziv:
 * a bal es a jobb oldali reszkifejezes eredmenyenek
 * osszegevel / szorzataval kell visszaterni. */
double szamol(Kifejezes *gy, double x) {
    switch (gy->tipus) {
      case Konstans:
        return gy->szam;
      case Valtozo:
        return x;
      case Osszeg:
        return szamol(gy->bal, x) + szamol(gy->jobb, x);
      case Szorzat:
        return szamol(gy->bal, x) * szamol(gy->jobb, x);
    }

    /* ide nem lenne szabad eljutni */
    abort();
}


int main(void) {
    Kifejezes *fa;
    double x;

    /* 2*3*x*x + 3*x */
    fa = uj_osszeg(
            uj_szorzat(
                uj_szorzat(uj_konstans(2), uj_konstans(3)),
                uj_szorzat(uj_valtozo(), uj_valtozo())),
            uj_szorzat(
                uj_konstans(3),
                uj_valtozo()));

    printf("f(x)=");
    kiir(fa);
    printf("\n");
    printf("f(2)=%g\n", szamol(fa, 2));
    /* kirajzolja a fuggvenyt. */
    for (x = -8; x <= 8; x += 1) {
        int yy = (int)(0.1 * szamol(fa, x));
        int i;
        for (i = 0; i < yy; ++i)
            printf(" ");
        printf("*\n");
    }
    torol(fa);

    return 0;
}
