#include <SDL.h>
#include <SDL_image.h>
#include <SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>


#define SZELES 1024
#define MAGAS 600

#define MAXPEHELY 1500
#define MINSEB 1.2
#define MAXSEB 2.5
#define MAXSZEL 5

#define AGSZAM 3

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *hatter;


double randdouble(double min, double max) {
    return (double)rand()/RAND_MAX*(max-min)+min;
}


/*       _\/ . \/_
        . _\/ \/_ .
       .  ' \_/ '  .
       ->->->_<-<-<-
       '  ._/ \_.  '
        . _/\ /\_ .
    jgs   /\ ' /\       */
typedef struct Pehely {
    double x;
    double y;
    double s;
} Pehely;


Uint32 getpixel(SDL_Surface *surface, int x, int y) {
    Uint32 *p = (Uint32*) ((Uint8 *)surface->pixels + y * surface->pitch + x * sizeof(Uint32));
    return *p;
}

bool megakad(SDL_Surface *surface, int x, int y) {
    Uint32 pix = getpixel(surface, x, y);
    return ((pix >> surface->format->Rshift << surface->format->Rloss) & 0xFF) > 0x40
        || ((pix >> surface->format->Gshift << surface->format->Gloss) & 0xFF) > 0x40;
}

/*                  .------,
      .\/.          |______|
    _\_}{_/_       _|_Ll___|_
     / }{ \       [__________]          .\/.
      '/\'        /          \        _\_\/_/_
                 ()  o  o    ()        / /\ \
                  \ ~~~   .  /          '/\'
             _\/   \ '...'  /    \/_
              \\   {`------'}    //
               \\  /`---/',`\\  //
                \/'  o  | |\ \`//
                /'      | | \/ /\
   __,. -- ~~ ~|    o   `\|      |~ ~~ -- . __
               |                 |
          jgs  \    o            /
                `._           _.'
                   ^~- . -  ~^                   */
void ujpehely(Pehely *f) {
    f->x = randdouble(1, SZELES - 2);
    f->y = 0;
    f->s = randdouble(MINSEB, MAXSEB);
}

void mozgat(Pehely pelyhek[], int *db) {
    static double szelirany = 0, szeliranycel = 0;
    SDL_Surface *kep = SDL_GetWindowSurface(window);

    if (*db < MAXPEHELY) {
        ujpehely(&pelyhek[*db]);
        *db += 1;
    }
    for (int i = 0; i < *db; i++) {
        pelyhek[i].y += pelyhek[i].s;   /* lefele */
        if (pelyhek[i].y >= MAGAS) {
            pixelRGBA(renderer, pelyhek[i].x, MAGAS-1, 0xFF, 0xFF, 0xFF, 0x30);
            ujpehely(&pelyhek[i]);
            continue;
        }
        if (megakad(kep, pelyhek[i].x, pelyhek[i].y) && rand() % 10 == 0) {            /* v */
            /* <- */
            if (pelyhek[i].x>0 && !megakad(kep, pelyhek[i].x-1, pelyhek[i].y-1) && !megakad(kep, pelyhek[i].x-1, pelyhek[i].y) && rand()%10 != 0)
                pelyhek[i].x--;
            /* -> */
            else if (pelyhek[i].x<kep->w-1 && !megakad(kep, pelyhek[i].x+1, pelyhek[i].y-1) && !megakad(kep, pelyhek[i].x+1, pelyhek[i].y) && rand()%10 != 0)
                pelyhek[i].x++;
            else {
                pixelRGBA(renderer, pelyhek[i].x, pelyhek[i].y-1, 0xFF, 0xFF, 0xFF, 0x30);
                ujpehely(&pelyhek[i]);
            }
        }

        double m = pelyhek[i].x;    /* oldalra mozgas */
        pelyhek[i].x += szelirany + randdouble(-1.5, +1.5);
        if (pelyhek[i].x < 1)
            pelyhek[i].x += kep->w-2;
        if (pelyhek[i].x > kep->w-1)
            pelyhek[i].x -= kep->w-2;
        if (rand() % 2 == 0 && megakad(kep, pelyhek[i].x, pelyhek[i].y))
            pelyhek[i].x = m;
    }

    /*                 _..._
                      |     |
                      |     |
                      |_..._|
                   __.;.---.;.__
                   '-/       \-'
           ._  ,    |  o _ o  |    . _
             \|     |   (_>   | ,  |/
             _\\  . '\ '---' /   \//--.
            `  \\/  |`'-----;`\-.//_
             .--\\ .'-.____.|-(.//  ,
                 \)      _  \  \/ '-'\
                 |      (_)  |  ||.__/
                 |       _   \  \|
      """"     ""|      (_)   \_.;""      """
           "" jgs \             /     ""
      ""      ...  '._       _.'  ""     ...
         ..           '-----'
                  """        ..    """          */

    SDL_RenderCopy(renderer, hatter, NULL, NULL);
    for (int i = 0; i < *db; i++)
        pixelRGBA(renderer, pelyhek[i].x, pelyhek[i].y, 0xFF, 0xFF, 0xFF, 0xC0);
    SDL_RenderPresent(renderer);

    if (rand()%100 == 0)    /* szelirany valtas, 1:100 hogy valtunk */
        szeliranycel = randdouble(-MAXSZEL, MAXSZEL);
    /* valtunk vagy nem, a konkret szeliranyt lassan kozelitjuk a megcelzott ertekhez */
    if (szeliranycel > szelirany)
        szelirany += 0.01;
    if (szeliranycel < szelirany)
        szelirany -= 0.01;
}





/*             |
             '.'.'
            -= o =-
             .'.'.
               |
               ,
              / \
            .'. o'.
           / 6 s ^.\
          /.-.o *.-.\
          `/. '.'9  \`
         .'6. *  s o '.
        /.--.s .6 .--.\
        `/ s '. .' * .\`
       .' o 6 .` .^ 6 s'.
      /.---. * ^ o .----.\
      `/s * `.^ s.' ^ * \`
     .' o , 6 `.' ^ o  6 '.
    /,-^--,  o ^ * s ,----,\
    `'-._s.;-,_6_^,-;._o.-'
         jgs |   |
             `"""`          */

typedef struct AgAdat {
    double doles;
    double helyzet;
    double rovidules;
} AgAdat;

void myline(SDL_Renderer *bmp, double x1, double y1, double x2, double y2, double w, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    double dx = x2-x1, dy = y2-y1;
    int lepes = sqrt(dx*dx + dy*dy)/w * 2;
    dx /= (lepes == 0) ? 1 : lepes;
    dy /= (lepes == 0) ? 1 : lepes;
    for (int i = 0; i <= lepes; ++i)
        filledCircleRGBA(bmp, x1 + dx*i, y1 + dy*i, w, r, g, b, a);
}

void fat_rajzol(SDL_Renderer *bmp, double x, double y, double hossz, double szog, double atlatszosag, AgAdat agak[], int n, double zold, double r) {
    if (n < 0)
        return;
    int atl = atlatszosag * 255;
    double zold1 = zold + randdouble(-0.2, 0.2);
    myline(bmp, x, y, x + hossz*cos(szog), y - hossz*sin(szog), r,
                         atl/2, (int)(0xFF-atl)*zold1, 0, atl);
    for (int i = 0; i < AGSZAM; ++i) {
        fat_rajzol(bmp, x + hossz*agak[i].helyzet*cos(szog), y - hossz*agak[i].helyzet*sin(szog),
                           hossz * (1-agak[i].rovidules),
                           szog + agak[i].doles + randdouble(-0.03, 0.03),
                           atlatszosag * 0.85,
                           agak, n-1,
                           zold, r * 0.9);
    }
}

/*           *
            /.\
           /..'\
           /'.'\
          /.''.'\
          /.'.'.\
   "'""""/'.''.'.\""'"'"
     jgs ^^^[_]^^^      */
void randomfa(SDL_Renderer *bmp, double x) {
    AgAdat agak[AGSZAM];
    double dolesmin = -2.0;
    double dolesmax = 2.0;
    for (int i = 0; i < AGSZAM; ++i) {  /* too randomy */
        agak[i].doles = dolesmin + (dolesmax-dolesmin)/(AGSZAM-1)*i;
        agak[i].helyzet = randdouble(0.8, 1.0);
        agak[i].rovidules = 0.5;
    }
    agak[AGSZAM/2].rovidules = 0.22; /* hogy magasak legyenek, a torzs rovidulese fix. igy a default magassag hatarozza meg */
    agak[AGSZAM/2].helyzet = 1.0;
    fat_rajzol(bmp, x, MAGAS-32, randdouble(50, 80), 90*3.1415/180, 0.9, agak, 9, 0.6, 4);
    SDL_RenderPresent(bmp);
}

/*
  +----------------+
  |                |
  |                |
  |                |
  |                |
  |                |
  XXXXXXXXXXXXXXXXXX
                    */
void clear(SDL_Renderer *bmp) {
    boxRGBA(bmp, 0, 0, SZELES-1, MAGAS-1, 0x30, 0x30, 0x90, 0xFF);
    boxRGBA(bmp, 0, MAGAS-32, SZELES-1, MAGAS-1, 0xF0, 0xF0, 0xF0, 0xFF);
    SDL_RenderPresent(bmp);
}





/*
 * FŐPROGRAM
 */
/* will initialaiz */
void init(void) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    window = SDL_CreateWindow("Boldog karacsonyt!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SZELES, MAGAS, 0);
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
    SDL_Surface *hatter_s = SDL_CreateRGBSurface(0, SZELES, MAGAS, 32, 0, 0, 0, 0);
    SDL_Renderer *hatter_r = SDL_CreateSoftwareRenderer(hatter_s);

    clear(hatter_r);
    for (int i = 0; i < 5; ++i)
        randomfa(hatter_r, SZELES*i/4 + rand()%50 - 25);

    hatter = SDL_CreateTextureFromSurface(renderer, hatter_s);
    SDL_DestroyRenderer(hatter_r);
    SDL_FreeSurface(hatter_s);
}

/* wait litl */
Uint32 idozit(Uint32 ms, void* param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}


/*         _
          {_}
          / \
         /   \
        /_____\
      {`_______`}
       // . . \\
      (/(__7__)\)
      |'-' = `-'|
      |         |
      /\       /\
     /  '.   .'  \
    /_/   `"`   \_\
   {__}###[_]###{__}
   (_/\_________/\_)
       |___|___|
  jgs   |--|--|
       (__)`(__)        */

int main(int argc, char *argv[]) {
    Pehely pelyhek[MAXPEHELY];

    init();

    SDL_TimerID id = SDL_AddTimer(20, idozit, NULL);
    bool kilep = false;
    int db = 0;
    while (!kilep) {
        SDL_Event ev;
        SDL_WaitEvent(&ev);

        switch (ev.type) {
            case SDL_USEREVENT:
                mozgat(pelyhek, &db);
                break;
            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_ESCAPE)
                    kilep = true;
                break;

            case SDL_QUIT:
                kilep = true;
                break;
        }
    }
    SDL_RemoveTimer(id);
    SDL_DestroyTexture(hatter);

    SDL_Quit();
    return 0;
}
