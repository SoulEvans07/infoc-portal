#include <stdio.h>

/* - Egy tort szamlalobol es nevezobol all, mindketto egesz szam.
 * - Globalisan deklaraljuk, hogy minden fuggvenybol latszodjon.
 * - A struct Tort a typedef segitsegevel a Tort tipusnevet is kapja,
 * igy kesobb nem kell mindenhova kiirni, hogy struct Tort, eleg annyi,
 * hogy Tort.
 * - Ez egy nagyon tipikus hasznalata a typedefnek; a tort tipus
 * definicioja (struct Tort { ... }) egybe van dolgozva a typedeffel
 * (typedef reginev ujnev).
 */
typedef struct Tort {
   int szaml, nev;
} Tort;

/* Kiir a kepernyore egy tortet, es uj sort kezd.
 * A fuggvenynek nincs visszateresi erteke, csak megcsinalja,
 * amit kertek tole. */
void tort_kiir(Tort t) {
   printf("%d/%d\n", t.szaml, t.nev);
}

/* Kiszamolja egy tort valos erteket, vagyis pl. 1/3 eseten
 * visszater 0.333333-mal.
 * Az osztas elott az egyik szamot (jelen esetben a szamlalot,
 * bar lehetne a nevezot vagy mindkettot is) double tipusuva
 * kell alakitani, kulonben a fordito egesz osztast ertelmezne
 * (szaml/nev -> int/int), es az eredmenynek csak az egesz
 * reszet kapnank meg. Ha az osztas egyik tagja double, akkor
 * a masik tagot mar automatikusan azza alakitja, es igy
 * a helyes eredmenyt kapjuk. */
double tort_valos(Tort t) {
   return (double)t.szaml / t.nev;
}

/* Letrehozunk egy tortet.
 * Olyan strukturat adunk vissza, ami mar egyszerusitett
 * tortet tartalmaz!
 * Az euklideszi algoritmus - a wikipedian szepen ki van elemezve.
 * A hasznalatara a kovetkezo fuggveny mutat peldat. */
Tort tort_letrehoz(int szaml, int nev) {
   Tort uj;
   int a=szaml, b=nev;
   while (b!=0) {
      int t=b;
      b=a%b;
      a=t;
   }

   uj.szaml=szaml/a;
   uj.nev=nev/a;
   return uj;
}

/* Osszeadunk ket tortet, keletkezik egy uj, harmadik tort.
 * Mielott visszaadnank azt, egyszerusitjuk (a letrehoz fuggvennyel). */
Tort tort_osszead(Tort t1, Tort t2) {
   return tort_letrehoz(t1.szaml*t2.nev + t2.szaml*t1.nev, t1.nev*t2.nev);
}

/* Szamlalot szamlaloval, nevezot nevezovel. Ez az egysoros
 * kod mindent elvegez. A szorzas eredmenye egybol 
 * visszaadodik a hivonak. */
Tort tort_szoroz(Tort t1, Tort t2) {
   return tort_letrehoz(t1.szaml*t2.szaml, t1.nev*t2.nev);
}

int main(void) {
   Tort x = {2, 3};   /* 2/3 */
   Tort y, z;

   printf("x=");
   tort_kiir(x);      /* 2/3 jelenik meg, es egy uj sor */

   printf("x=%f\n", tort_valos(x));   /* 0.666667 */
   
   printf("y=50/100=");
   y = tort_letrehoz(50, 100);    /* 1/2 lesz az 50/100-bol */
   tort_kiir(y);
   
   printf("x*y=");
   tort_kiir(tort_szoroz(x, y));

   printf("x+y=");
   tort_kiir(tort_osszead(x, y));
   
   printf("x*y+1/6=");
   z = tort_osszead(tort_szoroz(x, y), tort_letrehoz(1, 6));
   tort_kiir(z);
   
   return 0;
}
