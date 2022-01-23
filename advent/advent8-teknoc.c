#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

SDL_Renderer *renderer;

enum { SZELES = 512, MAGAS = 512 };

/* Teknoc interfesz */
typedef struct Teknoc {
    double x, y;   /* pozicio */
    double szog;   /* szog */
    bool rajzol;   /* rajzol-e mozgas kozben */
    unsigned long szin;      /* szin */
} Teknoc;

/* inicializal egy teknocot: kepernyo kozepe, jobbra nez, nem rajzol */
void teknoc_init(Teknoc *t);
/* beallitja a ceruza szinet, rgb: voros, zold, kek komponensek 0..255 */
void teknoc_szin(Teknoc *t, Uint8 r, Uint8 g, Uint8 b);
/* leteszi a ceruzat */
void teknoc_rajzol(Teknoc *t);
/* felemeli a ceruzat */
void teknoc_nemrajzol(Teknoc *t);
/* elore lep valahany keppontot (kozben rajzolhat) */
void teknoc_elore(Teknoc *t, double r);
/* balra fordul valahany fokkal */
void teknoc_fordul(Teknoc *t, double dszog);
/* teleportal az adott kepernyokoordinatakra es iranyba */
void teknoc_teleport(Teknoc *t, double ujx, double ujy, double ujszog);


/* Foprogram */
void fraktal(Teknoc *t, int mennyi, double mekkora) {
    if (mennyi == 0) {
        teknoc_elore(t, mekkora);
    } else {
        fraktal(t, mennyi - 1, mekkora);
        teknoc_fordul(t, 60);
        fraktal(t, mennyi - 1, mekkora);
        teknoc_fordul(t, -120);
        fraktal(t, mennyi - 1, mekkora);
        teknoc_fordul(t, 60);
        fraktal(t, mennyi - 1, mekkora);
    }
}

void sokszog(Teknoc *t, int csucsok, double oldalhossz) {
    for (int i = 0; i < csucsok; ++i) {
        teknoc_elore(t, oldalhossz);
        teknoc_fordul(t, 360.0 / csucsok);
    }
}

int main(int argc, char *argv[]) {
    Teknoc t;

    /* SDL inicializálása és ablak megnyitása */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Teknocgrafika", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SZELES, MAGAS, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    /* rajzolás */
    teknoc_init(&t);
    teknoc_rajzol(&t);

    /* spiral */
    teknoc_teleport(&t, 128, 128, 0);
    for (int i = 0; i < 128; ++i) {
        teknoc_szin(&t, i * 2, i * 2, 255);
        teknoc_elore(&t, i * 1.5);
        teknoc_fordul(&t, 89);
    }

    /* fraktalok */
    teknoc_szin(&t, 255, 0, 0);
    teknoc_teleport(&t, 10, MAGAS - 160, 0);
    fraktal(&t, 1, 80);
    teknoc_teleport(&t, 10, MAGAS - 90, 0);
    fraktal(&t, 2, 26.8);
    teknoc_teleport(&t, 10, MAGAS - 20, 0);
    fraktal(&t, 3, 9);

    /* hopehely */
    teknoc_szin(&t, 0, 0, 0);
    teknoc_teleport(&t, SZELES * 2 / 3, MAGAS / 2, 0);
    for (int i = 0; i < 6; ++i) {
        fraktal(&t, 2, 10);
        teknoc_fordul(&t, 60);
    }

    /* sokszogekbol */
    teknoc_szin(&t, 0, 128, 0);
    teknoc_teleport(&t, SZELES * 3 / 4, SZELES * 3 / 4, 0);
    for (int i = 0; i < 7; ++i) {
        sokszog(&t, 4, 60);
        teknoc_fordul(&t, 360.0 / 7);
    }
    SDL_RenderPresent(renderer);

    SDL_Event ev;
    while (SDL_WaitEvent(&ev) && ev.type != SDL_QUIT)
        ; /* üres */
    SDL_Quit();

    return 0;
}


/* Teknoc implementacio */
void teknoc_init(Teknoc *t) {
    t->x = SZELES / 2;
    t->y = MAGAS / 2;
    t->szog = 0;
    t->rajzol = 0;
    t->szin = 0x000000FF;
}

void teknoc_szin(Teknoc *t, Uint8 r, Uint8 g, Uint8 b) {
    t->szin = r << 24 | g << 16 | b << 8 | 255;
}

void teknoc_rajzol(Teknoc *t) {
    t->rajzol = true;
}

void teknoc_nemrajzol(Teknoc *t) {
    t->rajzol = false;
}

void teknoc_teleport(Teknoc *t, double ujx, double ujy, double ujszog) {
    t->x = ujx;
    t->y = ujy;
    t->szog = ujszog;
}

void teknoc_elore(Teknoc *t, double r) {
    double const deg2rad = 3.14159265358979323846 / 180.0;
    double ujx = t->x + r * cos(t->szog * deg2rad),
           ujy = t->y - r * sin(t->szog * deg2rad);
    if (t->rajzol)
        aalineColor(renderer, t->x, t->y, ujx, ujy, t->szin);
    t->x = ujx;
    t->y = ujy;
}

void teknoc_fordul(Teknoc *t, double dszog) {
    t->szog += dszog;
}
