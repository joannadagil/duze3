#include <stdio.h>
#include <stdlib.h>


typedef struct Osoba {
    union{
        int wiek;
        int rok;
    };
    int wiek0rok1;
    int *arr;
    int arr_lenght;
} Osoba;

int main(){
    Osoba *osoba1;
    osoba1 = malloc(sizeof(struct Osoba));
    osoba1->wiek = 15;
    osoba1->arr_lenght = 0;
    osoba1->wiek0rok1 = 0;

    int arr[2] = {1,2};
    Osoba osoba2;
    osoba2.rok = 1996;
    osoba2.wiek0rok1 = 1;
    osoba2.arr = arr;
    osoba2.arr_lenght = 2;

    printf("%d, %d\n", osoba2.arr[0], osoba2.arr[1]);
    printf("%d\n", osoba1->wiek);


    return 0;
}