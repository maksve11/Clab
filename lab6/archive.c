#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct Settings
{
    char *archName;
    int create;
    int extract;
    int list;
    int filecount;
    char *fileNames[1000];
} Settings;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct File
{
    int nameSize;
    char *name;
    int dataSize;
    char *data;
} File;
#pragma pack(pop)

File *readFile(char *name)
{
    File *file = (File *)malloc(sizeof(File));
    file->nameSize = strlen(name) + 1;
    file->name = (char *)malloc(sizeof(char) * file->nameSize);
    strcpy(file->name, name);
    FILE *input = fopen(name, "r");
    fseek(input, 0, SEEK_END);
    file->dataSize = ftell(input);
    fseek(input, 0, SEEK_SET);
    file->data = (char *)malloc(sizeof(char) * file->dataSize);
    fread(file->data, file->dataSize, 1, input);
    fclose(input);
    return file;
}

void writeFile(File *file)
{
    FILE *output = fopen(file->name, "w");
    fwrite(file->data, file->dataSize, 1, output);
    fclose(output);
}

#pragma pack(push, 1)
typedef struct Arch
{
    int fileCount;
    File **files;
} Arch;
#pragma pack(pop)

Arch *readArch(Settings *settings)
{
    FILE *input = fopen(settings->archName, "r");
    Arch *arch = (Arch *)malloc(sizeof(Arch));
    fread(&arch->fileCount, sizeof(arch->fileCount), 1, input);
    arch->files = (File **)malloc(sizeof(File *) * arch->fileCount);
    for (int i = 0; i < arch->fileCount; i++)
    {
        arch->files[i] = (File *)malloc(sizeof(File));
        fread(&arch->files[i]->nameSize, sizeof(arch->files[i]->nameSize), 1, input);
        arch->files[i]->name = (char *)malloc(sizeof(char) * arch->files[i]->nameSize);
        fread(arch->files[i]->name, arch->files[i]->nameSize, 1, input);
        fread(&arch->files[i]->dataSize, sizeof(arch->files[i]->dataSize), 1, input);
        arch->files[i]->data = (char *)malloc(sizeof(char) * arch->files[i]->dataSize);
        fread(arch->files[i]->data, arch->files[i]->dataSize, 1, input);
    }
    return arch;
}

void create(Settings *settings)
{
    Arch *arch = malloc(sizeof(Arch));
    arch->fileCount = settings->filecount;
    arch->files = malloc(sizeof(File *) * arch->fileCount);
    for (int i = 0; i < arch->fileCount; i++)
    {
        arch->files[i] = malloc(sizeof(File));
        arch->files[i] = readFile(settings->fileNames[i]);
    }
    FILE *output = fopen(settings->archName, "w");
    fwrite(&arch->fileCount, sizeof(arch->fileCount), 1, output);
    for (int i = 0; i < arch->fileCount; i++)
    {
        fwrite(&arch->files[i]->nameSize, sizeof(arch->files[i]->nameSize), 1, output);
        fwrite(arch->files[i]->name, arch->files[i]->nameSize, 1, output);
        fwrite(&arch->files[i]->dataSize, sizeof(arch->files[i]->dataSize), 1, output);
        fwrite(arch->files[i]->data, arch->files[i]->dataSize, 1, output);
    }
    fclose(output);
}

void extract(Settings *settings)
{
    Arch *arch = readArch(settings);
    for (int i = 0; i < arch->fileCount; i++)
    {
        writeFile(arch->files[i]);
    }
}

void list(Settings *settings)
{
    Arch *arch = readArch(settings);
    for (int i = 0; i < arch->fileCount; i++)
    {
        printf("%s ", arch->files[i]->name);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    Settings *settings = (Settings *)malloc(sizeof(Settings));

    if (argc <= 3)
    {
        printf("Correct usage: --file <filename.arc> --create <files>\n--file <filename.arc> --list\n--file <filename.arc> --extract");
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--file") == 0)
        {
            settings->archName = (char *)malloc(sizeof(char) * strlen(argv[++i]));
            strcpy(settings->archName, argv[i]);
        }
        else if (strcmp(argv[i], "--create") == 0)
        {
            settings->create = 1;
        }
        else if (strcmp(argv[i], "--extract") == 0)
        {
            settings->extract = 1;
        }
        else if (strcmp(argv[i], "--list") == 0)
        {
            settings->list = 1;
        }
        else
        {
            settings->fileNames[settings->filecount] = (char *)malloc(sizeof(char) * strlen(argv[i]));
            strcpy(settings->fileNames[settings->filecount++], argv[i]);
        }
    }

    if (settings->create == 1)
    {
        create(settings);
    }
    else if (settings->extract == 1)
    {
        extract(settings);
    }
    else if (settings->list == 1)
    {
        list(settings);
    }
}