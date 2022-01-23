/*
 * Ez a program szmajlikat keres a beirt szovegben, es atalakitja oket.
 * A :) karaktersorozatbol ☻, a <3 karaktersorozatbol ♥ karakter lesz,
 * a szoveg tobbi resze valtozatlan marad.
 * 
 * Ezt a ket karaktert lent a sztringekben UTF-8 karakterkodolassal
 * adtuk meg. Ebben a kodolasban mindket karaktert harom bajttal
 * irnak le: a ☻ karakternek a 0xE2 0x98 0xBB bajtsorozat, a
 * ♥ karakternek pedig a 0xE2 0x99 0xA5 bajtsorozat felel meg.
 * (Vagyis nem igaz az, hogy egy karakter az egy char, hanem jelen
 * esetben harom! Sajnos sok felreertes szokott abbol lenni, hogy
 * a C-ben a bajtot charnak hivjak.)
 * 
 * Ilyen bajtsorozatokat egy C sztringben ugy lehet megadni, hogy
 * az egyes bajtokat oktalisan, 8-as szamrendszerben adjuk meg, egy
 * \ visszaper karakterrel bevezetve. Vagyis a
 *   printf("\342\230\273");
 * sor egy vigyorit ir ki, a
 *   printf("\342\231\245");
 * sor pedig egy szivet.
 *
 * Sokszor gond nelkul mukodik egy printf("♥"); is. Hogy ne legyen
 * problema a fajl letoltesenel, ezert inkabb az oktalisan megadott
 * forma szerepel.
 *
 * Linuxon es mas oprendszereken, ahol egybol van UTF-8 karakterkeszlet,
 * rogton mukodik is a program.
 *
 * Windowson a leforditott programot parancssorbol kell kezzel elinditani.
 * Eloszor is, a parancssori ablak menujere kattintva, a tulajdonsagok pont
 * alatt a betutipust at kell allitani Lucida console-ra. Lasd a
 * karakterkodolasokkal kapcsolatos irast.
 */

#include <stdio.h>

int main(void) {
    typedef enum SzmajliAllapot {
        Alap,
        Kettospont,
        Kisebb
    } SzmajliAllapot;

    SzmajliAllapot all = Alap;
    int c;
    while ((c = getchar()) != EOF) {
        /* Az allapotgepet nem feltetlenul kell ugy lekodolni,
         * hogy a kulso switch() az allapot szerint van. Lehet
         * az esemeny szerint is. Lenyeg, hogy az esetszetvalasztas
         * megtortenjen mindketto szerint. */
        switch (c) {
            default:
                switch (all) {
                    case Alap: printf("%c", c); break;
                    case Kettospont: printf(":%c", c); break;
                    case Kisebb: printf("<%c", c); break;
                }
                all = Alap;
                break;
            case ':':
                switch (all) {
                    case Alap: /* semmi */ break;
                    case Kettospont: putchar(':'); break;
                    case Kisebb: putchar('<'); break;
                }
                all = Kettospont;
                break;
            case ')':
                switch (all) {
                    case Alap: printf("%c", c); break;
                    case Kettospont: printf("\342\230\273"); break;
                    case Kisebb: printf("<%c", c); break;
                }
                all = Alap;
                break;
            case '<':
                switch (all) {
                    case Alap: /* semmi */ break;
                    case Kettospont: putchar(':'); break;
                    case Kisebb: putchar('<'); break;
                }
                all = Kisebb;
                break;
            case '3':
                switch (all) {
                    case Alap: printf("%c", c); break;
                    case Kettospont: printf(":%c", c); break;
                    case Kisebb: printf("\342\231\245"); break;
                }
                all = Alap;
                break;
       }
   }
   
   return 0;
}
