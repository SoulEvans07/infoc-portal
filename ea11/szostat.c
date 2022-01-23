/* A program a szabványos bemenetről olvas egy szöveget,
 * és statisztikát készít, melyik szó hányszor szerepelt benne. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Bináris fa struktúra, max. 50 betűs szavak és
 * hozzájuk kapcsolt előfordulási darabszámok tárolására. */
typedef struct SzoStat {
   char szo[51];
   int db;
 
   struct SzoStat *bal, *jobb;
} SzoStat;


/* Ez a függvény egy szót (második paraméter) számol a bináris fában,
 * amelynek gyökerét paraméterként veszi át (első paraméter), és az esetleg
 * megváltozott gyökér pointerrel tér vissza.
 * Ha a szó még nem szerepel a fában, beilleszti, és 1-et ír a darabszámába.
 * Ha már benne volt, akkor növeli a darabszámot.
 *
 * Használata:
 *   gy = beszur(gy, "alma");
 */
SzoStat *beszur(SzoStat *gyoker, char *szo) {
    if (gyoker==NULL) {
        SzoStat *uj = (SzoStat*) malloc(sizeof(SzoStat));
        uj->bal = uj->jobb = NULL;
        strcpy(uj->szo, szo);
        uj->db = 1;
        return uj;     /* vissza kell térni vele! */
    }

    int er = strcmp(szo, gyoker->szo);
    if (er < 0)
        gyoker->bal = beszur(gyoker->bal, szo);
    else if (er > 0)
        gyoker->jobb = beszur(gyoker->jobb, szo);
    else
        gyoker->db++;
 
    return gyoker;
}


/* Ez a függvény kiírja a fában tárolt adatokat:
 * a szavakat és a hozzájuk tartozó darabszámokat.
 * Mivel inorder bejárást használ, a fa pedig rendezőfa,
 * ábécé rendben jelennek meg a szavak. */
void kiir(SzoStat *fa) {
    if (fa==NULL)
        return;
    kiir(fa->bal);
    printf("%s %d\n", fa->szo, fa->db);
    kiir(fa->jobb);
}


/* Felszabadítja a paraméterként kapott fát. */
void felszabadit(SzoStat *fa) {
    if (fa==NULL)
        return;
    felszabadit(fa->bal);
    felszabadit(fa->jobb);
    free(fa);
}


/* Főprogram. */
int main(void) {
    SzoStat *fa = NULL; /* üres fa */
    char szo[51];
 
    while (scanf("%s", szo) == 1)
        fa = beszur(fa, szo);
 
    kiir(fa);
    felszabadit(fa);
 
    return 0;
}
