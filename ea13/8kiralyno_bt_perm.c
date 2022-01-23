#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

void kiralyno_kiir(int *poz, int meret) {
    for (int i = 0; i < meret; ++i) {
        for (int j = 0; j < meret; ++j)
            printf("%c", poz[i] == j+1 ? 'O' : '.');
        printf("\n");
    }
    printf("\n\n");
}

bool kiralyno_nedik_oke(int *poz, int n) {
    for (int i = 0; i < n; ++i) {
        if (abs(poz[i] - poz[n]) == abs(i - n))
            return false;
    }
    return true;
}

void csere(int *pa, int *pb) {
    int temp = *pa;
    *pa = *pb;
    *pb = temp;
}

void kiralyno_keres(int *poz, int meret, int n) {
    if (!kiralyno_nedik_oke(poz, n-1)) {
        return;
    }
    if (n == meret) {
        kiralyno_kiir(poz, meret);
        return;
    }
    for (int i = n; i < meret; ++i) {
        csere(&poz[n], &poz[i]);
        kiralyno_keres(poz, meret, n+1);
        csere(&poz[n], &poz[i]);
    }
}

int main(void) {
    int poz[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    kiralyno_keres(poz, 8, 0);
}
