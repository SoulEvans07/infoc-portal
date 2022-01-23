/*
 * Az allapotgepeket nem csak egymasba agyazott switch()
 * szerkezetekkel lehet lekodolni. Sot nagyobb tablazatokat
 * kifejezetten nem erdemes ugy.
 * 
 * Ehelyett kihasznalhatjuk azt, hogy az allapotatmeneti tabla
 * es a tevekenysegtabla mind ketdimenziosak - az egyes
 * allapotatmeneteket es tevekenysegeket igy eltarolhatjuk egy
 * ketdimenzios tombben.
 * 
 * A tomb minden cellaja egy uj allapotot es egy tevekenyseget
 * tarol (igy a tomb strukturakbol kell felepuljon). A
 * tevekenyseget az ly szamlalos programnal eltarolhatjuk olyan
 * formaban, hogy beleirjuk a tombbe, mennyivel kell epp a
 * szamlalot novelni. (Bonyolultabb esetekben un.
 * fuggvenypointereket hasznalhatnank, de errol egy kesobbi
 * eloadasban lesz szo.)
 * 
 * A tablazatbol torteno kiolvasasnak a feltetele az, hogy mind
 * az allapotokhoz, mind az esemenyekhez (karakterekhez) egy
 * egesz szamot rendeljunk, amely a tablazatnak a sor- es
 * oszlopszama lesz. Az allapotoknal a hozzarendeles
 * automatikusan megtortenik (hiszen a felsorolt tipusu ertekek
 * egesz szamma alakulnak), a karaktereknel nekunk kell ezt
 * elvegezni.
 */

#include <stdio.h>
#include <stdbool.h>

/* az allapotok, es lekepezesuk 0..2 egesz szamra. */
typedef enum LyAllapot {
    alap = 0,
    l_volt = 1,
    ll_volt = 2,
} LyAllapot;

/* a beolvasott karakter lekepezese 0..2 egesz szamra. */
int karakterosztaly(char c) {
    switch (c) {
        case 'l': return 0;
        case 'y': return 1;
        default:  return 2;
    }
}

/* allapotatmenet es tevekenyseg a tablazat egy cellajaban. */
typedef struct TablaCella {
    LyAllapot kovetkezo;
    int tevekenyseg;
} TablaCella;

/* a 3x3-os allapotatmeneti es tevekenysegtabla. egy az
 * egyben ugyanaz, mint papiron! */
TablaCella allapotgep[3][3] = {
                    /* l */         /* y */      /* egyéb */
    /* alap    */ { { l_volt, 0 },  { alap, 0 }, { alap, 0 } }, 
    /* l_volt  */ { { ll_volt, 0 }, { alap, 1 }, { alap, 0 } }, 
    /* ll_volt */ { { ll_volt, 0 }, { alap, 2 }, { alap, 0 } }, 
};


int main(void) {
    /* az allapotgep kodja pedig ennyi. mindent a tablazatbol olvas! */
    LyAllapot all = alap;
    int szaml = 0;
    int c;
    while ((c = getchar()) != EOF) {
        int kar = karakterosztaly(c);
     
        /* tevekenyseg */
        szaml += allapotgep[all][kar].tevekenyseg;
        /* allapotatmenet */
        all = allapotgep[all][kar].kovetkezo;
    }
    printf("%d darab volt.\n", szaml);
    
    return 0;
}
