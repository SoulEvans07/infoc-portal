int a(char *mit)
{
    int i, j;

    j=0;
    for (i=0; mit[i]!=0; ++i) {
        if (mit[i]!=' ') {
            mit[j]=mit[i];
            j++;
        }
    }
    mit[j]=0;

    return 0;
}

int b(char *mit)
{
    char *cel=mit;
    while (*mit!=0) {
        if (*mit!=' ')
            *cel++=*mit;
        mit++;
    }
    *cel=0;

    return 0;
}

int c(char *mit)
{
    char *cel=mit;
    do {
        if (*mit!=' ')
            *cel++=*mit;
    } while (*mit++);

    return 0;
}

int d(char *m)
{
    char *c=m;
    do {
        if (*m-32)
            *c++=*m;
    } while (*m++);

    return 0;
}

e(char*m){char*c=m;do{if(*m-32)*c++=*m;}while(*m++);}

f(char*m){char*c=m;while(*c++=*m++)if(*(c-1)==32)c--;}

g(char*m){char*c=m;while(*c++=*m++)if(*--c!=32)c++;}

h(char*m){char*c=m;while(*c++=*m++)if(*--c-32)c++;}

i(char*m){char*c=m;while(*c++=*m++)*--c-32&&c++;}

j(char*m){char*c=m;while(*c=*m++)*c-32&&c++;}

k(char*m){for(char*c=m;*c=*m++;*c-32&&c++);}

l(char*m){for(char*c=m;*c=*m++;c+=*c!=32);}

#include <stdio.h>
#include <string.h>

int main()
{
    char eredeti[]=" Ez egy   proba  szoveg. ", szoveg[30];
    int (*fuggvenyek[])(char *)={a, b, c, d, e, f, g, h, i, j, k, l, 0};
    
    printf ("Eredeti: [%s]\n", eredeti);
    
    for (int i=0; fuggvenyek[i]!=0; ++i) {
        strcpy(szoveg, eredeti);
        fuggvenyek[i](szoveg);
        printf("%c. [%s]\n", 'a'+i, szoveg);
    }

    return 0;
}
