#include <stdio.h>
#include <stdbool.h>

/* Koordináták tárolására szolgáló struktúra */
typedef struct {
    int x, y;
} Koord;

/* Kincsek számolása */
int kincsek_szama(char t[][10], int sorok) {
    int i, j, db = 0;                 /* Ciklusváltozók és darabszám */
    for (i = 0; i < sorok; i++)       /* Végigmegyünk a sorokon */
        for (j = 0; j < 10; j++)      /* Majd az oszlopokon */
            if (t[i][j] == '$') db++; /* Találat esetén darabszám növelése */
    return db;
}

/* Labirintus kirajzolása */
void lab_kiir(char t[][10], int sorok) {
    int i, j; /* Ciklusváltozók */
    /* Végigmegyünk a sorokon */
    for (i = 0; i < sorok; i++) {
        for (j = 0; j < 10; j++)
            printf("%c", t[i][j]); /* Kiírunk minden oszlopot */
        printf("\n"); /* Következő sor új sorba jöjjön */
    }
}

/* Játékos megkeresése */
Koord jatekos_helye(char t[][10], int sorok) {
    int i, j;
    Koord k; /* Ezt a koordinátát adjuk majd visszas */
    for (i = 0; i < sorok; i++) { /* Végigmegyünk a sorokon */
        for (j = 0; j < 10; j++) { /* És az oszlopokon */
            if (t[i][j] == '@') { /* Ha megtaláljuk */
                k.x = j; /* x kordináta az oszlop */
                k.y = i; /* y koordináta a sor */
                return k; /* Visszaadjuk */
            }
        }
    }
    /* Ha eddig nem találtuk meg, akkor egy érvénytelen (-1, -1) */
    /* koordinátát adunk vissza */
    k.x = k.y = -1;
    return k;
}

/* Irányok */
enum irany {fel, jobb, le, bal};

/* Eldönti, hogy a játékos efy adott irányba mehet-e */
bool mehet_e(char t[][10], int sorok, enum irany ir) {
    Koord k = jatekos_helye(t, 10); /* Megkeressük hogy hol van */

    /* Ezután minden irányba megnézzük, nehogy túlindexeljünk, majd */
    /* megnézzük, hogy ne fal legyen */
    if (ir == fel  && k.y > 0       && t[k.y - 1][k.x] != '#') return true;
    if (ir == le   && k.y < sorok - 1 && t[k.y + 1][k.x] != '#') return true;
    if (ir == bal  && k.x > 0       && t[k.y][k.x - 1] != '#') return true;
    if (ir == jobb && k.x < 10 - 1    && t[k.y][k.x + 1] != '#') return true;
    /* Ha nem teljesült a feltétel, akkor nem mehetünk arra */
    return false;
}

/* Lépés eredménye */
enum lepes {rossz, kijarat, kincs, ures};

/* Játékos léptetése */
enum lepes leptet(char t[][10], int sorok, enum irany ir) {
    Koord jh = jatekos_helye(t, sorok); /* Megkeressük hogy hol van */
    Koord ujhely = jh; /* Ez lesz a játékos új koordinátája */
    char felulirt;   /* Ez lesz az a karakter amit felülírunk (amire a játékos rálép) */
    /* Ha nem mehet abba az irányba, akkor egyből visszatérünk */
    /* egy érvénytelen lépéssel */
    if (!mehet_e(t, sorok, ir)) return rossz;
    /* Az iránytól függően megváltoztatjuk a játékos új helyét */
    switch(ir) {
    case fel:
        ujhely.y--;
        break;
    case le:
        ujhely.y++;
        break;
    case bal:
        ujhely.x--;
        break;
    default:
        ujhely.x++;
        break;
    }
    t[jh.y][jh.x] = ' '; /* Ahol eddig volt, ott most üres hely lesz */
    felulirt = t[ujhely.y][ujhely.x]; /* Megjegyezzük, hogy milyen karakter volt ott ahova lépett */
    t[ujhely.y][ujhely.x] = '@'; /* Most hogy megjegyeztük mi volt ott, felülírjuk */
    if (felulirt == '$') return kincs; /* Ha kincset írtunk felül akkor ez lesz az eredmény */
    /* Ha elértük a labirintus szélét, akkor az kijárat */
    else if (ujhely.x == 0 || ujhely.x == 9 || ujhely.y == 0 || ujhely.y == 9) return kijarat;

    /* Különben csak simán léptünk */
    return ures;
}

int main(void) {
    /* Labirintus megadása, 10x10 tömb */
    char lab[10][10] = {
        {'#', '#', '#', '#', '#', '#', ' ', '#', '#', '#'},
        {'#', '$', ' ', ' ', ' ', '#', ' ', ' ', ' ', '#'},
        {'#', '#', '#', '#', ' ', '#', '#', '#', ' ', '#'},
        {'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'},
        {'#', ' ', '#', '#', '#', '#', ' ', '#', '#', '#'},
        {'#', ' ', '#', ' ', ' ', ' ', ' ', '#', '$', '#'},
        {'#', ' ', '#', '#', '$', '#', ' ', '#', ' ', '#'},
        {'#', '@', ' ', '#', '#', '#', ' ', '#', ' ', '#'},
        {'#', ' ', ' ', ' ', '$', '#', ' ', ' ', ' ', '#'},
        {'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
    };

    int osszkincs, megtalalt; /* Összes és megtalált kincsek nyilvántartása */
    char c; /* Karakter a beolvasáshoz */
    enum irany ir; /* Irány tárolása */
    enum lepes eredmeny; /* Eredmény tárolása */

    osszkincs = kincsek_szama(lab, 10); /* Megnézzük hány kincs van */
    megtalalt = 0; /* Találatot kinullázzuk */

    /* Ciklus amíg... */
    do {
        /* Kiírjuk a kincsek számát és a labirintust */
        printf("Kincsek: %d/%d\n", megtalalt, osszkincs);
        lab_kiir(lab, 10);

        do { /* Addig olvasunk be karaktert, amíg érvényeset nem kapunk (w a s d) */
            c = getchar();
        } while (c != 'w' && c != 'a' && c != 's' && c != 'd');

        /* Miután megvan az érvényes karakter meghatározzuk az irányt */
        switch(c) {
        case 'w':
            ir = fel;
            break;
        case 'a':
            ir = bal;
            break;
        case 's':
            ir = le;
            break;
        default:
            ir = jobb;
            break;
        }
        /* Léptetjük a játékost és figyeljük az eredményt */
        eredmeny = leptet(lab, 10, ir);
        /* Kincs esetén a találatot növeljük */
        if (eredmeny == kincs) megtalalt++;

    } while (eredmeny != kijarat); /* ...amíg el nem érünk a kijárathoz */

    /* A végén még egyszer kiírjuk a labirintust */
    printf("Jatek vege!\n");
    lab_kiir(lab, 10);

    return 0;
}
