#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


double cesaro_valoszinuseg(void);
int cesaro_kiserlet(void);
int lnko(int a, int b);


int main(void) {
    double pi;

    srand(time(0));
    pi = sqrt(6.0 / cesaro_valoszinuseg());
    printf("pi = %f", pi);
 
    return 0;
}


/* valoszinuseg. elvegzunk sokszor egy kiserletet: kiserlet(). megszamoljuk,
 * hanyszor sikerult; ezt elosztva az elvegzesek szamaval, megkapjuk a
 * valoszinuseget. */
double cesaro_valoszinuseg(void) {
    int i, db;
    
    db = 0;
    for (i = 1; i <= 1000; i += 1)
        if (cesaro_kiserlet())
            db += 1;
    
    return db/1000.0;   /* egesz osztas elkerulese */
}


/* mi a kiserlet? hogy ket veletlenszam legnagyobb kozos osztoja 1.
 * csinaljunk 1 es 1000 kozott veletlenszamokat. */
int cesaro_kiserlet(void) {
    return lnko(rand()%1000+1, rand()%1000+1)==1;
}


/* es mit jelent ket szam legnagyobb osztoja? puskazok a wikipediarol:
 * Euklidesz algoritmusa. */
int lnko(int a, int b) {
    while (b != 0) {
        int t = b;
        b = a%b;
        a = t;
    }
    return a;
}
