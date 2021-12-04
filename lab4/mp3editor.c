#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>

typedef unsigned __int8 uint8_t;

typedef struct header // структура хеддера
{
    uint8_t idv3[3];
    uint8_t ver[2];
    uint8_t flag;
    uint8_t size[4];
} ID3header;

typedef struct frame // структура фрейма
{
    uint8_t frameid[4];
    uint8_t size[4];
    uint8_t flag[2];
    uint8_t unicode;
} ID3frame;

ID3header head;
ID3frame frame;

void inttobyte(int x, uint8_t *cc, bool flag)
{
    int bit = 127;
    if (head.ver[0] == 4 || flag)
    {
        cc[3] = x & bit;
        cc[2] = (x >> 7) & bit;
        cc[1] = (x >> 14) & bit;
        cc[0] = (x >> 21) & bit;
    }
    else
    {
        cc[3] = x & bit;
        cc[2] = (x >> 8) & bit;
        cc[1] = (x >> 16) & bit;
        cc[0] = (x >> 24) & bit;
    }
}

int bytetoint(uint8_t cc[4]) //байты в инт
{
    return (int)cc[0] << 21 | (int)cc[1] << 14 | (int)cc[2] << 7 | (int)cc[3] << 0;
}

int showgetid3(char *filename, uint8_t *name, int *oldsize, bool flag)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("No such file or directory");
        exit(1);
    }

    fread(&head, 1, 10, file); // считываем в хедер первые 10 байт, где хранится информация о теге

    int poswrite = 0;                // Позиция откуда записываем/считываем
    int temp = bytetoint(head.size); // размер тега

    while (fread(&frame, 1, 11, file)) //читаем фрейм 10 байтов + 1 байт перед строкой юникода
    {
        if ((ftell(file) >= temp || frame.frameid[0] == 0) && (name != NULL)) // Если первый символ в ID Frame == 0 или прочитали больше чем нужно
        {
            printf("No such frame\n");
            exit(1);
        }
        else if (ftell(file) >= temp || frame.frameid[0] == 0) // Если первый символ в ID Frame == 0 или прочитали больше чем нужно
        {
            break;
        }
        int temp1 = bytetoint(frame.size); //размера значения фрейма (длина строки)

        uint8_t *cc = (uint8_t *)calloc(temp1, 1);
        fread(cc, 1, temp1 - 1, file); // Запоминаем значение фрейма
        cc[temp1 - 1] = 0;             // обнуляем последний элемент значения фрейма

        if (name == NULL)
        {
            printf("id: %-5s value: ", frame.frameid, temp1);
            if (frame.unicode)
            {
                wprintf(L"%ls\n", cc + 2); // Выводим значение фрейма, в зависимости от юникода
            }
            else
            {
                printf("%s\n", cc);
            }
        }
        else if (!strcmp(frame.frameid, name))
        {
            if (flag == 0)
            {
                printf("id: %-5s value: ", frame.frameid); //вывод одного фрейма
                if (frame.unicode)
                {
                    wprintf(L"%ls\n", cc + 2);
                }
                else
                {
                    printf("%s\n", cc);
                }
            }
            else
            {
                *oldsize = temp1;                    // Запоминаем размер прошлого Frame, чтобы не создавать новый
                poswrite = ftell(file) - 10 - temp1; // позиция, до которой будем просматривать файл
                break;
            }
        }
        free(cc);
    }
    fclose(file);
    return poswrite;
}

void setid3(char *filename, uint8_t *name, uint8_t *value)
{
    FILE *file1 = fopen(filename, "rb");
    if (file1 == NULL)
    {
        printf("No such file or directory");
        exit(1);
    }
    FILE *file2 = fopen("temp", "wb"); //временный для записи

    int *oldsize = calloc(1, sizeof(int));
    int poswrite = showgetid3(filename, name, oldsize, 1);
    int sizediff = strlen(value) - (*oldsize - 1);
    uint8_t sizeframe[4];

    fread(&head, 1, 10, file1);      //читаем хедер 10 байтов
    int temp = bytetoint(head.size); //размер всей структуры ID3 включая хедер
    temp += sizediff;                //обновили размер структуры в соответствии с новым фреймом

    inttobyte(temp, head.size, 0);
    fwrite(&head, 1, 10, file2); //записали новый хедер

    uint8_t *cc = (uint8_t *)malloc(poswrite - 10);
    fread(cc, 1, poswrite - 10, file1);
    fwrite(cc, 1, poswrite - 10, file2); //в предыдущей строке прочитали, а в этой записали все байты до фрейма, теперь пишем фрейм
    free(cc);

    memcpy(frame.frameid, name, 4);

    int sizetag = bytetoint(head.size);
    sizetag += sizediff;

    inttobyte(sizetag, head.size, 0);
    inttobyte(strlen(value) + 1, sizeframe, 1);
    memcpy(frame.size, sizeframe, 4);

    fwrite(&frame, 1, 11, file2); //записали фрейм в следующей его значение
    fwrite(value, 1, strlen(value), file2);

    fseek(file1, 0, SEEK_END);

    int readoff = *oldsize ? poswrite + 10 + *oldsize : poswrite;
    int readsize = ftell(file1) - readoff;

    cc = (uint8_t *)malloc(readsize);
    fseek(file1, readoff, SEEK_SET);
    fread(cc, 1, readsize, file1);
    fwrite(cc, 1, readsize, file2);
    free(cc);

    fclose(file1);
    fclose(file2);
    remove(filename);
    rename("temp", filename);
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "Russian");
    uint8_t *filepath = NULL;
    uint8_t *name = NULL;
    uint8_t *value = NULL;
    bool show = 0, get = 0, set = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--show") == 0)
        {
            show = true;
            continue;
        }
        if (argv[i][2] == 'f')
        {
            filepath = strpbrk(argv[i], "=") + 1;
        }
        if (argv[i][2] == 'g')
        {
            get = true;
            name = strpbrk(argv[i], "=") + 1;
            continue;
        }
        if (argv[i][2] == 's')
        {
            set = true;
            name = strpbrk(argv[i], "=") + 1;
            continue;
        }
        if (argv[i][2] == 'v')
        {
            value = strpbrk(argv[i], "=") + 1;
            continue;
        }
    }
    if (show)
    {
        printf("RESULT:\n");
        showgetid3(filepath, NULL, NULL, 0);
    }
    else if (get)
    {
        printf("RESULT:\n");
        showgetid3(filepath, name, NULL, 0);
    }
    else if (set)
    {
        printf("SUCCESSFULLY!\n");
        setid3(filepath, name, value);
    }
    return 0;
}