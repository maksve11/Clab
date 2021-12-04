#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include "stb_image.h"
#include "stb_image_write.h"
#include <string.h>
#include <dir.h>

char *concat(char *s1, char *s2)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);
    if (!result)
    {
        fprintf(stderr, "Insufficient memory!\n");
        exit(1);
    }
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);
    return result;
}

int NumberOfNeighbors(int n, int m, unsigned char generation[n][m], int x, int y)
{
    int k = 0;
    if ((int)generation[(x - 1 + n) % n][(y + 1 + m) % m] == 0)
        k++;
    if ((int)generation[(x - 1 + n) % n][(y - 1 + m) % m] == 0)
        k++;
    if ((int)generation[(x + 1 + n) % n][(y - 1 + m) % m] == 0)
        k++;
    if ((int)generation[(x + 1 + n) % n][(y + 1 + m) % m] == 0)
        k++;
    if ((int)generation[(x - 1 + n) % n][y] == 0)
        k++;
    if ((int)generation[(x + 1 + n) % n][y] == 0)
        k++;
    if ((int)generation[x][(y - 1 + m) % m] == 0)
        k++;
    if ((int)generation[x][(y + 1 + m) % m] == 0)
        k++;
    return k;
}

int main(int argc, char *argv[])
{
    char *Image;
    char *dirrectory;
    int max_iter = 1000;
    int dump_freq = 1;

    // reading params
    if (argc <= 4)
    {
        printf("Correct usage: --input <filename> --output <filedir>\n--max_iter <> --dump_freq <>");
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "--input"))
        {
            if (i + 1 < argc)
            {
                Image = argv[i + 1];
            }
            else
            {
                printf("Wrong format");
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--output"))
        {
            if (i + 1 < argc)
            {
                dirrectory = argv[i + 1];
            }
            else
            {
                printf("Wrong format");
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--max_iter"))
        {
            if (i + 1 < argc)
            {
                max_iter = atoi(argv[i + 1]);
            }
            else
            {
                printf("Wrong format");
                return 1;
            }
        }
        else if (!strcmp(argv[i], "--dump_freq"))
        {
            if (i + 1 < argc)
            {
                dump_freq = atoi(argv[i + 1]);
            }
            else
            {
                printf("Wrong format");
                return 1;
            }
        }
    }
    mkdir(dirrectory);

    int witdth;
    int height;
    int channels;

    unsigned char *pixeldata = stbi_load((Image), &witdth, &height, &channels, 1);
    for (int jj = 0; jj < max_iter; jj++)
    {
        int k = 0;
        unsigned char matrix[height][witdth];
        for (int i = 0; i < witdth; i++)
        {
            for (int j = 0; j < height; j++)
            {
                matrix[i][j] = pixeldata[k];
                k++;
            }
        }
        int index = 0;
        for (int i = 0; i < witdth; i++)
        {
            for (int j = 0; j < height; j++)
            {
                int k = NumberOfNeighbors(height, witdth, matrix, i, j);
                if ((k == 3) && ((int)(pixeldata[index]) == 255))
                    pixeldata[index] = (unsigned char)(0);
                else if (k < 2 || k > 3)
                    pixeldata[index] = (unsigned char)(255);
                index++;
            }
        }
        if (jj % dump_freq == 0)
        {
            char buf[17];
            char *path = concat(dirrectory,"\\");
            path = concat(path, concat(itoa(jj, buf, 10), ".bmp"));
            stbi_write_bmp(path, witdth, height, 1, pixeldata);
        }
    }
    return 0;
}