#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

enum { ABLAK_SZ = 480, ABLAK_M = 640 };

/* egy targy, ami lehet fa, ellenseg, golyo */
typedef enum TargyTipus { fa, ellenseg, golyo } TargyTipus;
typedef struct Targy {
    TargyTipus tipus;
    int x, y;
    bool torlendo;             /* a feldolgozo alg. hasznalja */

    struct Targy *kov;         /* lancolt lista */
} Targy;


/* betesz egy targyat a listaba, meghozza az elejere.
 * a lista elol strazsas, ezert tlista a strazsara mutat. */
void targy_listaba(Targy *tlista, Targy *t) {
    t->kov = tlista->kov;
    tlista->kov = t;
}


/* fat rajzol az adott koordinatara */
void fa_rajzol(SDL_Renderer *rdr, int x, int y) {
    filledTrigonColor(rdr, x, y-20, x-12, y+8, x+12, y+8, 0x008000FF);
    boxColor(rdr, x-2, y+9, x+2, y+13, 0x603000FF);
}


/* repulogepet rajzol. ha sajat, akkor felfele nez, ha nem, lefele */
void repgep_rajzol(SDL_Renderer *rdr, int x, int y, bool sajat) {
    int dx = rand()%5;        /* légcsavarhoz */
    int s = sajat ? 1:-1;     /* y tükrözéshez */
    Uint32 szin = sajat ? 0x000080FF : 0x400000FF; /* 0xRRGGBBAA */

    boxColor(rdr, x-3, y-20*s, x+3, y+20*s, szin);          /* test */
    boxColor(rdr, x-8, y+17*s, x+8, y+19*s, szin);          /* farok */
    boxColor(rdr, x-20, y-10*s, x+20, y-3*s, 0xFFFFFFFF);   /* szarny */
    boxColor(rdr, x-20, y-4*s, x+20, y-4*s, 0xC00000FF);    /* szarny */
    boxColor(rdr, x-dx, y-24*s, x+dx, y-23*s, 0xFFFFFFFF);  /* prop */
}


/* lovedeket rajzol */
void golyo_rajzol(SDL_Renderer *rdr, int x, int y) {
    filledCircleColor(rdr, x, y, 2, 0xFFFF00FF);
}


/* kirajzolja a jatekmezot. rdr egy SDL_Renderer, amit ez hasznal */
void palya_rajzol(SDL_Renderer *rdr, int x, int y, Targy *tlista, bool piros_e) {
    boxColor(rdr, 0, 0, ABLAK_SZ-1, ABLAK_M-1, piros_e ? 0x900000FF : 0x005000FF);
    /* ket es fel dimenzios grafika: alulra a fak, felulre a repulok */
    for (Targy *iter = tlista->kov; iter != NULL; iter = iter->kov)
        if (iter->tipus == fa)
            fa_rajzol(rdr, iter->x, iter->y);
    for (Targy *iter = tlista->kov; iter != NULL; iter = iter->kov)
        if (iter->tipus == ellenseg)
            repgep_rajzol(rdr, iter->x, iter->y, false);
    for (Targy *iter = tlista->kov; iter != NULL; iter = iter->kov)
        if (iter->tipus == golyo)
            golyo_rajzol(rdr, iter->x, iter->y);
    /* sajat (nem ellenseg) repgep */
    repgep_rajzol(rdr, x, y, true);

    SDL_RenderPresent(rdr);
}


/* mozgatja a listaban levo targyakat. rgx, rgy a jatekos koordinatai */
bool targyak_mozgat(Targy *tlista, int rgx, int rgy) {
    /* nullazzuk mindenhol */
    for (Targy *iter = tlista->kov; iter != NULL; iter = iter->kov)
        iter->torlendo = false;

    /* targyak feldolgozasa. legfokepp az y koordinatajukat lepteti.
     * ellensegnel nezi azt, hogy van-e a palyan golyo amelyik eltalalta */
    bool nekunkjott = false;
    int dx, dy;
    for (Targy *iter = tlista->kov; iter != NULL; iter = iter->kov) {
        switch (iter->tipus) {
            case fa:
                iter->y += 5;
                break;
            case ellenseg:
                iter->y += 7;
                /* eltalaltuk a golyoval? */
                for (Targy *iterg = tlista->kov; iterg != NULL; iterg = iterg->kov)
                    if (iterg->tipus == golyo) {
                        dx = iter->x - iterg->x;
                        dy = iter->y - iterg->y;
                        if (dx*dx+dy*dy < 15*15) { /* 15px pontosan kell */
                            iter->torlendo = true;
                            iterg->torlendo = true;
                        }
                    }
                /* nekunkjott?! */
                dx = iter->x - rgx;
                dy = iter->y - rgy;
                if (dx*dx+dy*dy < 40*40) {
                    nekunkjott = true;
                    iter->torlendo = true;
                }
                break;
            case golyo:
                iter->y -= 12;
                break;
        }

        /* ha kimegy a kepernyorol fent vagy lent, akkor ki lehet torolni */
        if (iter->y>ABLAK_M+20 || iter->y<0)
            iter->torlendo = true;
    }

    /* a torlesre jelolt targyakat toroljuk. itt segit a lista eleji strazsa nagyon */
    Targy *lemarado = tlista;
    Targy *iter = tlista->kov;
    while (iter != NULL) {
        if (iter->torlendo) {
            Targy *torlendo = iter;
            lemarado->kov = torlendo->kov;
            iter = torlendo->kov;       /* itt a lemarado valtozatlan! */
            free(torlendo);
        } else {
            lemarado = iter;
            iter = iter->kov;
        }
    }

    return nekunkjott;
}


static Uint32 idozit(Uint32 ms, void* param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}


void jatek(SDL_Renderer *renderer) {
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    SDL_TimerID id = SDL_AddTimer(40, idozit, NULL);

    /* ures lista: az elejen egy strazsa */
    Targy *t = (Targy*) malloc(sizeof(Targy));
    t->kov = NULL;

    int rgx = ABLAK_SZ/2, rgy = ABLAK_M*5/6;
    int kovetkezoellenseg = 0;   /* ennyi lepes mulva jon a kovetkezo szembe */
    int piros = 0;               /* piros hatter utkozesnel */

    bool kilep = false;
    while (!kilep) {
        SDL_Event ev;
        SDL_WaitEvent(&ev);
        switch (ev.type) {
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) {
                    case SDLK_SPACE:
                        /* space: uj golyo kilovese */
                        {
                            Targy *uj = (Targy*) malloc(sizeof(Targy));
                            uj->tipus = golyo;
                            uj->x = rgx;
                            uj->y = rgy-10;
                            targy_listaba(t, uj);
                        }
                        break;
                    case SDLK_ESCAPE:
                        kilep = true;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_USEREVENT:
                /* billentyuzet */
                if (keystate[SDL_SCANCODE_LEFT] && rgx>20)
                    rgx -= 10;
                if (keystate[SDL_SCANCODE_RIGHT] && rgx<ABLAK_SZ-20)
                    rgx += 10;

                /* mozgatas */
                bool utkozes = targyak_mozgat(t, rgx, rgy);
                if (utkozes)
                    piros = 50;
                if (piros > 0)
                    piros--;

                /* uj fak - eljott az ido? */
                if (rand()%5 == 0) {
                    Targy *uj = (Targy*) malloc(sizeof(Targy));
                    uj->tipus = fa;
                    uj->x = rand()%(ABLAK_SZ-40)+20;
                    uj->y = 0;
                    targy_listaba(t, uj);
                }
                if (kovetkezoellenseg-- < 0) {
                    Targy *uj = (Targy*) malloc(sizeof(Targy));
                    uj->tipus = ellenseg;
                    uj->x = rand()%(ABLAK_SZ-40)+20;
                    uj->y = 0;
                    targy_listaba(t, uj);
                    kovetkezoellenseg = rand()%20+20;
                }

                /* kirajzoljuk amit sikerult */
                palya_rajzol(renderer, rgx, rgy, t, piros > 0);
                break;
            case SDL_QUIT:
                kilep = true;
                break;
        }
    }
    SDL_RemoveTimer(id);

    /* memoria felszabaditas */
    while (t != NULL) {
        Targy *kov = t->kov;
        free(t);
        t = kov;
    }
}


int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Window *window = SDL_CreateWindow("Repulo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ABLAK_SZ, ABLAK_M, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    srand(time(NULL));

    jatek(renderer);

    SDL_Quit();

    return 0;
}
