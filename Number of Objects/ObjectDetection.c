#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_OBJ 25
#define B_VAL 255

typedef struct PGM
{
  char type[3];
  char **data;
  int width;
  int height;
  int grayvalue;
} PGM;
typedef struct dimensions
{
  int **space;
} dimensions;

// Function to ignore any comments in file
void ignoreComments(FILE *fp)
{
  int ch;
  char line[100];
  // Ignore any blank lines
  while ((ch = fgetc(fp)) != EOF && isspace(ch))
    ;
  // Recursively ignore comments in a PGM image commented lines
  // start with a '#'
  if (ch == '#')
  {
    fgets(line, sizeof(line), fp);
    // To get to next line
    ignoreComments(fp);
  }
  // check if anymore comments available
  else
  {
    fseek(fp, -1, SEEK_CUR);
  }
  // Beginning of current line
}

// Function to open the input a PGM file and process it
int openPGM(PGM *pgm, char filename[])
{
  FILE *pgmfile = fopen(filename, "rb");

  // If file does not exist, then return
  if (pgmfile == NULL)
  {
    printf("File does not exist\n");
    return 0;
  }
  ignoreComments(pgmfile);
  fscanf(pgmfile, "%s", pgm->type);
  ignoreComments(pgmfile);

  // Read the image dimensions
  fscanf(pgmfile, "%d %d", &(pgm->width),
         &(pgm->height));
  ignoreComments(pgmfile);

  // Read maximum gray value
  fscanf(pgmfile, "%d", &(pgm->grayvalue));
  ignoreComments(pgmfile);

  // Allocating memory to store img info in defined struct
  pgm->data = malloc(pgm->height * sizeof(unsigned char *));

  // Storing the pixel info in the struct
  if (pgm->type[1] == '5')
  {
    for (int i = 0; i < pgm->height; i++)
    {
      pgm->data[i] = malloc(pgm->width *
                            sizeof(unsigned char));
      // Read the gray values and write on allocated memory
      fread(pgm->data[i],
            pgm->width * sizeof(unsigned char),
            1, pgmfile);
    }
  }
  // Close the file
  fclose(pgmfile);
  return 1;
}

// Function to print the file details
void printImageDetails(PGM *pgm, char filename[])
{
  FILE *pgmfile = fopen(filename, "rb");

  // searches 1st occurrence of .
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

void box(PGM *pgm, int top, int bottom, int left, int right)
{
  for (int i = top; i <= bottom; i++)
  {
    pgm->data[i][left] = (char)B_VAL;
    pgm->data[i][right] = (char)B_VAL;
  }
  for (int i = left; i <= right; i++)
  {
    pgm->data[top][i] = (char)B_VAL;
    pgm->data[bottom][i] = (char)B_VAL;
  }
}
int right(PGM *pgm, int i, int j)
{
  int k = 0;
  while (j + k < pgm->width)
  {
    if (pgm->data[i][j + k] != (char)B_VAL)
    {
      break;
    }
    k++;
  }
  return j + k - 1;
}
int left(PGM *pgm, int i, int j)
{
  int k = 0;
  while (j - k >= 0)
  {
    if (pgm->data[i][j - k] != (char)B_VAL)
    {
      break;
    }
    k++;
  }
  return j - k + 1;
}
int dwn(PGM *pgm, int i, int j)
{
  int k = 0;
  while (i + k < pgm->height)
  {
    if (pgm->data[i + k][j] != (char)B_VAL)
    {
      break;
    }
    k++;
  }
  return i + k - 1;
}
int down(PGM *pgm, int i, int j)
{
  int k = 0, l = 0, down1 = 0, down2;
  while (j + k < pgm->width)
  {
    if (pgm->data[i][j + k] != (char)B_VAL)
    {
      break;
    }
    down2 = dwn(pgm, i, j + k);
    if (down2 > down1)
    {
      down1 = down2;
    }
    k++;
  }
  while (j - l >= 0)
  {
    if (pgm->data[i][j - l] != (char)B_VAL)
    {
      break;
    }
    down2 = dwn(pgm, i, j - l);
    if (down2 > down1)
    {
      down1 = down2;
    }
    l++;
  }
  return down1;
}
void detect(PGM *pgm, int i, int j, int l, dimensions *dim)
{
  dim->space[l][0] = i;
  int k = 0, left1 = 5000, right1 = 0, left2, right2, down1;
  while (i + k < pgm->height)
  {
    if (pgm->data[i + k][j] != (char)B_VAL)
    {
      break;
    }
    left2 = left(pgm, i + k, j);
    right2 = right(pgm, i + k, j);
    if (left2 < left1)
    {
      left1 = left2;
    }
    if (right2 > right1)
    {
      right1 = right2;
    }
    k++;
  }
  k--;
  down1 = down(pgm, i + k, j);
  dim->space[l][2] = left1;
  dim->space[l][3] = right1;
  dim->space[l][1] = down1;
}
int viable(PGM *pgm, int i, int j, dimensions *dim)
{
  if (pgm->data[i][j] != (char)B_VAL)
  {
    return 0;
  }
  for (int l = 0; l < MAX_OBJ; l++)
  {
    if (dim->space[l][1] == -1)
    {
      return 1;
    }
    if (i >= dim->space[l][0] && i <= dim->space[l][1])
    {
      if (j >= dim->space[l][2] && j <= dim->space[l][3])
      {
        return 0;
      }
    }
  }
  return 1;
}
dimensions *form()
{
  dimensions *dim = malloc(sizeof(dimensions));
  dim->space = malloc(sizeof(int *) * MAX_OBJ);
  for (int i = 0; i < MAX_OBJ; i++)
  {
    dim->space[i] = malloc(sizeof(int) * 4);
    dim->space[i][0] = 5000; // up
    dim->space[i][1] = -1;   // down
    dim->space[i][2] = 5000; // left
    dim->space[i][3] = -1;   // right
  }
  return dim;
}
void object(PGM *pgm, dimensions *dim)
{
  int l = 0;
  for (int i = 0; i < pgm->height; i++)
  {
    for (int j = 0; j < pgm->width; j++)
    {
      if (viable(pgm, i, j, dim))
      {
        detect(pgm, i, j, l, dim);
        l++;
        break;
      }
    }
  }
  for (int i = 0; i < MAX_OBJ; i++)
  {
    if (dim->space[i][1] == -1)
    {
      printf("\nNo of objects : %d\n", i);
      break;
    }
    box(pgm, dim->space[i][0], dim->space[i][1], dim->space[i][2], dim->space[i][3]);
  }
}
int main()
{
  PGM *pgm = malloc(sizeof(PGM));
  printf("Enter the PGM file name with extension\n");
  char ch[30];
  scanf("%s", ch);
  dimensions *dim = form();
  if (openPGM(pgm, ch))
  {
    printImageDetails(pgm, ch);
    object(pgm, dim);
    saveImage(pgm, "Image-Components.pgm");
    free(dim->space);
    free(dim);
    free(pgm->data);
    free(pgm);
  }
  return 0;
}