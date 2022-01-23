#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

typedef struct SzoLista {
    char *szo;
    struct SzoLista *kov;
} SzoLista;

/* A két kapott listát összefűzi, és visszaadja az így kapottat.
 * Nem történik új listaelem foglalás. */
SzoLista *osszefuz(SzoLista *egyik, SzoLista *masik) {
    if (egyik == NULL)
        return masik;

    SzoLista *futo = egyik;
    while (futo->kov != NULL)
        futo = futo->kov;
    futo->kov = masik;
    return egyik;
}

/* Egyelemű lista építése egy sztringből. */
SzoLista *ujegyszavas(char *szo) {
    SzoLista *uj = (SzoLista*) malloc(sizeof(SzoLista));
    uj->kov = NULL;
    uj->szo = (char*) malloc(sizeof(char)*(strlen(szo)+1));
    strcpy(uj->szo, szo);
    return uj;
}

/* A kapott listából kiválaszt egy szót,
 * és épít egy egyelemű listát, amelyikben az van. */
SzoLista *veletlenszo(char **szavak) {
    int db = 0;
    while (szavak[db] != NULL)
        ++db;
    if (db == 0)
        return NULL;

    int melyik = rand()%db;
    return ujegyszavas(szavak[melyik]);
}

/* Lista felszabadítása. */
void felszabadit(SzoLista *lista) {
    while (lista != NULL) {
        SzoLista *temp = lista->kov;
        free(lista->szo);
        free(lista);
        lista = temp;
    }
}

/* Kiír egy mondatot, végén ponttal. */
void kiir(SzoLista *mondat) {
    if (mondat == NULL) {
        printf("(üres mondat)\n");
        return;
    }

    SzoLista *iter = mondat;
    while (iter->kov != NULL) {     /* az utolso előttiig */
        printf("%s ", iter->szo);
        iter = iter->kov;
    }
    printf("%s.\n", iter->szo);   /* az utolsó */
}

/* Megmondja egy betűről, hogy magánhangzó-e. A névelők miatt kell. */
bool maganhangzo(char c) {
    return c=='a' || c=='e' || c=='i' || c=='o' || c=='u';
}

int main(void) {
    char *fonevek[] = {"macska", "tanar", "kutya", "parduc", "csirke", "hallgato", NULL};
    char *igek[] = {"fut", "alszik", "iszik", "eszik", "tanul", NULL};
    char *melleknevek[] = {"piros", "fekete", "lassu", "almos", NULL};
    char *hatarozok[] = {"gyorsan", "jol", "boldogan", NULL};

    SzoLista *mondat, *alanyi_resz, *allitmanyi_resz;

    srand(time(0));

    /* tomondatot ir */
    printf("Tomondat:\n  ");
    mondat = osszefuz(veletlenszo(fonevek), veletlenszo(igek));
    if (maganhangzo(mondat->szo[0]))
        mondat = osszefuz(ujegyszavas("az"), mondat);
    else
        mondat = osszefuz(ujegyszavas("a"), mondat);
    kiir(mondat);
    felszabadit(mondat);

    /* osszerak egy mondatot es kiirja */
    printf("\nBovitett mondat:\n  ");
    allitmanyi_resz = osszefuz(veletlenszo(hatarozok), veletlenszo(igek));
    alanyi_resz = osszefuz(veletlenszo(melleknevek), veletlenszo(fonevek));
    mondat = osszefuz(alanyi_resz, allitmanyi_resz);
    mondat = osszefuz(ujegyszavas(maganhangzo(mondat->szo[0]) ? "az":"a"), mondat);
    kiir(mondat);
    felszabadit(mondat);

    /* szolast ir ki */
    printf("\nSzolas:\n  ");
    allitmanyi_resz = osszefuz(veletlenszo(hatarozok), veletlenszo(igek));
    alanyi_resz = osszefuz(veletlenszo(melleknevek), veletlenszo(fonevek));
    mondat = osszefuz(alanyi_resz, allitmanyi_resz);
    kiir(mondat);
    felszabadit(mondat);

    return 0;
}
