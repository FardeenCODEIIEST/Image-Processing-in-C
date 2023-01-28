#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PGM
{
  char type[3];
  unsigned char **data;
  unsigned int width;
  unsigned int height;
  unsigned int grayvalue;
} PGM;

void ignoreComments(FILE *fp)
{
  int ch;
  char line[100];
  // Ignore any blank lines
  while ((ch = fgetc(fp)) != EOF && isspace(ch))
    ;
  // Recursively ignore comments in a PGM image
  // commented lines start with a '#'
  if (ch == '#')
  {
    fgets(line, sizeof(line), fp);
    // To get cursor to next line
    ignoreComments(fp);
  }
  // check if anymore comments available
  else
  {
    fseek(fp, -1, SEEK_CUR);
  }
  // Beginning of current line
}

int openPGM(PGM *pgm, char fname[])
{
  FILE *pgmfile = fopen(fname, "rb");
  if (pgmfile == NULL)
  {
    printf("File does not exist\n");
    return 0;
  }

  ignoreComments(pgmfile);
  fscanf(pgmfile, "%s", pgm->type);
  ignoreComments(pgmfile);

  fscanf(pgmfile, "%d %d", &(pgm->width),
         &(pgm->height));
  ignoreComments(pgmfile);

  fscanf(pgmfile, "%d", &(pgm->grayvalue));
  ignoreComments(pgmfile);

  pgm->data = malloc(pgm->height * sizeof(unsigned char *));
  for (int i = 0; i < pgm->height; i++)
  {
    pgm->data[i] = malloc(pgm->width *
                          sizeof(unsigned char));
    fread(pgm->data[i], pgm->width * sizeof(unsigned char), 1, pgmfile);
  }

  fclose(pgmfile);
  return 1;
}

void printImageDetails(PGM *pgm, char filename[])
{
  FILE *pgmfile = fopen(filename, "rb");

  // searches the occurrence of '.'
  char *ext = strrchr(filename, '.');

  if (!ext)
    printf("No extension found in file %s", filename);
  else // portion after .
    printf("File format     : %s\n", ext + 1);

  printf("PGM File type   : %s\n", pgm->type);
  printf("Width of img    : %d px\n", pgm->width);
  printf("Height of img   : %d px\n", pgm->height);
  printf("Max Gray value  : %d\n", pgm->grayvalue);

  fclose(pgmfile);
}
void saveImage(PGM *pgm, char fname[])
{
  FILE *fp = fopen(fname, "wb");

  fprintf(fp, "%s\n", pgm->type);
  fprintf(fp, "%d %d\n", pgm->width, pgm->height);
  fprintf(fp, "%d\n", pgm->grayvalue);

  for (int i = 0; i < pgm->height; i++)
  {
    for (int j = 0; j < pgm->width; j++)
    {
      fprintf(fp, "%c", pgm->data[i][j]);
    }
  }

  fclose(fp);
}

PGM *filter(PGM *pgm, int filt_size)
{
  PGM *pgm1 = malloc(sizeof(PGM));
  int s = filt_size / 2;

  strcpy(pgm1->type, pgm->type);
  pgm1->height = pgm->height;
  pgm1->width = pgm->width;
  pgm1->grayvalue = pgm->grayvalue;
  pgm1->data = malloc(pgm->height * sizeof(unsigned char *));
  for (int i = 0; i < pgm->height; i++)
  {
    pgm1->data[i] = malloc(pgm->width *
                           sizeof(unsigned char));
  }
  for (int i = 0; i < pgm->height; i++)
  {
    for (int j = 0; j < pgm->width; j++)
    {
      int d = 0;
      int e = 0;
      for (int k = -s; k <= s; k++)
      {
        for (int l = -s; l <= s; l++)
        {
          if (i + k > 0 && j + l > 0 && i + k < pgm->height && j + l < pgm->width)
          {
            d++;
            e = e + (int)pgm->data[i + k][j + l];
          }
        }
      }
      e = e / d;
      pgm1->data[i][j] = (char)e;
    }
  }
  return pgm1;
}

int main()
{
  PGM *pgm = malloc(sizeof(PGM));
  printf("Enter the PGM file name with extension\n");
  char ch[30];
  scanf("%s", ch);
  if (openPGM(pgm, ch))
  {
    printImageDetails(pgm, ch);
    int choice;
    printf("Enter the size of the filter window\n");
    scanf("%d", &choice);
    if (choice && 1)
    {
      PGM *filtered = filter(pgm, choice);
      saveImage(filtered, "filtered.pgm");
      printf("The image file has been filtered\n");
      free(filtered->data);
      free(filtered);
    }
    else
    {
      printf("Wrong size for the filter window\n");
      exit(EXIT_FAILURE);
    }
  }
  free(pgm->data);
  free(pgm);
  return 0;
}
