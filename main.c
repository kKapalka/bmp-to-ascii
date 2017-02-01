#include "lib.h"

int main(int argc, char *argv[])
{

  FILE *image;
  unsigned char **pixelmap;
  size_t data[4]={0,0,0,0};
  int flag;
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
  if((flag=init_pixelmap(&pixelmap,data))==-1)
  {
    abort("Blad podczas alokacji %s\n");
  }

/*-------Zapelnienie tablicy---------------*/
  fetch_pixelmap(image,pixelmap,data);
  fclose(image);
/*-------Zamkniecie pliku i wywietlenie tablicy na ekran-------------*/
  frender(stdout,pixelmap,data);
/*-------Zapisanie tablicy pixelmap do pliku--------------*/
  FILE *out;
  if(out=fopen("test.txt","ab+")) frender(out,pixelmap,data);
  fclose(out);
/*-------Uwolnienie zasobow--------------------*/ 
  if(flag==0) free_map(pixelmap,data);
  return 0;
}
