#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

void rendez(void *tomb,
            int meret, size_t elemmeret,
            int (*kisebb_e)(void const *pa, void const *pb))
{
   int i;
   void *temp;
   
   temp=malloc(elemmeret);
   
   for (i=0; i<meret-1; ++i) {
      void *pi=(char*)tomb + i*elemmeret;
      void *plk=pi;
      int j;
      
      for (j=i+1; j<meret; ++j) {
         void *pj=(char*)tomb + j*elemmeret;
         if (kisebb_e(pj, plk))
            plk=pj;
      }

      memcpy(temp, plk, elemmeret);    /* temp=lk */
      memcpy(plk, pi, elemmeret);   /* lk=eleje */
      memcpy(pi, temp, elemmeret); /* eleje=temp */
   }
   
   free(temp);
}

/* igaz, ha a<b */
int kisebb(void const *pva, void const *pvb) {
   double const *pa=(double const *)pva;
   double const *pb=(double const *)pvb;
   return *pa<*pb;
}
 
/* igaz, ha |a|<|b| */
int abszolut_kisebb(void const *pva, void const *pvb) {
   double const *pa=(double const *)pva;
   double const *pb=(double const *)pvb;
   return fabs(*pa)<fabs(*pb);
}
 
int main(void) {
   int i;
   double tomb[10]={1.2, 5.6, 9, -1.4, -6, 5, 9.1, 11, 0, -12};
 
   rendez(tomb, 10, sizeof(tomb[0]), kisebb);
   for (i=0; i<10; ++i) 
      printf("%7.2f", tomb[i]);
   printf("\n");
 
   rendez(tomb, 10, sizeof(tomb[0]), abszolut_kisebb);
   for (i=0; i<10; ++i) 
      printf("%7.2f", tomb[i]);
   printf("\n");
 
   return 0;
}
