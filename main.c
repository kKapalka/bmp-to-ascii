#include "lib.h"
/**
* Program do dzialania potrzebuje zmiennych
* - odnosnika do pliku
* - tablicy dwuwymiarowej typu unsigned char, aby mogla przechowac natezenie koloru w skali szarosci
* - tablice 3-elementowa typu size_t, aby przechowala kolejno
* - sume dwoch pierwszych bajtow pliku (bajt 1 * 256 + bajt 2), szerokosc i wysokosc pliku
* - zmienna flagowa przechowujaca informacje o tym, czy alokacja tablicy przebiegla pomyslnie, oraz gdzie wystapil blad
**/
int main(int argc, char *argv[])
{

  FILE *image;
  unsigned char **pixelmap;
  size_t data[3]={0,0,0};
  int pixelflag;
  char *pathname;
  if(argc>1) pathname=argv[1];
  else assign_str(&pathname,"Podaj sciezke do pliku: ");
/*---------Otwarcie pliku -----------*/
  if(!(image=fopen((const char *)pathname,"r")))
  {
    abort("Blad otwarcia pliku - %s\n");
  }
/*--------Wyciagniecie istotnych informacji--------------*/
  get_data(data, image);
/*--------Alokacja tablicy-------------------*/
  if((pixelflag=init_pixelmap(&pixelmap,data))==-1)
  {
    abort("Blad podczas alokacji %s\n");
  }
/*-------Zapelnienie tablicy---------------*/
  fetch_pixelmap(image,pixelmap,data);
  fclose(image);
/*-------Zamkniecie pliku i wywietlenie tablicy na ekran-------------*/
  frender(stdout,pixelmap,data);
  FILE *out;
  if(out=fopen("test.txt","ab+")) frender(out,pixelmap,data);
  fclose(out);
/*-------Uwolnienie zasobow--------------------*/ 
  if(pixelflag==0) free_map(pixelmap,data);
  return 0;
}
