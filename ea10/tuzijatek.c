/* Az alábbi SDL-es program tűzijátékot rajzol ki.
 *
 * Az előadáson magyarázott függvények részletes implementációit
 * tartalmazza. Többféle robbanó elemet is ismer; a "robbanó"
 * típusú véletlenszerű irányokba esik szét, míg a "virág" típusú
 * kör alakban robban, és a darabjai még tovább robbannak egy
 * kicsit később.
 *
 * A láncolt lista kezelése mindenben megegyezik azzal, ahogy
 * az előadáson bemutatásra került. Egyszeresen láncolt, elején
 * strázsás listát alkalmaz:
 *  - az első strázsa azért van, hogy a lista eleje mutató ne
 *    változzon.
 *  - mivel lényegében mindegy, hogy az új elemek a listába hova
 *    kerülnek (nem lesz szemmel látható különbség), ezért mindig
 *    az aktuálisan feldolgozott után kerülnek.
 *  - a törlés miatt viszont kell lemaradó pointer is, továbbá
 *    külön figyelni kell a ciklusváltozóra.
 * A nem triviális listaműveletekhez tartozó rajzok az előadás-
 * anyagban találhatóak.
 */

#include <stdlib.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

//#define ELOADAS 1

/* tipikus globalis valtozo: mindenki latja, mindenkinek tudnia kell a meretet */
SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font;

enum Tipus { robbano, virag, virag_darab, eltuno };
typedef struct Pont {
    enum Tipus tipus;
    double x, y;        /* koordináták */
    double vx, vy;      /* sebesség */
    unsigned char r, g, b;
    double elettartam;  /* ennyi idő múlva robban v. eltűnik */

    struct Pont *kov;    /* listában */
} Pont;


/* általánosságban */
Pont *uj_pont(double x, double y, double vx, double vy) {
    Pont *uj = (Pont *) malloc(sizeof(Pont));
    uj->x = x;
    uj->y = y;
    uj->vx = vx;
    uj->vy = vy;
    uj->kov = NULL;
    /* ez a trükkös rész az rgb (vörös, zöld, kék) komponensek közül sötétít maximum 2-t. */
    /* a trükk az, hogy a generált véletlenszámok a 0...6 tartományból vannak; nem a 0...7 */
    /* tartományból. tehát a 3 biten tárolható számok közül kimarad az az egy, amelyikben */
    /* mind a 3 bit 1-es. */
    int torol = rand()%7;
    uj->r = torol&1 ? 128 : rand()%32+224;
    uj->g = torol&2 ? 128 : rand()%32+224;
    uj->b = torol&4 ? 128 : rand()%32+224;
    return uj;
}


/* szokásos tűzijáték */
Pont *uj_robbano(void) {
    int w, h;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    Pont *uj = uj_pont(rand()%(w/2) + w/4, h, rand()%40-20, -(rand()%40+80));
    uj->elettartam = rand()%5+6;
    uj->tipus = robbano;
    return uj;
}


/* virág alakúvá robbanó fajta */
Pont *uj_virag(void) {
    Pont *uj = uj_robbano();
    uj->tipus = virag;
    return uj;
}


/* ez is tulajdonképp egy robbanó fajta, de mások a paraméterei */
Pont *uj_virag_darab(double x, double y, double vx, double vy, unsigned char r, unsigned char g, unsigned char b) {
    Pont *uj = uj_pont(x, y, vx, vy);
    uj->elettartam = 1.5+rand()%10/30.0;
    uj->tipus = virag_darab;
    uj->r = r;
    uj->g = g;
    uj->b = b;
    return uj;
}


/* végleg eltűnő */
Pont *uj_eltuno(double x, double y, unsigned char r, unsigned char g, unsigned char b) {
    Pont *uj = uj_pont(x, y, rand()%60-30, rand()%60-30);
    uj->elettartam = 3+rand()%10/10.0;
    uj->tipus = eltuno;
    uj->r = r;
    uj->g = g;
    uj->b = b;
    return uj;
}


/* beszúrja a "mit" elemet a "miután" elemet követően. */
void beszur(Pont *miutan, Pont *mit) {
    mit->kov = miutan->kov;
    miutan->kov = mit;
}


/* felszabadítja a listát */
void felszabadit(Pont *lista) {
    while (lista != NULL) {
        Pont *kov = lista->kov;
        free(lista);
        lista = kov;
    }
}


/* az adott pontlistát "szimulálja", delta_t időlépéssel */
void szimulal(Pont *lista, double delta_t) {
    const double g = 9.81;

    /* minden pont: mozgás, robbanás */
    Pont *lemarado = lista;
    Pont *iter = lista->kov;
    Pont *kovetkezo;
    while (iter != NULL) {
        iter->elettartam -= delta_t;
        if (iter->elettartam < 0) {
            switch (iter->tipus) {
                case robbano:
                    for (int i = 0; i < 30; i++)
                        beszur(iter, uj_eltuno(iter->x, iter->y, iter->r, iter->g, iter->b));
                    break;
                case virag:
                    for (int i = 0; i < 10; i++) {
                        /* a körön körbe; szög=i/max*2pi */
                        double sz = i/10.0*2*3.141592;
                        double vx = cos(sz)*(rand()%10+80), vy=sin(sz)*(rand()%10+80);
                        beszur(iter, uj_virag_darab(iter->x, iter->y, vx, vy, iter->r, iter->g, iter->b));
                    }
                    break;
                case virag_darab:
                    for (int i = 0; i < 20; i++)
                        beszur(iter, uj_eltuno(iter->x, iter->y, iter->r, iter->g, iter->b));
                    break;
                case eltuno:
                    break;
            }

            lemarado->kov = iter->kov;  /* törlendőt kihagy kihagy */
            kovetkezo = iter->kov;      /* ez a ciklushoz kell */
            free(iter);                 /* mivel az itert felszabadítjuk! */
        } else {
            iter->x += iter->vx*delta_t;
            iter->y += iter->vy*delta_t;
            iter->vy += g*delta_t;
            /* kis halvanyodás */
            if (iter->tipus == eltuno) {
                if (iter->r > 4) iter->r -= 4;
                if (iter->g > 4) iter->g -= 4;
                if (iter->b > 4) iter->b -= 4;
            }
            lemarado = iter;
            kovetkezo = iter->kov;
        }

        iter = kovetkezo;
    }
}


void kirajzol(Pont *lista) {
    /* kirajzol */
    int w, h;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    boxRGBA(renderer, 0, 0, w-1, h-1, 0, 0, 0, 255);
    for (Pont *iter = lista->kov; iter != NULL; iter = iter->kov) {
        filledCircleRGBA(renderer, iter->x, iter->y, 3, iter->r, iter->g, iter->b, 255);
        aacircleRGBA(renderer, iter->x, iter->y, 3, iter->r, iter->g, iter->b, 255);
    }
#if ELOADAS
    /* ez a rész kiírja, épp hány pont van */
    if (font) {
        char szam[20];
        int h = 0;
        for (Pont *iter = lista->kov; iter != NULL; iter = iter->kov)
            h += 1;
        sprintf(szam, "%d", h);

        SDL_Surface *felirat = TTF_RenderUTF8_Solid(font, szam, (SDL_Color){255, 255, 255});
        SDL_Texture *felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
        SDL_Rect cel = {0, 0, strlen(szam)*30, 30};
        SDL_RenderCopy(renderer, felirat_t, NULL, &cel);
        SDL_DestroyTexture(felirat_t);
        SDL_FreeSurface(felirat);
    }
#endif
    SDL_RenderPresent(renderer);
}


/* az idozito fv */
Uint32 idozit(Uint32 ms, void* param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;   /* ujabb varakozas */
}


int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
#if ELOADAS
    window = SDL_CreateWindow("Tuzijatek", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
    window = SDL_CreateWindow("Tuzijatek", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, 0);
#endif
    if (!window) {
        fprintf(stderr, "Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) {
        fprintf(stderr, "Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }

#if ELOADAS
    TTF_Init();
    font = TTF_OpenFont("../sdl/LiberationMono-Regular.ttf", 48);
    if (!font) {
        fprintf(stderr, "Nem nyithato meg a betutipus: %s", TTF_GetError());
        exit(1);
    }
#endif

    srand(time(0));

    /* strázsa */
    Pont *lista = (Pont*) malloc(sizeof(Pont));
    /* hogy rögtön legyen benne valami */
    lista->kov = uj_robbano();
    lista->kov->kov = NULL;

    SDL_TimerID id = SDL_AddTimer(20, idozit, NULL);
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        switch (event.type) {
            /* felhasznaloi esemeny: ilyeneket general az idozito fuggveny */
            case SDL_USEREVENT:
                szimulal(lista, 0.075);
                kirajzol(lista);
                beszur(lista, rand()%5 == 0 ? uj_virag() : uj_robbano());
                break;

            case SDL_QUIT:
            case SDL_KEYDOWN:
                quit = true;
                break;
        }
    }
    SDL_RemoveTimer(id);

    felszabadit(lista);
#if ELOADAS
    if (font)
        TTF_CloseFont(font);
#endif

    SDL_Quit();
    return 0;
}
