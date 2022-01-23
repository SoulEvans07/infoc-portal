#include <stdio.h>

int main(void) {
    typedef enum LyAllapot {
        alap,
        l_volt,
        ll_volt,
    } LyAllapot;
    
    int szaml = 0;
    LyAllapot all = alap;      /* indulaskor */
    int c;
    while ((c = getchar()) != EOF) {
        switch (all) {
        case alap:   /* alap állapot */
            if (c == 'l')
                all = l_volt;
            break;

        case l_volt: /* már volt egy 'l' */
            switch (c) {
            case 'l':
                all = ll_volt;
                break;
            case 'y':
                szaml += 1;
                all = alap;
                break;
            default:
                all = alap;
                break;
            }
            break;

        case ll_volt:
            switch (c) {
            case 'l':
                break;
            case 'y':
                szaml += 2;
                all = alap;
                break;
            default:
                all = alap;
                break;
            }
            break;
        }
    }

    printf("%d darab volt.\n", szaml);

    return 0;
}
