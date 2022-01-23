/* LABIRINTUS GENERALO ELLER ALGORITMUSAVAL - STDOUT VERZIO */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


enum Cella { JARAT = ' ', FAL = 'X' };


/* HALMAZOK KEZELESE ****************************************************************************/
/* megszamolja, hogy egy adott halmaz (mi) hany elemu */
int halmaz_hany_elemu(int *hovatartozik, int szel, int mi) {
    int db = 0;
    for (int x = 0; x < szel; x++)
        if (hovatartozik[x] == mi)
            db++;
    return db;
}


/* ad egy uj indexet, ami jo lesz egy ures halmazhoz */
int ures_halmaz_index() {
    static int ujindex = 1;
    return ujindex++;
}


/* halmazok uniojat kepzi (mit es mivel a ket halmaz) */
void halmazok_unioja(int *hovatartozik, int szel, int mit, int mivel) {
    for (int x = 0; x < szel; x++)
        if (hovatartozik[x] == mivel)
            hovatartozik[x] = mit;
}


/* LABIRINTUS GENERALO ***************************************************************************/
void maze(int szel, int mag) {
    int hovatartozik[szel];         /* ez tarolja, hogy melyik terem hova tartozik */

    /* eloszor minden terem kulon van. */
    for (int x = 0; x < szel; x++)
        hovatartozik[x] = ures_halmaz_index();

    /* felso sor */
    for (int x = 0; x < szel * 2 + 1; x++)
        putchar(FAL);
    putchar('\n');

    /* gyartjuk a labirintust soronkent */
    for (int y = 0; y < mag - 1; y++) {
        /* EZ ITT EGY OLYAN SOR, AMELYIK TERMEKET TARTALMAZ,
         * vagyis itt a vizszintes osszekottetesek vannak */
        putchar(FAL);   /* bal szelso fal */
        for (int x = 0; x < szel - 1; x++) {
            /* ez a terem */
            putchar(JARAT);

            /* es most jon az osszekottetes. */
            /* csak akkor szabad osszekotni oket, ha nincsenek egy halmazban */
            if (hovatartozik[x] != hovatartozik[x + 1]) {
                /* veletlenszeruen eldontjuk, osszekotjuk-e */
                if (rand() % 2 == 0) {
                    /* osszekotodtek - kozos halmaz */
                    halmazok_unioja(hovatartozik, szel, hovatartozik[x], hovatartozik[x + 1]);
                    putchar(JARAT);
                } else {
                    /* ha nem kotottuk ossze, akkor itt fal van */
                    putchar(FAL);
                }
            } else {
                /* mert egyebkent nem szabad osszekotni, szal ide fixen fal */
                putchar(FAL);
            }
        }
        /* az utolso terem, fal, es a termeket tartalmazo sor vege */
        putchar(JARAT);
        putchar(FAL);   /* jobb szelso fal */
        putchar('\n');

        /* EZ MEG EGY TERMEKET NEM TARTALMAZO SOR */
        putchar(FAL);           /* bal oldali fal */
        /* fuggoleges osszekottetesek */
        for (int x = 0; x < szel; x++) {
            /* ha egy elemu, muszaj bekotni */
            if (halmaz_hany_elemu(hovatartozik, szel, hovatartozik[x]) == 1)
                putchar(JARAT);
            else {
                /* amugy eldonthetjuk, hogy bekotjuk-e. */
                if (rand() % 2 == 0)
                    putchar(JARAT);
                else {
                    putchar(FAL);
                    /* ha nem kotottuk be, akkor itt egy uj halmaz keletkezik */
                    hovatartozik[x] = ures_halmaz_index(hovatartozik, szel);
                }
            }

            /* ez a termek kozott atlosan elhelyezkedo fal, ez fix */
            putchar(FAL);
        }
        /* jobb szelso fal nem kell, mert az elobb a ciklus vegen
         * kirajzolodott mar. szoval sor vege. */
        putchar('\n');
    }

    /* az utolso sor: minden kulonallot egyesiteni kell */
    putchar(FAL);   /* bal oldali fal */
    for (int x = 0; x < szel - 1; x++) {
        putchar(JARAT);     /* a terem */
        if (hovatartozik[x] != hovatartozik[x + 1]) {
            putchar(JARAT);
            halmazok_unioja(hovatartozik, szel, hovatartozik[x], hovatartozik[x + 1]);
        } else
            putchar(FAL);
    }
    putchar(JARAT); /* utolso terem */
    putchar(FAL);   /* jobb oldali fal */
    putchar('\n');

    /* also sor */
    for (int x = 0; x < szel * 2 + 1; x++)
        putchar(FAL);
    putchar('\n');
}


/* foprogram *****************************************/
int main(void) {
    int szel, mag;

    srand(time(0));

    printf("Hany terem legyen szelteben? ");
    scanf("%d", &szel);
    printf("Hany terem legyen magassagban? ");
    scanf("%d", &mag);

    maze(szel, mag);

    return 0;
}
