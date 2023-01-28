// C program to read a BMP Image and
// write the same into a PGM Image file
#include <stdio.h>

void main()
{
  int i, j, temp = 0;
  int width = 13, height = 13;

  // Suppose the 2D Array to be converted to Image is as given below
  char image[13][13] = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, (char)255, (char)255, (char)255, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, (char)255, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, (char)255, (char)255, (char)255, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, (char)255, 0, 0, 0, 0, 0, (char)255, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, (char)255, (char)255, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (char)255, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (char)255, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

  FILE *pgmimg;
  pgmimg = fopen("pgmimg.pgm", "wb");

  // Writing Magic Number to the File
  fprintf(pgmimg, "P2\n");

  // Writing Width and Height
  fprintf(pgmimg, "%d %d\n", width, height);

  // Writing the maximum gray value
  fprintf(pgmimg, "255\n");
  int count = 0;
  for (i = 0; i < height; i++)
  {
    for (j = 0; j < width; j++)
    {
      temp = image[i][j];

      // Writing the gray values in the 2D array to the file
      fprintf(pgmimg, "%c ", temp);
    }
    fprintf(pgmimg, "\n");
  }
  fclose(pgmimg);
}
