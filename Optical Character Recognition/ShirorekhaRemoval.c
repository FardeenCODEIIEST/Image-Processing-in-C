#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B_VAL 255
#define HEADER_WIDTH 2

typedef struct PGM
{
  char type[3];
  char **data;
  int width;
  int height;
  int grayvalue;
} PGM;

typedef struct kernel
{
  int size;
  int **info;
} kernel;

void ignoreComments(FILE *fp)
{
  int ch;
  char line[100];
  while ((ch = fgetc(fp)) != EOF && isspace(ch))
    ;
  if (ch == '#')
  {
    fgets(line, sizeof(line), fp);
    ignoreComments(fp);
  }
  else
  {
    fseek(fp, -1, SEEK_CUR);
  }
}

int openPGM(PGM *pgm, char filename[])
{
  FILE *pgmfile = fopen(filename, "rb");
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
  if (pgm->type[1] == '5')
  {
    for (int i = 0; i < pgm->height; i++)
    {
      pgm->data[i] = malloc(pgm->width *
                            sizeof(unsigned char));
      fread(pgm->data[i],
            pgm->width * sizeof(unsigned char),
            1, pgmfile);
    }
  }
  fclose(pgmfile);
  return 1;
}

void copyPGM(PGM *pdest, PGM *psrc)
{
  strcpy(pdest->type, psrc->type);
  pdest->width = psrc->width;
  pdest->height = psrc->height;
  pdest->grayvalue = psrc->grayvalue;
  for (int i = 0; i < pdest->height; i++)
  {
    for (int j = 0; j < pdest->width; j++)
    {
      pdest->data[i][j] = psrc->data[i][j];
    }
  }
}

void printImageDetails(PGM *pgm, char filename[])
{
  FILE *pgmfile = fopen(filename, "rb");
  char *ext = strrchr(filename, '.');
  if (!ext)
    printf("No extension found in file %s", filename);
  else
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

void init_ker(kernel *ker, int size)
{
  ker->size = size;
  ker->info = malloc(size * sizeof(int *));
  for (int i = 0; i < size; i++)
  {
    ker->info[i] = malloc(sizeof(int));
  }
  for (int i = 0; i < size; i++)
  {
    ker->info[i][0] = 1;
  }
}

void erosion1(PGM *pgm, PGM *pgm1, int i, int j, kernel *ker)
{
  int a = ker->size;
  for (int l = 0; l < a; l++)
  {
    if (pgm->data[i][j + l - a / 2] == (char)B_VAL)
    {
      pgm1->data[i][j] = (char)B_VAL;
      break;
    }
  }
}

void binaryPaint(PGM *pgm)
{
  for (int i = 0; i < pgm->height; i++)
  {
    for (int j = 0; j < pgm->width; j++)
    {
      if (pgm->data[i][j] != (char)0)
      {
        pgm->data[i][j] = (char)B_VAL;
      }
    }
  }
}

int cmp(const void *a, const void *b)
{
  return (*(int *)a - *(int *)b);
}

int header(PGM *pgm)
{
  int *arr = malloc(sizeof(int) * pgm->width);
  for (int j = 0; j < pgm->width; j++)
  {
    for (int i = 0; i < pgm->height; i++)
    {
      if (i == pgm->height / 2)
      {
        arr[j] = pgm->height / 2;
        break;
      }
      if (pgm->data[i][j] == (char)0)
      {
        arr[j] = i;
        break;
      }
    }
  }
  qsort(arr, pgm->width, sizeof(int), cmp);
  int a = arr[pgm->width / 3];
  free(arr);
  return a;
}

int letter_length(PGM *pgm)
{
  int a = header(pgm);
  int *arr = malloc(sizeof(int) * pgm->width);
  for (int j = 0; j < pgm->width; j++)
  {
    for (int i = a; i < pgm->height; i++)
    {
      if (i == pgm->height / 2)
      {
        arr[j] = pgm->height / 2;
        break;
      }
      if (pgm->data[i][j] != (char)0)
      {
        arr[j] = i;
        break;
      }
    }
  }
  qsort(arr, pgm->width, sizeof(int), cmp);
  int b = arr[pgm->width - 1];
  free(arr);
  return b - a;
}

int spacing(PGM *pgm)
{
  int a = header(pgm);
  int b = letter_length(pgm);
  int *arr = malloc(sizeof(int) * pgm->width);
  for (int j = 0; j < pgm->width; j++)
  {
    for (int i = a + b; i < pgm->height; i++)
    {
      if (i == pgm->height / 2)
      {
        arr[j] = pgm->height / 2;
        break;
      }
      if (pgm->data[i][j] != (char)B_VAL)
      {
        arr[j] = i;
        break;
      }
    }
  }
  qsort(arr, pgm->width, sizeof(int), cmp);
  int c = arr[pgm->width / 3];
  free(arr);
  return c - a;
}

int stroke(PGM *pgm)
{
  int a = header(pgm);
  int b = spacing(pgm);
  int m, k = 0;
  int *arr = malloc(sizeof(int) * 250);
  for (int i = a + 2; i < pgm->height - 1; i = i + b)
  {
    for (int j = 0; j < pgm->width - 1; j++)
    {
      if (pgm->data[i][j] == (char)B_VAL && pgm->data[i][j + 1] == (char)0)
      {
        m = 1;
        while (pgm->data[i][j + m] != (char)B_VAL)
        {
          m++;
        }
        arr[k++] = m;
      }
    }
  }
  qsort(arr, k, sizeof(int), cmp);
  int c = arr[k - 1];
  free(arr);
  return c;
}

void oper(PGM *pgm, PGM *pgm1, kernel *ker)
{
  int a = header(pgm);
  int b = spacing(pgm);
  int k;
  for (int i = a; i < pgm->height; i = i + b)
  {
    for (int j = 0; j < pgm->width; j++)
    {
      k = 0;
      while (k <= HEADER_WIDTH)
      {
        erosion1(pgm, pgm1, i + k, j, ker);
        k++;
      }
    }
  }
}

int main()
{
  PGM *pgm = malloc(sizeof(PGM));
  printf("Enter the PGM file name with extension\n");
  char ch[30];
  scanf("%s", ch);
  PGM *pgm1 = malloc(sizeof(PGM));
  kernel *ker = malloc(sizeof(kernel));
  if (openPGM(pgm, ch))
  {
    openPGM(pgm1, ch);
    printImageDetails(pgm, ch);
    binaryPaint(pgm); // To convert it to pure binary
    int c = stroke(pgm);
    init_ker(ker, 2 * c);
    oper(pgm, pgm1, ker); // Actual operation
    saveImage(pgm1, "ResultAfterShirorekhaRemoval.pgm");
    printf("Stoke width : %d px\n", c);
    free(ker->info);
    free(ker);
    free(pgm1->data);
    free(pgm1);
    free(pgm->data);
    free(pgm);
  }
  return 0;
}