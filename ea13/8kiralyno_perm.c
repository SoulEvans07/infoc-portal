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

bool kiralyno_oke(int *poz, int meret) {
    for (int i = 0; i < meret-1; ++i)
        for (int j = i+1; j < meret; ++j)
            if (abs(poz[i] - poz[j]) == abs(i - j))
                return false;
    return true;
}

void csere(int *pi, int *pj) {
    int temp = *pi;
    *pi = *pj;
    *pj = temp;
}

void kiralyno_keres(int *poz, int meret, int honnan) {
    if (honnan == meret) {
        if (kiralyno_oke(poz, meret))
            kiralyno_kiir(poz, meret);
        return;
    }
    for (int i = honnan; i < meret; ++i) {
        csere(&poz[honnan], &poz[i]);
        kiralyno_keres(poz, meret, honnan+1);
        csere(&poz[honnan], &poz[i]);
    }
}

int main(void) {
    int poz[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    kiralyno_keres(poz, 8, 0);
    return 0;
}
