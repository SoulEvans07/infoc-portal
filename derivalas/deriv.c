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


/* egy kifejezes masolata. konstans masolata konstans, valtozo masolata masolat.
 * osszeg masolata egy osszeg - amely bal es jobb oldala az eredeti osszeg bal
 * es jobb oldalanak masolata! szoval ez is rekurziv. */
Kifejezes *masol(Kifejezes *m) {
    switch (m->tipus) {
      case Konstans:
        return uj_konstans(m->szam);
      case Valtozo:
        return uj_valtozo();
      case Osszeg:
        return uj_osszeg(masol(m->bal), masol(m->jobb));
      case Szorzat:
        return uj_szorzat(masol(m->bal), masol(m->jobb));
    }

    abort();
}


/* egyszerusites. ez is egy ujonnan mallocolt kifejezessel ter vissza,
 * amely az eredetinek vagy egyszerusitett valtozata, vagy csak a masolata
 * (ha nem lehetett egyszerusiteni).
 * a megvalositott egyszerusitesek, ahol a, b tetszoleges kifejezesek,
 * es c valamilyen konstans:
 *   c+c, a+0, 0+a,
 *   c*c, c*0, 0*c, c*1, 1*c
 * termeszetesen az egyszerusites is rekurziv. osszeg es szorzat eseten
 * eloszor egyszerusitjuk a reszkifejezeseket, es utana az egyszerusitettekbol
 * dolgozunk tovabb. */
Kifejezes *egyszerusit(Kifejezes *m) {
    Kifejezes *bale, *jobbe;

    switch (m->tipus) {
      case Konstans:
        return uj_konstans(m->szam);

      case Valtozo:
        return uj_valtozo();

      case Osszeg:
        bale = egyszerusit(m->bal);
        jobbe = egyszerusit(m->jobb);
        if (bale->tipus == Konstans && jobbe->tipus == Konstans) { /* szam+szam */
            double o = bale->szam + jobbe->szam;
            torol(bale);
            torol(jobbe);
            return uj_konstans(o);
        }
        if (bale->tipus == Konstans && bale->szam == 0) { /* 0+valami */
            torol(bale);
            return jobbe;
        }
        if (jobbe->tipus == Konstans && jobbe->szam == 0) { /* valami+0 */
            torol(jobbe);
            return bale;
        }
        return uj_osszeg(bale, jobbe);

      case Szorzat:
        bale = egyszerusit(m->bal);
        jobbe = egyszerusit(m->jobb);
        if (bale->tipus == Konstans && jobbe->tipus == Konstans) { /* szam*szam */
            double sz = bale->szam * jobbe->szam;
            torol(bale);
            torol(jobbe);
            return uj_konstans(sz);
        }
        if (bale->tipus == Konstans && bale->szam == 0) { /* 0*valami */
            torol(jobbe);
            return bale;
        }
        if (jobbe->tipus == Konstans && jobbe->szam == 0) { /* valami*0 */
            torol(bale);
            return jobbe;
        }
        if (bale->tipus == Konstans && bale->szam == 1) { /* 1*valami */
            torol(bale);
            return jobbe;
        }
        if (jobbe->tipus == Konstans && jobbe->szam == 1) { /* valami*1 */
            torol(jobbe);
            return bale;
        }
        return uj_szorzat(bale, jobbe);
    }

    abort();
}


/* derivalas. visszater egy ujonnan foglalt kifejezessel,
 * ami az eredetinek a derivaltja. a megvalositott derivalasi
 * szabalyok:
 * c   -> 0
 * x   -> 1
 * a+b -> a'+b'
 * a*b -> a'b + ab'
 */
Kifejezes *derival(Kifejezes *m) {
    switch (m->tipus) {
      case Konstans:
        return uj_konstans(0);
      case Valtozo:
        return uj_konstans(1);
      case Osszeg:
        return uj_osszeg(derival(m->bal), derival(m->jobb));   /* a'+b' */
      case Szorzat:                                            /* a'b + ab' */
        /* fontos, hogy a hivo itt arra szamit, hogy a szorzatok, amelyeket
         * ez a kifejezes tartalmaz, mind dinamikusan foglalt, egyedi kifejezeseket
         * tartalmaznak. mivel a derivalt fuggveny ilyet ad, a parja nem
         * tartalmazhatja m->jobbot, hanem csak masol(m->jobbot)! */
        return uj_osszeg(
                   uj_szorzat(derival(m->bal), masol(m->jobb)),
                   uj_szorzat(masol(m->bal), derival(m->jobb)));
    }

    abort();
}


int main(void) {
    Kifejezes *egysz, *deriv, *derivegysz;
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

    egysz = egyszerusit(fa);
    deriv = derival(fa);
    derivegysz = egyszerusit(deriv);

    printf("f(x)=");
    kiir(fa);
    printf("\n");
    printf("egysz f(x)=");
    kiir(egysz);
    printf("\n");
    /* kirajzolja a fuggvenyt. */
    for (x = -8; x <= 8; x += 1) {
        int yy = (int)(0.1 * szamol(fa, x));
        int i;
        for (i = 0; i < yy; ++i)
            printf(" ");
        printf("*\n");
    }
    printf("f'(x)=");
    kiir(deriv);
    printf("\n");
    printf("egysz f'(x)=");
    kiir(derivegysz);
    printf("\n");
    torol(fa);
    torol(deriv);
    torol(egysz);
    torol(derivegysz);

    return 0;
}
