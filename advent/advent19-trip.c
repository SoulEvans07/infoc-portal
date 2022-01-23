#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <time.h>

const int MERET = 32;
const int ABLAK_SZ = 800, ABLAK_M = 600;

SDL_Renderer *renderer;
SDL_Window *window;
const Uint8 *keystate;

typedef struct Pont {
    double x, y, z;     /* eredeti 3d koordináta */
    double xf, yf, zf;  /* forgatott 3d koordináta */
    int xk, yk;         /* képernyőn koordináta */
} Pont;

typedef struct Negyszog {
    Pont *sarkok[4];
    Uint32 szin;
} Negyszog;

void sdl_init() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    window = SDL_CreateWindow("Trip", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ABLAK_SZ, ABLAK_M, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    keystate = SDL_GetKeyboardState(NULL);
}

int negyszogrendez(void const *vn1, void const *vn2) {
    Negyszog const *n1 = (Negyszog const *) vn1;
    Negyszog const *n2 = (Negyszog const *) vn2;
    if (n1->sarkok[0]->zf < n2->sarkok[0]->zf)
        return +1;
    if (n1->sarkok[0]->zf > n2->sarkok[0]->zf)
        return -1;
    return 0;
}


void negyszog_rajzol(Negyszog n) {
    Sint16 x[4] = { n.sarkok[0]->xk, n.sarkok[1]->xk, n.sarkok[2]->xk, n.sarkok[3]->xk },
                  y[4] = { n.sarkok[0]->yk, n.sarkok[1]->yk, n.sarkok[2]->yk, n.sarkok[3]->yk };
    filledPolygonColor(renderer, x, y, 4, n.szin);
    aapolygonColor(renderer, x, y, 4, 0x00000080);
}


static Uint32 idozit(Uint32 ms, void* param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}

void hullamzas(Pont *pontok, double t) {
    for (int y = 0; y < MERET; ++y) {
        for (int x = 0; x < MERET; ++x) {
            int i = y * MERET + x;
            pontok[i].y = 64 * sin(t + x / 5.0) * cos(t + y / 5.0);
        }
    }
}

void forgatas(Pont *pontok, double fx, double fy) {
    for (int i = 0; i < MERET * MERET; ++i) {
        Pont p1 = pontok[i];
        Pont p2 = { p1.x * cos(fy) - p1.z * sin(fy), p1.y, p1.x * sin(fy) + p1.z * cos(fy) };
        Pont p3 = { p2.x, p2.y * cos(fx) - p2.z * sin(fx), p2.y * sin(fx) + p2.z * cos(fx) };
        pontok[i].xf = p3.x;
        pontok[i].yf = p3.y;
        pontok[i].zf = p3.z;
        pontok[i].xk = ABLAK_SZ / 2 + 600 * pontok[i].xf / (pontok[i].zf + 600);
        pontok[i].yk = ABLAK_M / 2 - 600 * pontok[i].yf / (pontok[i].zf + 600);
    }
}

void rajzolas(Negyszog *negyszogek) {
    qsort(negyszogek, (MERET - 1) * (MERET - 1), sizeof(negyszogek[0]), negyszogrendez);
    for (int i = 0; i < (MERET - 1) * (MERET - 1); ++i)
        negyszog_rajzol(negyszogek[i]);
    SDL_RenderPresent(renderer);
}


int main(int argc, char *argv[]) {
    sdl_init();

    srand(time(NULL));
    Pont *pontok = (Pont *) malloc(sizeof(Pont) * MERET * MERET);
    Negyszog *negyszogek = (Negyszog *) malloc(sizeof(Pont) * (MERET - 1) * (MERET - 1));

    for (int y = 0; y < MERET; ++y)
        for (int x = 0; x < MERET; ++x) {
            int i = y * MERET + x;
            pontok[i].x = 500 * (x / (double)MERET - 0.5);
            pontok[i].z = 500 * (y / (double)MERET - 0.5);
        }
    for (int y = 0; y < MERET - 1; ++y)
        for (int x = 0; x < MERET - 1; ++x) {
            int i = y * (MERET - 1) + x;
            negyszogek[i].sarkok[0] = pontok + y * MERET + x;
            negyszogek[i].sarkok[1] = pontok + y * MERET + x + 1;
            negyszogek[i].sarkok[2] = pontok + (y + 1) * MERET + x + 1;
            negyszogek[i].sarkok[3] = pontok + (y + 1) * MERET + x;
            negyszogek[i].szin = (rand() % 65536 << 16) | rand() % 65536;
        }


    SDL_TimerID id = SDL_AddTimer(40, idozit, NULL);
    SDL_Event ev;
    double t = 0;
    double fx = -0.5, fy = -0.1;
    while (SDL_WaitEvent(&ev) && (ev.type != SDL_QUIT)) {
        if (ev.type == SDL_USEREVENT) {
            boxRGBA(renderer, 0, 0, ABLAK_SZ, ABLAK_M, 0, 0, 0, 255);

            t += 0.02;
            hullamzas(pontok, t);
            forgatas(pontok, fx, fy);
            rajzolas(negyszogek);

            /* gombnyomások? */
            if (keystate[SDL_SCANCODE_UP])
                fx += 0.02;
            if (keystate[SDL_SCANCODE_DOWN])
                fx -= 0.02;
            if (keystate[SDL_SCANCODE_LEFT])
                fy -= 0.02;
            if (keystate[SDL_SCANCODE_RIGHT])
                fy += 0.02;
        }
    }
    SDL_RemoveTimer(id);

    SDL_Quit();
    free(pontok);
    free(negyszogek);

    return 0;
}
