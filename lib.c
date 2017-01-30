#include "lib.h"

void assign_str(char **value, char *text)
{
  char temp_value[BUFSIZ];
  int i;
  fprintf(stdout,"%s",text);
  if(i=fscanf(stdin,"%s",temp_value)) *value=temp_value;
  else *value=NULL;
}

void get_data(size_t *data,FILE *fp)
{
  short bytes;
  data[0]=getc(fp)*256+getc(fp);
  if(data[0]==BMP_CHECK)
  {

    printf("Odczytano typ: BMP\n");
    for(int i=2;i<122;i++)
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
	else bytes=getc(fp);
    }
  }
  else if (data[0]==JPG_CHECK) printf("Odczytano typ: JPG\n");
  else if (data[0]==2595 || data[0]==9065) printf("Odczytano typ: Program w jezyku C/C++\n"); // ENTER + #include lub #include
  else printf("Nie odczytano typu pliku. Zawartosc bufora: %lu\n",data[0]);
}

int assemble_bytes(FILE *fp, int size)
{
  int buffer=getc(fp);
  for(int i=1;i<size;i++) buffer+=getc(fp)<<(i*8);
  return buffer;
}

int init_pixelmap(unsigned char ***map, size_t *data)
{
  if(data[WIDTH] * data[HEIGHT])
  {
    unsigned char **tempmap = malloc(data[HEIGHT]*sizeof(unsigned char *));
    if(!tempmap) return -1;
    for(int i=0;i<data[HEIGHT];i++){
      tempmap[i]=malloc(data[WIDTH]);
      if(!tempmap[i]) return -1;
    }
    *map=tempmap;
  }
return 0;
}

void fetch_pixelmap(FILE *fp, unsigned char **map, size_t *data)
{
  if(!data[HEIGHT]) return;
  int row_size = ((data[WIDTH]*24 + 31)/32)*4;
  int bytes;
  int fill_dword = row_size-(data[WIDTH]*3);
  printf("row_size: %d\n",row_size);

  for(int i=data[HEIGHT]-1;i>=0;i--)
  {
    for(int j=0;j<data[WIDTH];j++)
    {
	map[i][j]=(getc(fp)+getc(fp)+getc(fp))/3;        
    }
    if(fill_dword)bytes=assemble_bytes(fp,fill_dword);
  }
}

void frender(FILE *fp,unsigned char **map, size_t *data)
{
//char asciitab=" .*+|icwIEXQ38&#@";
char asciitab[70]="$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
  int i,j, display_char;
for(i=0;i<data[HEIGHT];i++)
  {
  
    for(j=0;j<data[WIDTH];j++)
    {
	if(fp==stdout) display_char=(sizeof(asciitab)-1)-(map[i][j]*(sizeof(asciitab)))/256;
	else display_char=(map[i][j]*(sizeof(asciitab)))/256;
	fprintf(fp,"%c",asciitab[display_char]);
    }
    fprintf(fp,"\n");
  }
}

void free_map(unsigned char **map, size_t *data)
{
  for(int i=0;i<data[HEIGHT];i++) free(map[i]);
  free(map);
}
