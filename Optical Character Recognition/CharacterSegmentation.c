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

int letter_height(PGM *pgm)
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

int spacing(PGM *pgm, int let_len)
{
    int a = header(pgm);
    int b = let_len;
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

int uright(PGM *pgm, int i, int j, int a)
{
    if (pgm->data[i][j] == (char)B_VAL || i <= a)
    {
        return j;
    }
    int arr[] = {
        uright(pgm, i, j + 1, a),
        uright(pgm, i - 1, j, a),
    };
    qsort(arr, 2, sizeof(int), cmp);
    return arr[1];
}

int dright(PGM *pgm, int i, int j, int a)
{
    if (pgm->data[i][j] == (char)B_VAL || i <= a)
    {
        return j;
    }
    int arr[] = {
        dright(pgm, i, j + 1, a),
        dright(pgm, i + 1, j, a),
    };
    qsort(arr, 2, sizeof(int), cmp);
    return arr[1];
}

int max(int a, int b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

int right(PGM *pgm, int i, int j, int a)
{
    int x = uright(pgm, i, j, a);
    int y = dright(pgm, i, j, a);
    return max(x, y);
}

void blank(PGM *pgm, int i, int z)
{
    pgm->data[i][z + 1] = (char)B_VAL;
    pgm->data[i + 1][z + 1] = (char)B_VAL;
    pgm->data[i + 2][z + 1] = (char)B_VAL;
    pgm->data[i][z + 2] = (char)B_VAL;
    pgm->data[i + 1][z + 2] = (char)B_VAL;
    pgm->data[i + 2][z + 2] = (char)B_VAL;
    pgm->data[i][z + 3] = (char)B_VAL;
    pgm->data[i + 1][z + 3] = (char)B_VAL;
    pgm->data[i + 2][z + 3] = (char)B_VAL;
}

void oper1(PGM *pgm)
{
    int a = header(pgm);
    int b = letter_height(pgm);
    int c = spacing(pgm, b);
    int y, z, k;
    for (int i = a; i < pgm->height; i = i + c)
    {
        for (int j = 0; j < pgm->width; j++)
        {
            z = right(pgm, i + b / 4 + 3, j, i + 2);
            if (z > j + 1)
            {
                k = 0;
                for (int l = 3; l < 7; l++)
                {
                    if (pgm->data[i + b / 2 - 1][z + l] == (char)0)
                    {
                        y = right(pgm, i + b / 2 - 1, z + l, i + 2);
                        k = l;
                        if (y <= z + k + 5)
                        {
                            z = y;
                            blank(pgm, i, z);
                        }
                        else
                        {
                            blank(pgm, i, z);
                        }
                        break;
                    }
                }
                if (k == 0)
                {
                    blank(pgm, i, z + 1);
                }
            }
            j = z;
        }
    }
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
        binaryPaint(pgm);
        oper1(pgm);
        saveImage(pgm, "Result.pgm");
        free(pgm->data);
        free(pgm);
    }
    return 0;
}
