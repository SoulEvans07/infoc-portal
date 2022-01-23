#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * EZEK A RESZEK TARTOZNAK A TOOLKITHEZ
 */

/* itt lehet állítgatni a színeket */
unsigned long const hatter = 0x303030FF;
unsigned long const keret = 0x000000FF;
unsigned long const alapszin = 0x101010FF;
unsigned long const keretvilagos = 0xFFFFFF20;
unsigned long const gombfeliratszin = 0x00C0F0FF;
unsigned long const csuszkaszin = 0x00FFFF50;
unsigned long const feliratszin = 0x00C0F0FF;

/* mindenki hasznalja */
SDL_Renderer *renderer;

/* egy widget, az altalanos es a specialis adatokkal */
typedef struct Widget Widget;
struct Widget {
    int x, y, szeles, magas;            /* pozicio es meret */

    /* kirajzolashoz hasznalt fv */
    void (*rajzolo_fv)(Widget *widget);
    void (*kattintas_fv)(Widget *widget, int x, int y);
    /* kattintaskor ez hivodik, ha nem NULL */
    void (*felhasznaloi_cb)(Widget *widget, int x, int y, void *param);
    void *felhasznaloi_cb_param;           /* ezt a parametert megkapja a param valtozoban */

    enum WidgetTipus {                  /* ilyen típusú lehet */
        gomb, csuszka, szinesteglalap, felirat
    } tipus;
    union {
        struct {
            char felirat[20];           /* a gomb szövege */
        } gomb;
        struct {
            double jelenlegi;           /* erteke; 0.0-1.0 */
        } csuszka;
        struct {
            unsigned char r, g, b;      /* szin */
        } szinesteglalap;
        struct {
            char szoveg[20];
        } felirat;
    } adat;
};

/* egy widget kirajzolasa, altalanossagban. egy keretet ad csak. */
/* a keret tulnyulik az aktiv (kattinthato) mereten. */
void widget_alap_rajzol(Widget *widget) {
    roundedRectangleColor(renderer, widget->x - 1, widget->y - 1, widget->x + widget->szeles, widget->y + widget->magas, 2, keret);
    boxColor(renderer, widget->x, widget->y, widget->x + widget->szeles - 1, widget->y + widget->magas - 1, alapszin);
    for (int y = 0; y < 20; ++y) {
        boxRGBA(renderer, widget->x, widget->y + y * widget->magas / 20, widget->x + widget->szeles - 1, widget->y + (y + 1)*widget->magas / 20 - 1, 255, 255, 255, (19 - y) * 3);
    }
    rectangleColor(renderer, widget->x, widget->y, widget->x + widget->szeles - 1, widget->y + widget->magas - 1, keretvilagos);
}

/* letrehoz egy uj, egyelore ismeretlen tipusu widgetet. tulajdonkepp csak a helyet allitja be */
Widget *uj_widget(int x, int y, int szeles, int magas) {
    Widget *uj = (Widget *) malloc(sizeof(Widget));
    uj->x = x;
    uj->y = y;
    uj->szeles = szeles;
    uj->magas = magas;
    uj->rajzolo_fv = widget_alap_rajzol;
    uj->kattintas_fv = NULL;
    uj->felhasznaloi_cb = NULL;
    uj->felhasznaloi_cb_param = NULL;
    return uj;
}

/* kirajzolja a gombot. keret, alapszin, felirat. */
void gomb_rajzol(Widget *gomb) {
    widget_alap_rajzol(gomb);
    stringColor(renderer, gomb->x + (gomb->szeles - strlen(gomb->adat.gomb.felirat) * 8) / 2, gomb->y + (gomb->magas - 8) / 2, gomb->adat.gomb.felirat, gombfeliratszin);
}

/* gombot hoz letre. */
Widget *uj_gomb(int x, int y, int szeles, int magas, char const *felirat) {
    Widget *uj = uj_widget(x, y, szeles, magas);
    uj->tipus = gomb;
    uj->rajzolo_fv = gomb_rajzol;
    strcpy(uj->adat.gomb.felirat, felirat);
    return uj;
}

/* kirajzol egy feliratot. nem hivodik az alap rajzolo, mert nem kell neki keret! */
void felirat_rajzol(Widget *felirat) {
    stringColor(renderer, felirat->x, felirat->y, felirat->adat.felirat.szoveg, feliratszin);
}

/* feliratot hoz letre. */
Widget *uj_felirat(int x, int y, char const *szoveg) {
    Widget *uj = uj_widget(x, y, 0, 0);
    uj->tipus = felirat;
    uj->rajzolo_fv = felirat_rajzol;
    strcpy(uj->adat.felirat.szoveg, szoveg);
    return uj;
}

/* csuszkaot rajzol; a keretet meghagyja, de belul mindent
 * felulir a sajat szinevel */
void csuszka_rajzol(Widget *csuszka) {
    widget_alap_rajzol(csuszka);
    boxColor(renderer, csuszka->x, csuszka->y, csuszka->x + csuszka->szeles * csuszka->adat.csuszka.jelenlegi, csuszka->y + csuszka->magas - 1, csuszkaszin);
}

/* ez a sajat adatfeldolgozoja; nem kulon hozzaadott, hanem elvalaszthatatlan */
void csuszka_kattintas(Widget *csuszka, int x, int y) {
    csuszka->adat.csuszka.jelenlegi = (double) (x - 1) / (csuszka->szeles);
    csuszka_rajzol(csuszka);
}

/* csuszkat hoz letre */
Widget *uj_csuszka(int x, int y, int szeles, int magas, double kezdeti) {
    Widget *uj = uj_widget(x, y, szeles, magas);
    uj->tipus = csuszka;
    uj->rajzolo_fv = csuszka_rajzol;
    uj->kattintas_fv = csuszka_kattintas;
    uj->adat.csuszka.jelenlegi = kezdeti;
    return uj;
}

/* szines teglalapot rajzol ki; az eredeti tartalmat teljesen felulirja az adott szinnel */
void szinesteglalap_rajzol(Widget *teglalap) {
    widget_alap_rajzol(teglalap);
    boxRGBA(renderer, teglalap->x, teglalap->y, teglalap->x + teglalap->szeles - 1, teglalap->y + teglalap->magas - 1,
            teglalap->adat.szinesteglalap.r, teglalap->adat.szinesteglalap.g, teglalap->adat.szinesteglalap.b, 255);
}

/* szines teglalapot hoz letre, kezdeti ertekkent a megadott szinekkel */
Widget *uj_szinesteglalap(int x, int y, int szeles, int magas, unsigned char r, unsigned char g, unsigned char b) {
    Widget *uj = uj_widget(x, y, szeles, magas);
    uj->tipus = szinesteglalap;
    uj->rajzolo_fv = szinesteglalap_rajzol;
    uj->adat.szinesteglalap.r = r;
    uj->adat.szinesteglalap.g = g;
    uj->adat.szinesteglalap.b = b;
    return uj;
}

/* az esemenyhurok kirajzolja a widgeteket, es feldolgozza a felhasznalotol erkezo
 * esemenyeket (eger kattintasok es ablak bezarasa.) */
void esemenyvezerelt_main(Widget *widgetek[]) {

    /* azzal kezdjuk, hogy mindent kirajzolunk */
    SDL_SetRenderDrawColor(renderer, (hatter & 0xFF000000) >> 6, (hatter & 0x00FF0000) >> 4, (hatter & 0x0000FF00) >> 2, (hatter & 0x000000FF));
    SDL_RenderClear(renderer);
    for (int i = 0; widgetek[i] != NULL; i++)
        widgetek[i]->rajzolo_fv(widgetek[i]);
    SDL_RenderPresent(renderer);

    /* esemenyhurok */
    bool vege = false;
    SDL_Event ev;
    while (!vege && SDL_WaitEvent(&ev)) {
        switch (ev.type) {
        case SDL_QUIT:
            vege = true;
            break;

        case SDL_MOUSEBUTTONUP:
            for (int i = 0; widgetek[i] != NULL; i++) {
                /* ennek a widgetnek a teruletere kattintott? */
                if (ev.button.x >= widgetek[i]->x && ev.button.y >= widgetek[i]->y
                        && ev.button.x < widgetek[i]->x + widgetek[i]->szeles
                        && ev.button.y < widgetek[i]->y + widgetek[i]->magas)
                {
                    /* widget-relativ kattintas koordinatak */
                    int bx = ev.button.x - widgetek[i]->x;
                    int by = ev.button.y - widgetek[i]->y;
                    /* widgetek belso mukodese */
                    if (widgetek[i]->kattintas_fv != NULL)
                        widgetek[i]->kattintas_fv(widgetek[i], bx, bx);
                    /* esetleg tarsitott callback */
                    if (widgetek[i]->felhasznaloi_cb != NULL)
                        widgetek[i]->felhasznaloi_cb(widgetek[i], bx, by,
                                                     widgetek[i]->felhasznaloi_cb_param);
                }
            }
            SDL_RenderPresent(renderer);       /* tortenhettek valtozasok, rajzolas */
            break;
        }
    }
}

/* a gomb rakattintva general egy szintetikus SDL_QUIT esemenyt.
 * barmely alkalmazas hasznalhatja kilepes gombnak */
void kilep_gomb_cb(Widget *widget, int x, int y, void *param) {
    SDL_Event ev = {SDL_QUIT};
    SDL_PushEvent(&ev);
}


/*
 * INNENTOL JON AZ ALKALMAZASUNK
 */

/* egy ilyen strukturara mutato pointer lesz az apply gomb void* parametere.
 * tartalmazza a widgeteket, hogy r,g,b csuszkaokbol a teglalap szinezheto legyen. */
typedef struct UIAdat {
    Widget *r, *g, *b, *teglalap;
} UIAdat;

void mehet_gomb_cb(Widget *widget, int x, int y, void *param) {
    UIAdat *widgetek = (UIAdat *) param;
    widgetek->teglalap->adat.szinesteglalap.r = widgetek->r->adat.csuszka.jelenlegi * 255.0;
    widgetek->teglalap->adat.szinesteglalap.g = widgetek->g->adat.csuszka.jelenlegi * 255.0;
    widgetek->teglalap->adat.szinesteglalap.b = widgetek->b->adat.csuszka.jelenlegi * 255.0;
    widgetek->teglalap->rajzolo_fv(widgetek->teglalap);
}

int main(int argc, char *argv[]) {
    /* SDL inditasa */
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Window *window = SDL_CreateWindow("Widgetek", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 276, 212, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }

    /* widgetek létrehozása */
    Widget *widgetek[9 + 1];
    widgetek[0] = uj_gomb(216, 10, 50, 32, "kil\x82p");
    widgetek[1] = uj_felirat(10, 52, "v\x94r\x94s");
    widgetek[2] = uj_csuszka(10, 64, 256, 20, 0.5);
    widgetek[3] = uj_felirat(10, 90, "z\x94ld");
    widgetek[4] = uj_csuszka(10, 102, 256, 20, 0.0);
    widgetek[5] = uj_felirat(10, 128, "k\x82k");
    widgetek[6] = uj_csuszka(10, 140, 256, 20, 0.0);
    widgetek[7] = uj_gomb(10, 170, 50, 32, "mehet");
    widgetek[8] = uj_szinesteglalap(138, 170, 128, 32, 128, 0, 0);
    widgetek[9] = NULL;

    /* a 0. gomb kattintasa - kilepes */
    widgetek[0]->felhasznaloi_cb = kilep_gomb_cb;

    /* a mehet gomb kattintasa + parametere */
    UIAdat klikk_adat;
    klikk_adat.r = widgetek[2];
    klikk_adat.g = widgetek[4];
    klikk_adat.b = widgetek[6];
    klikk_adat.teglalap = widgetek[8];
    widgetek[7]->felhasznaloi_cb = mehet_gomb_cb;
    widgetek[7]->felhasznaloi_cb_param = &klikk_adat;

    esemenyvezerelt_main(widgetek);

    for (int i = 0; widgetek[i] != NULL; ++i)
        free(widgetek[i]);

    return 0;
}
