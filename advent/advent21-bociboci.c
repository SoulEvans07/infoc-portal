#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_keycode.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


#define PI 3.14159265


typedef struct Hang {
    SDL_Keycode sym;         /* billentyuje */
    double frek;        /* a frekvenciaja */
    double hangero;     /* a hangero, 0->1 és 1->0 valtoztatva, hogy ne pattogjon */
    bool szol;          /* epp szol-e */
} Hang;


/* nehany globalis valtozo a hangok adataihoz */
SDL_AudioSpec audiospec;
double const hangero = 4096;            /* itt allithato a program hangereje */
double const hangositas = 0.025;        /* a ki-be kapcsolashoz */
double t = 0;                           /* az eltelt ido */


/* ez hivodik meg, amikor az SDL ujabb adag hangot ker tolunk */
void hang_callback(void *userdata, Uint8 *stream8, int len) {
    Hang *hangok = (Hang *) userdata;     /* a main tombjet atvesszuk a pointeren keresztul */
    Sint16 *stream = (Sint16 *) stream8;  /* a puffer, amit kaptunk: signed integer, 16 bit */
    double dt = 1.0/audiospec.freq;       /* ennyi masodperc hangmintankent */

    int mintak = len / sizeof(stream[0]);         /* mert bajtban szamol, nekunk meg mintak szama kell */

    /* vegigmegyunk ezen az idoszeleten */
    for (int x = 0; x < mintak; x++) {
        double s = 0;       /* az aktualis minta */

        /* es osszegzunk (osszemixelunk) minden hangot */
        for (int i = 0; hangok[i].sym != SDLK_UNKNOWN; ++i) {
            double s_ez;

            /* szinusz - a leglagyabb hang
             *     _     _
             *    / \   / \
             * \_/   \_/   \_ */
            s_ez = sin(hangok[i].frek * 2 * PI * t);

            /* fureszfog - mint a reszelo, nem csak a rajzon
             *  /| /| /|
             * / |/ |/ | */
            //~ s_ez = fmod(hangok[i].frek * t, 1) * 2 - 1;

            /* haromszog - fureszfognal lagyabb
             *  /\  /\
             * /  \/  \ */
            //~ s_ez = fabs(fmod(hangok[i].frek * t, 1)-0.5) * 4 - 1;

            /* negyszog - 80as evek videojatekai
             *    __    __
             *   |  |  |  |
             * __|  |__|  |__ */
            //~ s_ez = round(fmod(hangok[i].frek * t, 1)) * 2 - 1;

            /* itt allitodik a hangereje az adott hangnak, hogy
             * a ki/bekapcsolaskor ne legyen pattanas */
            if (hangok[i].szol) {
                if (hangok[i].hangero < 1)
                hangok[i].hangero += hangositas;
            } else {
                if (hangok[i].hangero < hangositas)
                    hangok[i].hangero = 0;
                if (hangok[i].hangero > 0)
                    hangok[i].hangero -= hangositas;
            }
            s += s_ez * hangero * hangok[i].hangero;
        }
        /* itt bekerul a pufferbe az osszemixelt minta */
        stream[x] = s;

        t += dt;    /* eltelt ennyi ido */
    }
}


/* SDL hanglejatszas inditasa */
void hang_init(Hang *hangok) {
    audiospec.freq = 44100;                /* 44100Hz - CD minoseg, 48000 - dvd minoseg */
    audiospec.format = AUDIO_S16SYS;       /* 16-bit elojeles; a rendszer bajtsorrendjevel */
    audiospec.channels = 1;                /* mono */
    audiospec.samples = audiospec.freq/50; /* puffer merete - 1/50 sec */
    audiospec.callback = hang_callback;    /* sdl hivja, ha ujabb adag hang kell neki */
    audiospec.userdata = hangok;           /* egy pointert atad mindig a callbacknek */
    if (SDL_OpenAudio(&audiospec, NULL) < 0) {
        fprintf(stderr, "Hiba a hanggal: %s\n", SDL_GetError());
        exit(1);
    }
}


/* amit a neve mond. */
void zongora_rajzol(SDL_Renderer *renderer, int siz, int keret) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    for (int i = 0; i < 8; ++i) {
        rectangleColor(renderer, i*siz+keret, keret, (i+1)*siz+keret-1, 5*siz+keret, 0x000000FF);
        characterColor(renderer, i*siz+keret+(siz-8)/2, 5*siz+keret-12, "QWERTYUI"[i], 0x000000FF);
    }
    for (int i = 0; i < 6; ++i) {
        if (i == 2) continue;
        boxColor(renderer, (i+1)*siz+keret-1-siz/3, keret, (i+1)*siz+keret-1+siz/3, 3.5*siz+keret, 0x000000FF);
        characterColor(renderer, (i+1)*siz+keret-1-siz/3+(2*siz/3-8)/2, 3.5*siz+keret-12, "234567"[i], 0xFFFFFFFF);
    }
    SDL_RenderPresent(renderer);
}


int main(int argc, char **argv) {
    Hang hangok[] = {
        { SDLK_q, 440.0 * pow(2, -9 * 1.0/12.0) },   /* C */
        { SDLK_2, 440.0 * pow(2, -8 * 1.0/12.0) },   /* C# */
        { SDLK_w, 440.0 * pow(2, -7 * 1.0/12.0) },   /* D */
        { SDLK_3, 440.0 * pow(2, -6 * 1.0/12.0) },   /* D# */
        { SDLK_e, 440.0 * pow(2, -5 * 1.0/12.0) },   /* E */
        { SDLK_r, 440.0 * pow(2, -4 * 1.0/12.0) },   /* F */
        { SDLK_5, 440.0 * pow(2, -3 * 1.0/12.0) },   /* F# */
        { SDLK_t, 440.0 * pow(2, -2 * 1.0/12.0) },   /* G */
        { SDLK_6, 440.0 * pow(2, -1 * 1.0/12.0) },   /* G# */
        { SDLK_y, 440.0 * pow(2,  0 * 1.0/12.0) },   /* A */
        { SDLK_7, 440.0 * pow(2,  1 * 1.0/12.0) },   /* A# */
        { SDLK_u, 440.0 * pow(2,  2 * 1.0/12.0) },   /* H */
        { SDLK_i, 440.0 * pow(2,  3 * 1.0/12.0) },   /* C' */
        { SDLK_UNKNOWN }    /* tomb veget jeloli */
    };
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    hang_init(hangok);

    /* zongora es ablak meretei */
    int siz = 60, keret = 20;

    /* ablak megnyitasa */
    SDL_Window *window = SDL_CreateWindow("Boci boci tarka", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, siz*8+2*keret, siz*5+2*keret, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }

    zongora_rajzol(renderer, siz, keret);

    /* hang inditasa */
    SDL_PauseAudio(0);

    /* esemenyvezerelt ciklus */
    bool kilep = false;
    while (!kilep) {
        SDL_Event ev;
        SDL_WaitEvent(&ev);

        switch (ev.type) {
            case SDL_QUIT:
                kilep = true;
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                /* magzar billentzuyet miatt */
                if (ev.key.keysym.sym == SDLK_z)
                    ev.key.keysym.sym = SDLK_y;
                for (int i = 0; hangok[i].sym != SDLK_UNKNOWN; ++i)
                    if (ev.key.keysym.sym == hangok[i].sym)    /* ehhez tartozik? */
                        hangok[i].szol = ev.type == SDL_KEYDOWN;
                break;
        }
    }

    /* hang megallitasa */
    SDL_PauseAudio(1);

    SDL_Quit();

    return 0;
}
