#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define PALYASZELES 20      /* ennyi meter szeles a palya */
#define HOSSZEGYSEG 10      /* ennyi meterenkent van fa es boja */
#define ELORE 25            /* indulaskor ennyi egysegnyi palyat hoz letre elore */

SDL_Renderer *renderer;
SDL_Window *window;
const int ABLAK_SZ = 640, ABLAK_M = 480;

typedef struct Pont3D {
    double x, y, z;
} Pont3D;

typedef struct Targy {
    enum { fa, boja, palyaszele } tipus;
    bool nekiment;              /* igaz, ha mar megkapta erte a pontot */
    Pont3D p0;                  /* referenciapont */
    Pont3D n1[4], n2[4];        /* ket negyszog - rajzhoz */
    Uint32 c1, c2;              /* ket szin */
    struct Targy *kov;          /* lancolt listahoz */
} Targy;

/* 3d vektormuveletek */
Pont3D pont3d_eltol(Pont3D p, double dx, double dy, double dz) {
    Pont3D uj = {p.x + dx, p.y + dy, p.z + dz};
    return uj;
}

Pont3D pont3d_forgat_x(Pont3D p, double szog) {
    Pont3D f = {p.x, p.y *cos(szog) - p.z *sin(szog), p.y *sin(szog) + p.z *cos(szog)};
    return f;
}

Pont3D pont3d_forgat_y(Pont3D p, double szog) {
    Pont3D f = {p.x *cos(szog) - p.z *sin(szog), p.y, p.x *sin(szog) + p.z *cos(szog)};
    return f;
}

Pont3D pont3d_forgat_z(Pont3D p, double szog) {
    Pont3D f = {p.x *cos(szog) - p.y *sin(szog), p.x *sin(szog) + p.y *cos(szog), p.z};
    return f;
}

double pont3d_tavolsag(Pont3D p1, double x2, double y2, double z2) {
    double dx = p1.x - x2, dy = p1.y - y2, dz = p1.z - z2;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

/* minden targyat elmozdit az adott vektorral */
void jelenet_mozgat(Targy *lista, double dx, double dy, double dz) {
    Targy *iter;
    for (iter = lista->kov; iter != NULL; iter = iter->kov) {
        int i;
        iter->p0 = pont3d_eltol(iter->p0, dx, dy, dz);
        for (i = 0; i < 4; i++) {
            iter->n1[i] = pont3d_eltol(iter->n1[i], dx, dy, dz);
            iter->n2[i] = pont3d_eltol(iter->n2[i], dx, dy, dz);
        }
    }
}


void targy_lista_elejere(Targy *lista, Targy *t) {
    t->kov = lista->kov;
    lista->kov = t;
}


Targy *fa_hozzaad(Pont3D p0) {
    Targy *uj = (Targy *) malloc(sizeof(Targy));
    uj->tipus = fa;
    uj->nekiment = false;
    uj->p0 = p0;
    uj->n1[0] = pont3d_eltol(p0, -0.1, 0, 0);
    uj->n1[1] = pont3d_eltol(p0, -0.1, 0.3, 0);
    uj->n1[2] = pont3d_eltol(p0,  0.1, 0.3, 0);
    uj->n1[3] = pont3d_eltol(p0,  0.1, 0, 0);
    uj->c1 = 0x804040FF; /* barna */
    uj->n2[0] = pont3d_eltol(p0, -0.8, 0.3, 0);
    uj->n2[1] = pont3d_eltol(p0, -0.1, 4.3, 0);
    uj->n2[2] = pont3d_eltol(p0,  0.1, 4.3, 0);
    uj->n2[3] = pont3d_eltol(p0,  0.8, 0.3, 0);
    uj->c2 = 0x008000FF; /* zold */
    return uj;
}


Targy *boja_hozzaad(Pont3D p0) {
    Targy *uj = (Targy *) malloc(sizeof(Targy));
    uj->tipus = boja;
    uj->nekiment = false;
    uj->p0 = p0;
    uj->n2[0] = pont3d_eltol(p0,  0.00, 1.6, 0);
    uj->n2[1] = pont3d_eltol(p0,  0.00, 2.0, 0);
    uj->n2[2] = pont3d_eltol(p0,  0.50, 1.9, 0);
    uj->n2[3] = pont3d_eltol(p0,  0.50, 1.7, 0);
    uj->c1 = 0xFF00FFFF; /* lila */
    uj->n1[0] = pont3d_eltol(p0, -0.02, 0, 0);
    uj->n1[1] = pont3d_eltol(p0, -0.02, 2, 0);
    uj->n1[2] = pont3d_eltol(p0,  0.02, 2, 0);
    uj->n1[3] = pont3d_eltol(p0,  0.02, 0, 0);
    uj->c2 = 0xFF00FFFF; /* lila */
    return uj;
}

Targy *palyaszele_hozzaad(Pont3D p0) {
    Targy *uj = (Targy *) malloc(sizeof(Targy));
    uj->tipus = palyaszele;
    uj->nekiment = false;
    uj->p0 = p0;
    uj->n1[0] = pont3d_eltol(p0, -PALYASZELES, 0.1, 0);
    uj->n1[1] = pont3d_eltol(p0, -PALYASZELES, 0.1, HOSSZEGYSEG);
    uj->n1[2] = pont3d_eltol(p0, -PALYASZELES, 0.2, HOSSZEGYSEG);
    uj->n1[3] = pont3d_eltol(p0, -PALYASZELES, 0.2, 0);
    uj->c1 = 0x404040FF; /* szurke */
    uj->n2[0] = pont3d_eltol(p0, PALYASZELES, 0.1, 0);
    uj->n2[1] = pont3d_eltol(p0, PALYASZELES, 0.1, HOSSZEGYSEG);
    uj->n2[2] = pont3d_eltol(p0, PALYASZELES, 0.2, HOSSZEGYSEG);
    uj->n2[3] = pont3d_eltol(p0, PALYASZELES, 0.2, 0);
    uj->c2 = 0x404040FF; /* szurke */
    return uj;
}

/* igazzal ter vissza, ha torolni kene */
bool negyszog_kepernyore(SDL_Renderer *kep, Pont3D negyszog[], int col, double xpos, double irany, double doles) {
    int i;
    Sint16 pontx[4], ponty[4];

    for (i = 0; i < 4; i++) {
        Pont3D f;
        double xk, yk;

        f = pont3d_eltol(negyszog[i], xpos, -1.7, 0); /* sielo x pozicioja es szemmagassaga */
        f = pont3d_forgat_x(f, 13 * 3.14 / 180);    /* lejto dolese */
        f = pont3d_forgat_y(f, irany);              /* fordulas (merre nez) */
        f = pont3d_forgat_z(f, doles);              /* kanyarban doles */
        /* vegul f a nezet iranyaba transzformalt pont. ha tul kozel van, vagy negativ, eldobjuk */
        if (f.z < 0.2)
            return true;
        /* nezet sikja: 1m tavol */
        xk =  f.x / (f.z + 1) * 500 + ABLAK_SZ / 2;
        yk = -f.y / (f.z + 1) * 500 + ABLAK_M / 2;
        pontx[i] = xk;
        ponty[i] = yk;
    }
    filledPolygonColor(kep, pontx, ponty, 4, col);
    aapolygonColor(kep, pontx, ponty, 4, 0x00000080);
    return false;
}

/* kirajzolja a targyakat. hatulrol elorefele halad, hogy elfedjek egymast, kb ahogy kell */
void jelenet_feldolgoz(SDL_Texture *hatter, Targy *lista, double xpos, double irany, double doles, int *pont) {
    Targy *lemarado, *iter;

    /* a hatter masolatara rajzolunk */
    SDL_RenderCopy(renderer, hatter, NULL, NULL);

    lemarado = lista;
    iter = lista->kov;
    while (iter != NULL) {
        /* utkozes faval, vagy boja? 1m-nel kozelebbi referenciapontok */
        if (!iter->nekiment && pont3d_tavolsag(iter->p0, -xpos, 0, 0) < 1) {
            iter->nekiment = true;
            switch (iter->tipus) {
                case boja:
                    *pont += 1;
                    break;
                case fa:
                    *pont -= 10;
                    break;
                case palyaszele: /* semmi */
                    break;
            }
        }
        /* kirajzolja; esetleg torli */
        bool torol = negyszog_kepernyore(renderer, iter->n1, iter->c1, xpos, irany, doles)
                     || negyszog_kepernyore(renderer, iter->n2, iter->c2, xpos, irany, doles);
        if (torol) {
            lemarado->kov = iter->kov;
            free(iter);     /* ilyenkor a lemarado nem valtozik */
            iter = lemarado->kov;
        } else {
            lemarado = iter;
            iter = iter->kov;       /* amugy megyunk a kovetkezore */
        }
    }

    char s[100];
    sprintf(s, "Szerzett pontok: %d", *pont);
    stringColor(renderer, ABLAK_SZ / 2 - strlen(s) * 8 / 2, 10, s, 0xFFFF00FF);

    SDL_RenderPresent(renderer);
}


SDL_Texture *hatter_megrajzol() {
    SDL_Surface *hatter = SDL_CreateRGBSurface(0, ABLAK_SZ, ABLAK_M, 32, 0, 0, 0, 0);
    SDL_Renderer *hatter_renderer = SDL_CreateSoftwareRenderer(hatter);

    boxColor(hatter_renderer, 0, 0, hatter->w - 1, hatter->h / 2 + 49, 0x2020FFFF); /* ho */
    boxColor(hatter_renderer, 0, hatter->h / 2 + 50, hatter->w - 1, hatter->h - 1, 0xFFFFFFFF); /* ho */

    filledCircleColor(hatter_renderer, hatter->w, 0, 32, 0xFFFF00FF);        /* nap */
    int x;
    for (x = 180; x < 270; x += 10) {
        aalineColor(hatter_renderer, hatter->w, 0, hatter->w + 70 * cos(x * 3.14 / 180), -70 * sin(x * 3.14 / 180), 0xFFFF00FF);
        aalineColor(hatter_renderer, hatter->w, 1, hatter->w + 70 * cos(x * 3.14 / 180), -70 * sin(x * 3.14 / 180) + 1, 0xFFFF00FF);
    }

    SDL_Texture *hatter_t = SDL_CreateTextureFromSurface(renderer, hatter);
    SDL_DestroyRenderer(hatter_renderer);
    SDL_FreeSurface(hatter);

    return hatter_t;
}

/* general egy uj HOSSZEGYSEG meretu palyat: ket oldalt palyaszelet, egy fat es egy bojat */
void uj_palyaresz(Targy *lista, double z) {
    Pont3D p0 = {0, 0, z};
    p0.x = (rand() % PALYASZELES * 2 - PALYASZELES) * 0.9;
    targy_lista_elejere(lista, fa_hozzaad(p0));
    p0.x = (rand() % PALYASZELES * 2 - PALYASZELES) * 0.9;
    targy_lista_elejere(lista, boja_hozzaad(p0));
    p0.x = 0;
    targy_lista_elejere(lista, palyaszele_hozzaad(p0));
}


static Uint32 idozit(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}


int main(int argc, char *argv[]) {
    Targy lista;        /* targyak listajanak nem dinamikusan foglalt strazsaja */
    lista.kov = NULL;  /* a strazsa kovetkezo pointere null */

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    window = SDL_CreateWindow("Si", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ABLAK_SZ, ABLAK_M, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    srand(time(NULL));

    SDL_Texture *hatter = hatter_megrajzol();
    for (int i = 0; i < ELORE; i++) /* elore legyart valamennyi palyat */
        uj_palyaresz(&lista, i * HOSSZEGYSEG);

    double v = 1;          /* sebesseg */
    double szog = 90;      /* milyen iranyba megy epp, 90fok = egyenesen a volgynek */
    double xpos = 0;       /* aktualis x pozicio, keresztbe a palyan */
    double doles = 0;      /* mennyire dol epp be a kanyarban */
    double lefele = 0;     /* hany metert ment lefele */
    int pont = 0;          /* hany pontot gyujtott */
    const Uint8 *key = SDL_GetKeyboardState(NULL);
    SDL_TimerID id = SDL_AddTimer(40, idozit, NULL);
    bool kilep = false;
    while (!kilep) {
        SDL_Event ev;
        SDL_WaitEvent(&ev);
        switch (ev.type) {
            case SDL_QUIT:
                kilep = true;
                break;
            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_ESCAPE)
                    kilep = 1;
                break;
            case SDL_USEREVENT:
                doles *= 0.7;
                if (key[SDL_SCANCODE_LEFT]) {        /* balra kanyarodas - nyomva tartassal */
                    szog -= 3;
                    if (szog < 75)           /* limit */
                        szog = 75;
                    if (doles > -10)         /* bedoles a kanyarban (z tengely szerinti forg.) */
                        doles -= 1;
                }
                if (key[SDL_SCANCODE_RIGHT]) {       /* jobbra kanyarodas */
                    szog += 3;
                    if (szog > 105)
                        szog = 105;
                    if (doles < 10)
                        doles += 1;
                }
                if (v < 1 && !key[SDL_SCANCODE_DOWN]) /* lejton gyorsul */
                    v += 0.05;
                if (key[SDL_SCANCODE_DOWN]) {        /* hoekezve lassul */
                    v -= 0.03;
                    if (v < 0.1)
                        v = 0.1;
                }

                xpos += 2 * v * cos(szog * 3.14 / 180);
                if (xpos < -PALYASZELES)
                    xpos = -PALYASZELES;
                if (xpos > PALYASZELES)
                    xpos = PALYASZELES;
                lefele += -v * sin(szog * 3.14 / 180);
                /* ha ennyit ment lefele, akkor uj HOSSZEGYSEGnyi meretet general a palyahoz */
                if (lefele < -HOSSZEGYSEG) {
                    uj_palyaresz(&lista, (ELORE - 1)*HOSSZEGYSEG);
                    lefele += HOSSZEGYSEG;
                }
                jelenet_mozgat(&lista, 0, 0, -v * sin(szog * 3.14 / 180));
                jelenet_feldolgoz(hatter, &lista, xpos, (szog - 90) * 3.14 / 180, doles * 3.14 / 180, &pont);
                break;
        }
    }
    SDL_RemoveTimer(id);

    /* felszabadítások */
    SDL_DestroyTexture(hatter);
    while (lista.kov != NULL) {
        Targy *kov = lista.kov->kov;
        free(lista.kov);
        lista.kov = kov;
    }

    return 0;
}
