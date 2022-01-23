/* LABIRINTUS GENERALO A GROWING TREE ALGORITMUSAVAL */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <SDL.h>

/* KONFIGURACIO */
#define MERETX 35
#define MERETY 35
/* egy cella merete */
#define PIXEL 16
/* kesleltetes */
#define SLEEP 40

typedef enum Cella { Fal, Jarat, Ideiglenes } Cella;

typedef Cella Kep[MERETY][MERETX];

/* globalis valtozo: kepernyo */
SDL_Renderer *renderer;

/* PONTOKAT TAROLO TIPUS ES DOLGAI ******************************************/
typedef struct Pont {
    int x, y;   /* koordinatak */
} Pont;


enum { PONTOKTOMBMERET = MERETX * MERETY / 4 }; /* ennyi eleg kell legyen */
typedef struct Pontok {
    int db;
    Pont pontok[PONTOKTOMBMERET];
} Pontok;


void pontok_init(Pontok *p) {
    p->db = 0;
}


/* kitorli az adott indexu elemet a tombbol */
void pontok_elemet_torol(Pontok *p, int hanyadikat) {
    /* nincs is annyiadik? */
    assert(hanyadikat < p->db);

    for (int i = hanyadikat + 1; i < p->db; ++i)
        p->pontok[i - 1] = p->pontok[i];
    p->db--;
}


/* hozzafuz egy uj elemet a tombhoz */
void pontok_hozzafuz(Pontok *p, int x, int y) {
    assert(p->db < PONTOKTOMBMERET);
    p->pontok[p->db].x = x;
    p->pontok[p->db].y = y;
    p->db++;
}


/* igazzal ter vissza, ha ures */
bool pontok_ures(Pontok *p) {
    return p->db == 0;
}
/* </Pontok> ***********************************************/


/* RAJZOLAS ******************************************/
void rajzol(Kep k, int x, int y, Cella mit) {
    /* tömb a színeknek: fal, jarat, ideiglenes */
    static SDL_Color szinek[] = {
        {0xff, 0xff, 0xff, 0xff},
        {0x00, 0x00, 0x00, 0xff},
        {0x00, 0xff, 0x00, 0xff},
    };
    k[y][x] = mit;
    SDL_Rect r = { x * PIXEL, y * PIXEL, PIXEL, PIXEL };
    SDL_SetRenderDrawColor(renderer, szinek[mit].r, szinek[mit].g, szinek[mit].b, szinek[mit].a);
    SDL_RenderFillRect(renderer, &r);
}


/* ures palyat (kepet) csinal */
void ures(Kep k) {
    /* keret, mintha ott jarat lenne, es akkor nem megy oda a rajzolo */
    for (int y = 0; y < MERETY; y++) {
        rajzol(k, 0, y, Jarat);
        rajzol(k, MERETX - 1, y, Jarat);
    }
    for (int x = 0; x < MERETX; x++) {
        rajzol(k, x, 0, Jarat);
        rajzol(k, x, MERETY - 1, Jarat);
    }

    /* tobbi telibe fallal */
    for (int y = 1; y < MERETY - 1; y++)
        for (int x = 1; x < MERETX - 1; x++)
            rajzol(k, x, y, Fal);
}


/* A LABIRINTUS GENERALO *****************************************/
void labirintus_rajzol(Kep k, int kx, int ky) {
    typedef enum { balra, jobbra, fel, le } Irany;
    Irany iranyok[4] = { balra, jobbra, fel, le };
    /* iranykoordinatak; indexek megegyeznek az iranyok tombbel! */
    int const dx[4] = { -1, 1, 0, 0 };
    int const dy[4] = { 0, 0, 1, -1 };
    Pontok l;

    pontok_init(&l);

    /* a kiindulasi pontot kivajjuk */
    k[ky][kx] = Jarat;
    pontok_hozzafuz(&l, kx, ky);

    while (!pontok_ures(&l)) {
        /* kiveszunk egy elemet a listabol. ezt fogjuk vizsgalni. */
        /* itt lehet allitani, hogy elso (0), utolso (l.db-1)
         * vagy masmelyik elemet vegye ki a tombbol. */
        int veletlen_index = rand() % l.db;
        Pont p = l.pontok[veletlen_index];

        /* valasztunk egy random iranyt is */
        Irany r = iranyok[rand() % 4];
        int cx = p.x + dx[r] * 2;
        int cy = p.y + dy[r] * 2;

        /* megprobalunk menni arra. */
        if (k[cy][cx] == Fal) {
            rajzol(k, cx, cy, Ideiglenes);      /* uj terem */
            rajzol(k, p.x + dx[r], p.y + dy[r], Jarat); /* odavezeto ut*/
            pontok_hozzafuz(&l, cx, cy);
            SDL_RenderPresent(renderer);
            SDL_Delay(SLEEP);
        }
        /* itt szebb lenne, ha osszegyujtenenk a lehetseges iranyokat,
         * es utana abbol valasztanank veletlenszeruen. mert lehet, hogy
         * lehet menni valamerre, de a random pont nem azt az iranyt
         * talalja ki... osszesen csak 4 irany van, ugyhogy ez a pongyolasag
         * most belefer. */

        /* es akkor most, lehet meg innen vhova menni? */
        bool lehet_meg_valahova = false;
        for (int i = 0; i < 4; i++) {
            cx = p.x + dx[i] * 2;
            cy = p.y + dy[i] * 2;
            if (k[cy][cx] == Fal)
                lehet_meg_valahova = true;
        }
        /* ha mar nem, akkor kiszedjuk a tarolobol */
        if (!lehet_meg_valahova) {
            rajzol(k, p.x, p.y, Jarat);
            pontok_elemet_torol(&l, veletlen_index);
            SDL_RenderPresent(renderer);
            SDL_Delay(SLEEP);
        }
    }
}


/* foprogram ******************************************/
int main(int argc, char *argv[]) {
    srand(time(0));

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Window *window = SDL_CreateWindow("Labirintus", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MERETX * PIXEL, MERETY * PIXEL, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }

    Kep k;
    ures(k);
    labirintus_rajzol(k, MERETX / 4 * 2, MERETY / 4 * 2);

    SDL_Delay(5000);

    return 0;
}
