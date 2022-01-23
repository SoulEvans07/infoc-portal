/* Negy alapmuveletbol allo matematikai kifejezeseket kiertekelo
 * program. A megvalositas egy rekurziv alaszallo ertelmezo.
 * Reszletesebb magyarazat a 14. eloadas anyaganak elso feleben!
 * A program nem tokeletes; nincs felkeszitve minden fajta hibas
 * kifejezesre.
 */

#include <stdio.h>
#include <stdbool.h>


bool karakter(char **szoveg, char const *ertekek, char *karakter);
bool kifejezes(char **szoveg, int *ertek);
bool osszeg(char **szoveg, int *ertek);
bool pluszminusz_szorzat(char **szoveg, char *muvelet, int *ertek);
bool szam(char **szoveg, int *ertek);
bool szamjegy(char **szoveg, char *szamjegy);
bool szorzat(char **szoveg, int *ertek);
bool szorper_tenyezo(char **szoveg, char *muvelet, int *ertek);
bool tenyezo(char **szoveg, int *ertek);
bool whitespace(char **szoveg);
bool eos(char **szoveg);
bool zarojeles(char **szoveg, int *ertek);
bool egyetlen_kifejezes(char **szoveg, int *ertek);


int main(void) {
    char txt[500], *szoveg, *res;
    int szam;

    while (NULL != (res = fgets(txt, 500, stdin)) && txt[0] != '\n') {
        szoveg = txt;
        if (egyetlen_kifejezes(&szoveg, &szam)) {
            printf("Értéke: %d\n", szam);
        }
        else {
            printf("Nem sikerült értelmezni a szöveget.\n");
        }
    }

    return 0;
}


/* Azt nezi, hogy a megadott sztring egy kifejezest tartalmaz-e,
 * utana pedig vege van-e (nincs mar tobb karakter). Azert van ra
 * szukseg kulon, mert a kifejezes() nevu szabaly elfogad olyan sztringet
 * is, aminek az elejen kifejezes van, de a vegen van meg adat,
 * pl. "1 + 2 alma". */
bool egyetlen_kifejezes(char **szoveg, int *ertek) {
    char *ptxt = *szoveg;
    if (kifejezes(&ptxt, ertek) && whitespace(&ptxt) && eos(&ptxt)) {
        *szoveg = ptxt;
        return true;
    }
    return false;
}


/* A legfelso szabaly. Megnezi, a sztringben kifejezes
 * van-e; ha igen, akkor az erteket beteszi a valtozoba. */
bool kifejezes(char **szoveg, int *ertek) {
    return osszeg(szoveg, ertek);
}


/* Egy darab szamjegyet probal illeszteni. Ha sikerul, beteszi
 * a szamjegy nevu parameterbe. */
bool szamjegy(char **szoveg, char *szamjegy) {
    char *ptxt = *szoveg;
    if (*ptxt >= '0' &&  *ptxt <= '9') {
        *szamjegy = *ptxt;
        *szoveg += 1;
        return true;
    }

    return false;
}


/* Egy egesz tizes szamrendszerbeli szamot illeszt. Ha sikerul,
 * az erteket integerre alakitva az ertek nevu parameterbe teszi. */
bool szam(char **szoveg, int *ertek) {
    char *ptxt = *szoveg, c;
    whitespace(&ptxt);

    if (*ptxt == '0') {
        *ertek = 0;
        *szoveg += 1;
        return true;
    }

    if (szamjegy(&ptxt, &c)) {
        int val = c - '0';
        while (szamjegy(&ptxt, &c)) {
            val = val * 10 + (c - '0');
        }

        *ertek = val;
        *szoveg = ptxt;
        return true;
    }

    return false;
}


/* Egy karaktert probal meg illeszteni. A lehetseges karakterek
 * az ertekek nevu sztringben vannak. Ha sikerul az illesztes,
 * a megtalalt karakter a karakter nevu valtozoba kerul. */
bool karakter(char **szoveg, char const *ertekek, char *karakter) {
    for (int i = 0; ertekek[i] != '\0'; ++i) {
        if (**szoveg == ertekek[i]) {
            *karakter = **szoveg;
            *szoveg += 1;
            return true;
        }
    }

    return false;
}


/* Osszeget probal illeszteni. Ha sikerul, az ertek a parameterkent
 * kapott valtozoba kerul. */
bool osszeg(char **szoveg, int *ertek) {
    char *ptxt = *szoveg;

    int val;
    if (szorzat(&ptxt, &val)) {
        int tmp;
        char c;
        while (pluszminusz_szorzat(&ptxt, &c, &tmp)) {
            if (c == '+') val += tmp;
            else val -= tmp;
        }

        *ertek = val;
        *szoveg = ptxt;
        return true;
    }

    return false;
}


/* egy plusz vagy minusz jelet, utana pedig egy szorzatot illeszt.
 * ha sikeres, akkor a muveletbe teszi a muveleti jelet, az ertekbe
 * pedig a szorzas eredmenyet; egyeb esetben a ket parameter hivas
 * utani erteke definialatlan. */
bool pluszminusz_szorzat(char **szoveg, char *muvelet, int *ertek) {
    char *ptxt = *szoveg;

    if (whitespace(&ptxt) && karakter(&ptxt, "+-", muvelet) && szorzat(&ptxt, ertek)) {
        /* eddigre a karakter() a muveletbe, a szorzat() az ertekbe tette az illesztesek eredmenyeit */
        *szoveg = ptxt;
        return true;
    }
    return false;
}


/* Szorzatot probal illeszteni. Ha sikerul, az erteket beteszi a
 * parameterkent kapott valtozoba. */
bool szorzat(char **szoveg, int *ertek) {
    char *ptxt = *szoveg;

    int val;
    if (tenyezo(&ptxt, &val)) {
        int tmp;
        char c;

        while (szorper_tenyezo(&ptxt, &c, &tmp)) {
            if (c == '*') val *= tmp;
            else val /= tmp;
        }

        *ertek = val;
        *szoveg = ptxt;
        return true;
    }

    return false;
}


/* egy szorzas vagy osztas jelet, utana pedig egy tenyezot illeszt.
 * ha sikeres, akkor a muveletbe teszi a muveleti jelet, az ertekbe
 * pedig a szorzas eredmenyet; egyeb esetben a ket parameter hivas
 * utani erteke definialatlan. */
bool szorper_tenyezo(char **szoveg, char *muvelet, int *ertek) {
    char *ptxt = *szoveg;

    if (whitespace(&ptxt) && karakter(&ptxt, "*/", muvelet) && tenyezo(&ptxt, ertek)) {
        /* eddigre a karakter() a muveletbe, a tenyezo() az ertekbe tette az illesztesek eredmenyeit */
        *szoveg = ptxt;
        return true;
    }
    return false;
}


/* Tenyezot probal illeszteni. A tenyezo lehet egy szam,
 * vagy egy zarojeles kifejezes. Ha sikerult, erteket a
 * parameterkent kapott valtozoba teszi. */
bool tenyezo(char **szoveg, int *ertek) {
    char *ptxt = *szoveg;

    int val;
    if (szam(&ptxt, &val)) {
        *ertek = val;
        *szoveg = ptxt;
        return true;
    }

    if (zarojeles(&ptxt, &val)) {
        *ertek = val;
        *szoveg = ptxt;
        return true;
    }


    return false;
}


/* Zarojeles kifejezest probal illeszteni. */
bool zarojeles(char **szoveg, int *ertek) {
    char *ptxt = *szoveg, c;
    int val;

    if (whitespace(&ptxt) && karakter(&ptxt, "(", &c) && kifejezes(&ptxt, &val) && whitespace(&ptxt) && karakter(&ptxt, ")", &c)) {
        *ertek = val;
        *szoveg = ptxt;
        return true;
    }

    return false;
}


/* Szokozt vagy szokoz jellegu karaktert illeszt, es lepteti a pointert.
 * Mindig sikeres - ha nem talal szokozt, akkor is. Csak azert van, hogy
 * a kifejezesben tetszolegesen sok szokoz szerepelhessen. */
bool whitespace(char **szoveg) {
    char c;
    while (karakter(szoveg, " \t\n", &c));
    return true;
}


/* Sztring vegen vagyunk-e. */
bool eos(char **szoveg) {
    return **szoveg == '\0';
}
