#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
int fsize(FILE *file) // file size
{
    int pred = ftell(file);
    fseek(file, 0L, SEEK_END);
    int sz = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return sz;
}
int countl(FILE *file) //col-vo strok
{
    int kk = 0;
    char str1[1000];
    while (fgets(str1, 1000, file) != NULL)
    {
        int lk = strlen(str1);
        if (str1[lk - 1] == '\n')
            kk++;
    }
    int l1 = strlen(str1);
    if (l1 != 0)
    {
        if (str1[l1 - 1] != '\n')
            kk++;
    }
    else
        kk = 0;
    return kk;
}
int countw(FILE *file) // col-vo words
{
    int cc = 0;
    fseek(file, 0L, SEEK_SET);
    int n;
    bool flag;
    while ((n = fgetc(file)) != EOF)
    {
        if (isspace(n))
            flag = 0;
        else if (flag == 0)
        {
            cc++;
            flag = 1;
        }
    }
    return cc;
}
int main(int argc, char *argv[])
{
    char fname[1000];
    FILE *wordc;
    int p;
    bool line = 0, bytes = 0, word = 0, fileen = 0;
    if (argc == 1)
    {
        printf("Введены неправильные аргументы");
        return 1;
    }
    for (int i = 1; i < argc; i++)
    {
        if ((!strcmp(argv[i], "-l")) || (!strcmp(argv[i], "--lines")) && (!line))
            line = 1;
        else if ((!strcmp(argv[i], "-c")) || (!strcmp(argv[i], "--bytes")) && (!bytes))
            bytes = 1;
        else if ((!strcmp(argv[i], "-w")) || (!strcmp(argv[i], "--words")) && (!word))
            word = 1;
        else
        {
            //strcpy(fname, argv[i]);
            p = i;
            if (i != (argc - 1))
            {
                printf("Введены неправильные аргументы");
                return 1;
            }
            wordc = fopen(argv[p], "r");
            if (!wordc)
            {
                printf("Введены неправильные аргументы");
                return 1;
            }
            fclose(wordc);
            fileen = 1;
            break;
        }
    }
    if (!fileen)
    {
        printf("Введены неправильные аргументы");
        return 1;
    }
    wordc = fopen(argv[p], "r");
    if (line)
        printf("Количество строк в файле: %d\n", countl(wordc));
    if (bytes)
        printf("Размер файла в байтах: %d\n", fsize(wordc));
    if (word)
        printf("Количество слов в файле: %d\n", countw(wordc));
    fclose(wordc);
    return 0;
}