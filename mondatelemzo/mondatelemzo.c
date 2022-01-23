#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * A mondat szavainak tárolására szolgáló struktúra.
 */
typedef struct SzoLista {
    char *szo; /**< Az aktuális szó. */
    struct SzoLista *kov; /**< A következő szó struktúrájának címe. */
} SzoLista;

char *melleknevek[] = {"piros", "lassú", "álmos", "vidám", "hasznos", NULL}; /**< A melléknevek tömbje. */
char *fonevek[] = {"macska", "tanár", "kutya", "hallgató", NULL}; /**< A főnevek tömbje. */
char *hatarozok[] = {"gyorsan", "lassan", "boldogan", NULL}; /**< A határozószók tömbje. */
char *igek[] = {"alszik", "iszik", "tanul", NULL}; /**< Az igék tömbje. */
char *nevelok[] = {"a", "az", NULL}; /**< A névelők tömbje. */

/**
 * Az lehetséges mondatrészeket reprezentáló felsorolt típus.
 */
typedef enum mondatresz {
    alany,      /**< alany */
    allitmany,  /**< állítmány */
    jelzo,      /**< jelző */
    hatarozo,   /**< határozó */
    nevelo      /**< névelő */
} mondatresz;

char **szotar[] = {fonevek, igek, melleknevek, hatarozok, nevelok}; /**< A szavak tömbjeit összefoglaló tömb, amit #mondatresz értékkel indexelünk.  */

/**
 * Megmondja egy szóról, hogy benne van-e a megadott tömbben és ha igen, akkor
 * a talalatba betölti a szó tömbbeli címét.
 * @param szo a keresett szó
 * @param szavak  a szavak tömbje, ahol keresünk
 * @param talalat a megtalalt szó címét írja bele a függvény
 * @return IGAZ, ha volt találat
 */
bool benne_van(char *szo, char **szavak, char **talalat) {
    unsigned i;
    for (i = 0; szavak[i] != NULL; ++i) {
        if (0 == strcmp(szo, szavak[i])) {
            *talalat = szavak[i];
            return true;
        }
    }

    return false;
}

/**
 * Megnézi, hogy az aktuális szó a listában a megadott mondatrész-e és ha igen, akkor a szo változóba betölti a
 * a megtalált szó tömbbeli címét.
 * @param aktualis a mondatot tároló láncolt listában az aktuális szó címe
 * @param a_mondatresz a keresett mondatrész típusa
 * @param szo az értelmezett szó címét írja bele a függvény
 * @return IGAZ, ha volt találat
 */
bool adott_mondatresz(SzoLista **aktualis, mondatresz a_mondatresz, char **szo) {
    char *talalat;

    if (*aktualis == NULL) return false;

    if (benne_van((*aktualis)->szo, szotar[a_mondatresz], &talalat)) {
        *aktualis = (*aktualis)->kov;
        *szo = talalat;
        return true;
    }

    return false;
}

/**
 * Megnézi, hogy az aktuális mondat tőmondat-e. Ha igen, akkor kiírja mondatrészeket és lépteti a lista mutatót.
 * @param aktualis a mondatot tároló láncolt listában az aktuális szó címe
 * @return IGAZ, ha volt találat
 */
bool tomondat(SzoLista **aktualis) {
    char *a_nevelo, *az_alany, *az_allitmany;
    SzoLista *eredeti = *aktualis;

    if (   adott_mondatresz(aktualis, nevelo, &a_nevelo)
            && adott_mondatresz(aktualis, alany, &az_alany)
            && adott_mondatresz(aktualis, allitmany, &az_allitmany)
       ) {
        printf("Tőmondat\n\tAlany: %s\n\tÁllítmány: %s\n", az_alany, az_allitmany);
        return true;
    }
    else {
        *aktualis = eredeti;
        return false;
    }
}

/**
 * Megnézi, hogy az aktuális mondat összetett mondat-e. Ha igen, akkor kiírja mondatrészeket és lépteti a lista mutatót.
 * @param aktualis a mondatot tároló láncolt listában az aktuális szó címe
 * @return IGAZ, ha volt találat
 */
bool osszetett_mondat(SzoLista **aktualis) {
    char *a_nevelo, *a_jelzo, *az_alany, *a_hatarozo, *az_allitmany;
    SzoLista *eredeti = *aktualis;

    if (   adott_mondatresz(aktualis, nevelo, &a_nevelo)
            && adott_mondatresz(aktualis, jelzo, &a_jelzo)
            && adott_mondatresz(aktualis, alany, &az_alany)
            && adott_mondatresz(aktualis, hatarozo, &a_hatarozo)
            && adott_mondatresz(aktualis, allitmany, &az_allitmany)
       ) {
        printf("Összetett mondat\n\tJelző: %s\n\tAlany: %s\n\tHatározó: %s\n\tÁllítmány: %s\n",
               a_jelzo, az_alany, a_hatarozo, az_allitmany);
        return true;
    }
    else {
        *aktualis = eredeti;
        return false;
    }
}

/**
 * Megnézi, hogy az aktuális mondat szólás-e. Ha igen, akkor kiírja mondatrészeket és lépteti a lista mutatót.
 * @param aktualis a mondatot tároló láncolt listában az aktuális szó címe
 * @return IGAZ, ha volt találat
 */
bool szolas(SzoLista **aktualis) {
    char *a_jelzo, *az_alany, *a_hatarozo, *az_allitmany;
    SzoLista *eredeti = *aktualis;

    if (   adott_mondatresz(aktualis, jelzo, &a_jelzo)
            && adott_mondatresz(aktualis, alany, &az_alany)
            && adott_mondatresz(aktualis, hatarozo, &a_hatarozo)
            && adott_mondatresz(aktualis, allitmany, &az_allitmany)
       ) {
        printf("Szólás:\n\tJelző: %s\n\tAlany: %s\n\tHatározó: %s\n\tÁllítmány: %s\n",
               a_jelzo, az_alany, a_hatarozo, az_allitmany);
        return true;
    }
    else {
        *aktualis = eredeti;
        return false;
    }
}


/**
 * Megnézi, hogy az aktuális mondat a nyelvtan egy helyes mondata-e.
 * @param aktualis a mondatot tároló láncolt listában az aktuális szó címe
 * @return IGAZ, ha volt találat
 */
bool mondat(SzoLista **aktualis) {
    return tomondat(aktualis) || osszetett_mondat(aktualis) || szolas(aktualis);
}

/**
 * Behelyez egy szót a szólistába.
 * @param aktualis a mondatot tároló láncolt lista címe
 */
void szo_berak(SzoLista **aktualis, char const *szo) {
    SzoLista *uj, *mozgo = *aktualis;

    uj = (SzoLista *)malloc(sizeof(SzoLista));
    uj->szo = (char *)malloc(strlen(szo) + 1);

    strcpy(uj->szo, szo);
    uj->kov = NULL;

    if (*aktualis == NULL) {
        *aktualis = uj;
    }
    else {
        while (mozgo->kov != NULL) mozgo = mozgo->kov;
        mozgo->kov = uj;
    }
}

/**
 * Felszabadít egy listát.
 * @param lista A felszabadítandó lista.
 */
void felszabadit(SzoLista *lista) {
    while (lista != NULL) {
        SzoLista *temp = lista->kov;
        free(lista->szo);
        free(lista);
        lista = temp;
    }
}

/**
 * A főprogram.
 */
int main(void) {
    SzoLista *szoveg = NULL;
    char szo[50];

    printf("Írj be csupa kis betűkkel egy tomondatot, osszetett mondatot vagy szolast és a végére tegyél pontot!\n? ");
    bool vege = false;
    while (!vege && scanf("%s", szo) == 1) {
        char *pont = strchr(szo, '.');
        if (pont != NULL)
            *pont = '\0';
        szo_berak(&szoveg, szo);
        if (pont != NULL)
            vege = true;
    }

    if (!mondat(&szoveg)) {
        printf("Nem értem.\n");
    }

    return 0;
}
