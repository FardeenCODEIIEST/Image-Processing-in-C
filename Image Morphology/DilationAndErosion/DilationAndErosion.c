#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B_VAL 255

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
    ker->info[i] = malloc(size * sizeof(int));
  }
}
int dilation(PGM *pgm, PGM *pgm1, int i, int j, kernel *ker)
{
  static int c = 0;
  int k, l, m = ker->size / 2;
  for (k = 0; k < ker->size; k++)
  {
    for (l = 0; l < ker->size; l++)
    {
      if (i + k - m >= 0 && i + k - m < pgm->height && j + l - m >= 0 && j + l - m < pgm->width) // Checking the edge cases
      {
        if (ker->info[k][l] == 1)
        {
          pgm1->data[i + k - m][j + l - m] = (char)B_VAL;
          c++;
        }
      }
    }
  }
  return c;
}

int erosion(PGM *pgm, PGM *pgm1, int i, int j, kernel *ker)
{
  static int c = 0;
  int k, l, m = ker->size / 2;
  for (k = 0; k < ker->size; k++)
  {
    for (l = 0; l < ker->size; l++)
    {
      if (i + k - m >= 0 && i + k - m < pgm->height && j + l - m >= 0 && j + l - m < pgm->width) // checking the edge cases
      {
        if (ker->info[k][l] == 1 && pgm->data[i + k - m][j + l - m] != (char)B_VAL)
        {
          pgm1->data[i][j] = (char)0;
          c++;
        }
      }
    }
  }
  return c;
}

int operation(PGM *pgm, PGM *pgm1, kernel *ker, int s)
{
  int ans;
  int i, j;
  for (i = 0; i < pgm->height; i++)
  {
    for (j = 0; j < pgm->width; j++)
    {
      if (pgm->data[i][j] == (char)B_VAL)
      {
        if (s == 0)
          ans = dilation(pgm, pgm1, i, j, ker);
        else if (s == 1)
          ans = erosion(pgm, pgm1, i, j, ker);
      }
    }
  }
  return ans;
}

int main()
{
  PGM *pgm = malloc(sizeof(PGM));
  PGM *pgm1 = malloc(sizeof(PGM));
  printf("Enter the PGM file name with extension\n");
  char ch[60];
  scanf("%s", ch);
  kernel *ker = malloc(sizeof(kernel));
  int size, s;
  int count = 0;
  printf("Enter size of kernel : \n");
  scanf("%d", &size);
  init_ker(ker, size);
  printf("Enter the data values in the kernel, enter 0 or 1\n");
  for (int i = 0; i < size; i++)
  {
    for (int j = 0; j < size; j++)
    {
      int b;
      scanf("%d", &b);
      ker->info[i][j] = b;
    }
  }
  printf("Enter 0 for dilation, 1 for erosion :\n");
  scanf("%d", &s);
  if (openPGM(pgm, ch))
  {
    openPGM(pgm1, ch);
    printImageDetails(pgm, ch);
    count = operation(pgm, pgm1, ker, s);
    if (s)
    {
      saveImage(pgm1, "Eroded-Image.pgm");
      printf("Number of pixels eroded is %d\n", count);
      printf("Image has been successfully eroded\n");
    }
    else
    {
      saveImage(pgm1, "Dilated-Image.pgm");
      printf("Number of pixels increased is %d\n", count);
      printf("Image has been successfully dilated\n");
    }
    free(pgm->data);
    free(pgm);
    free(pgm1->data);
    free(pgm1);
    free(ker);
  }
  return 0;
}