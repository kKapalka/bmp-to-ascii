#include "lib.h"


/**
* Funkcja assign_str przyjmuje odnosnik do tablicy charow i druga tablice reprezentujaca tekst
* Funkcjonuje podobnie do funkcji input() z jezyka python
* Wyswietla tekst bedacy zapytaniem, i czeka na wprowadzenie tekstu do klawiatury
* potem ten tekst przypisuje do odnosnika wprowadzonego jako pierwszy argument
**/

void assign_str(char **value, char *text)
{
  char temp_value[BUFSIZ];
  int i;
  fprintf(stdout,"%s",text);
  if(i=fscanf(stdin,"%s",temp_value))
  {
    *value=temp_value;
  }
  else *value=NULL;
}


/**
* funkcja get_data przyjmuje tablice mogaca przechowac duze integery typu unsigned i operuje na pliku
* tablica typu size_t docelowo ma przechowac dokladnie trzy zmienne - "checksume", szerokosc i wysokosc pliku graficznego
* Zczytuje informacje z tresci naglowkowej - z pierwszych dwoch bitow tworzy "checksume" za pomoca ktorej
* okresla typ pliku wprowadzonego do programu.
* Jezeli wykryje plik BMP przechodzi przez calosc tresci naglowkowej - 122 bajty - i po drodze zczytuje istotne informacje
* potrzebne do przetwarzania pliku. Szerokosc znajduje sie na bitach 18-21, wysokosc - na bitach 22-25
* te informacje sa zapisywane w miejscach kolejno data[1] i data[2]
* Funkcja umie wykryc ponadto pliki .jpg i pliki tekstowe zaczynajace sie od #include lub ENTER+#include
* i uznaje je jako program typu C/C++.
* W razie niepowodzenia w wykryciu rodzaju pliku wyswietla na ekran "checksume"
**/

void get_data(size_t *data,FILE *fp)
{
  short bytes;
  int i;
  data[0]=getc(fp)*256+getc(fp);
  if(data[0]==BMP_CHECK)
  {
    printf("Odczytano typ: BMP\n");
    int offset=get_offset(fp);
    for(i=15;i<offset;i++)
    {
	if(i==18)
	{
	  data[WIDTH]=assemble_bytes(fp,4);
	  i+=3;
	}
	else if(i==22)
	{
	  data[HEIGHT]=assemble_bytes(fp,4);
	  i+=3;
	}
	else if(i==28)
	{
	  data[BYTES_PER_PIXEL]=assemble_bytes(fp,2)/8;
	  i++;
	}
	else bytes=getc(fp);
    }
  }
  else if (data[0]==JPG_CHECK) printf("Odczytano typ: JPG\n");
  else if (data[0]==2595 || data[0]==9065) printf("Odczytano typ: Program w jezyku C/C++\n");
  else printf("Nie odczytano typu pliku. Zawartosc bufora: %lu\n",data[0]);
}


/**
* Funkcja assemble_bytes sprzega ze soba ilosc bajtow okreslona zmienna int size z tresci pliku
* Koniecznosc istnienia - szerokosc i wysokosc w pliku BMP zapisana jest w nastepujacy sposob
* Bajt 1 - Bajt 2 - Bajt 3 - Bajt 4
* czyli zczytanie tej wartosci jako integer nie da pozadanego efektu
* ponadto funkcja jest efektywna w zbieraniu niepotzebnych 
**/

int assemble_bytes(FILE *fp, int size)
{
  int i,buffer=0;
  for(i=0;i<size;i++) buffer+=getc(fp)<<(i*8);
  return buffer;
}


/**
* Funkcja init_pixelmap przyjmuje odnosnik do dwuwymiarowej tablicy charow oraz tablice wypelniona wartosciami z funkcji get_data
* Sprawdza, czy iloczyn szerokosci i wysokosci nie daje czasem zera
* dynamicznie tworzy tablice o rozmiarze [Wysokosc][Szerokosc]
* i przypisuje jej adres do odnosnika bedacego argumentem 1
* Jezeli alokacja przebiegla pomyslnie zwraca 0
* Blad w jakimkolwiek miejscu powoduje zwrocenie wartosci -1
**/

int init_pixelmap(unsigned char ***map, size_t *data)
{
  if(data[WIDTH] * data[HEIGHT])
  {
    unsigned char **tempmap = malloc(data[HEIGHT]*sizeof(unsigned char *));
    if(!tempmap)
    {
	return -1;
    }
    for(int i=0;i<data[HEIGHT];i++)
    {
      tempmap[i]=malloc(data[WIDTH]);
      if(!tempmap[i])
      {
	return -1;
      }
    }
    *map=tempmap;
  }
return 0;
}

/**
* Funkcja fetch_pixelmap operuje na pliku, przyjmuje dodatkowo jako argumenty tablice dwuwymiarowa
* inicjalizowana wewnatrz init_pixelmap() i tablice przechowujaca istotne informacje
* Funkcja na poczatku determinuje rozmiar szeregu, gdyz informacje odnosnie pikseli sa sprzegane w 4-bajtowe "dwordy"
* a nadmiar bajtow jest zapelniany zerami.
* Funkcja zapelnia dwuwymiarowa tablice srednia arytmetyczna trzech kolejnych bajtow pliku
* a jezeli napotka koniec szeregu, osobno zbiera nadmiarowe bajty
**/
void fetch_pixelmap(FILE *fp, unsigned char **map, size_t *data)
{
  if(!data[HEIGHT]) return;
  int row_size = ((data[WIDTH]*data[BYTE_PER_PIXEL]*8 + 31)/32)*4;
  int bytes;
  int fill_dword = row_size-(data[WIDTH]*data[BYTE_PER_PIXEL]);

  for(int i=data[HEIGHT]-1;i>=0;i--)
  {
    for(int j=0;j<data[WIDTH];j++)
    {
	map[i][j]=get_pixel(fp,data[BYTE_PER_PIXEL]);        
    }
    bytes=assemble_bytes(fp,fill_dword);
  }
}


/**
* funkcja frender przyjmuje odnosnik do pliku, tablice zapelniona wartosciami wewnatrz funkcji fetch_pixelmap, oraz istotne informacje
* Najpierw tworzy tablice reprezentujaca skale szarosci z zastosowaniem dostepnych dla systemu charakterow
* potem oblicza, jaki charakter ma byc wyswietlony na jakiej pozycji wzgledem tablicy poprzez
* przeskalowanie wartosci przez liczbe zmiennych, ktora tablica moze przechowac i pomnozenie przez rozmiar tablicy skali szarosci.
* Przykladowo - gdy map[i][j] przechowuje wartosc 30, to tablica wyswietli charakter bedacy na
* (30*70)/256 = 8.203125 ~ 8 pozycji, to jest '#' 
* Wewnatrz terminala tlo jest czarne, litery - biale, wiec znaki beda wybieranie od drugiej strony tablicy
* wiec wg. powyzszego przykladu wyswietli sie charakter z (70-1)-8.2 = 60.8 ~ 60 pozycji
* Przy skonczeniu szeregu przekazany bedzie znak \n i generowanie obrazu na ekran rozpocznie sie od nowej linii
**/

void frender(FILE *fp,unsigned char **map, size_t *data)
{
  char asciitab[70]="$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
  int i,j, display_char;
for(i=0;i<data[HEIGHT];i++)
  {
  
    for(j=0;j<data[WIDTH];j++)
    {
	if(fp==stdout)
	{
	  display_char=(sizeof(asciitab)-1)-(map[i][j]*(sizeof(asciitab)))/256;
	}
	else
	{
	  display_char=(map[i][j]*(sizeof(asciitab)))/256;
	}
	fprintf(fp,"%c",asciitab[display_char]);
    }
    fprintf(fp,"\n");
  }
}
/**
* funkcja free_map przyjmuje dwuwymiarowa tablice i istotne informacje
* Sluzy do uwolnienia zasobow. Po prostu.
**/
void free_map(unsigned char **map, size_t *data)
{
  for(int i=0;i<data[HEIGHT];i++)
  {
    free(map[i]);
  }
  free(map);
}

/**
* funkcja get_offset zczytuje z bajtow 14-18 informacje, na jak dalekiej odleglosci w bajtach znajduje sie bitmapa
**/
int get_offset(FILE *fp)
{
  int offset;
  for(int i=2;i<15;i++)
  {
    if(i==10)
    {
    offset=assemble_bytes(fp,4);
    i+=3;
    }
    else getc(fp);
  }
  return offset;
}
/**
* funkcja get_pixel zczytuje wartosci z bajtow formujacych pixel i zwraca ich srednia arytmetyczna
**/
int get_pixel(FILE *fp, int pixel_size)
{
  int pixel=0;
  for(int i=0;i<pixel_size;i++)pixel+=getc(fp);
  return pixel/pixel_size;
}
