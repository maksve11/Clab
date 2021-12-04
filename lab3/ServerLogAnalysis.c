#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

char *readl(FILE *inputfile, int symbolcount, bool *lineIsLast)
{
    int symbol = getc(inputfile);
    char *line;

    if (symbol != '\n' && symbol != EOF)
    {
        line = readl(inputfile, symbolcount + 1, lineIsLast);
        line[symbolcount] = symbol;
    }
    else
    {
        line = (char *)malloc(symbolcount + 1);
        line[symbolcount] = '\0';
        if (symbol == EOF)
            *lineIsLast = true;
        else
            *lineIsLast = false;
    }
    return line;
}

bool parsestring(FILE *inputfile, char **time, char **req, char **status)
{
    bool lineIsLast;
    char *line = readl(inputfile, 0, &lineIsLast);
    strtok(line, "["); // разбивает строку на части по указанному разделителю

    *time = strtok(NULL, "]");
    strtok(NULL, "\"");

    *req = strtok(NULL, "\"");
    *status = strtok(NULL, " ");

    return lineIsLast;
}

int parsetime(char *strtime)
{
    struct tm time; // из библиотеки time.h
    char *ch;

    ch = strtok(strtime, "/: ");
    time.tm_mday = atoi(ch);

    ch = strtok(NULL, "/: ");
    if (strcmp(ch, "Jan") == 0)
        time.tm_mon = 0;
    else if (strcmp(ch, "Jan") == 0) // сравнение двух строк 0-идентичны
        time.tm_mon = 0;
    else if (strcmp(ch, "Feb") == 0)
        time.tm_mon = 1;
    else if (strcmp(ch, "Mar") == 0)
        time.tm_mon = 2;
    else if (strcmp(ch, "Apr") == 0)
        time.tm_mon = 3;
    else if (strcmp(ch, "May") == 0)
        time.tm_mon = 4;
    else if (strcmp(ch, "Jun") == 0)
        time.tm_mon = 5;
    else if (strcmp(ch, "Jul") == 0)
        time.tm_mon = 6;
    else if (strcmp(ch, "Aug") == 0)
        time.tm_mon = 7;
    else if (strcmp(ch, "Sep") == 0)
        time.tm_mon = 8;
    else if (strcmp(ch, "Oct") == 0)
        time.tm_mon = 9;
    else if (strcmp(ch, "Nov") == 0)
        time.tm_mon = 10;
    else if (strcmp(ch, "Dec") == 0)
        time.tm_mon = 11;
    ch = strtok(NULL, "/: ");
    time.tm_year = atoi(ch) - 1900;

    ch = strtok(NULL, "/: ");
    time.tm_hour = atoi(ch);

    ch = strtok(NULL, "/: ");
    time.tm_min = atoi(ch);

    ch = strtok(NULL, "/: ");
    time.tm_sec = atoi(ch);

    return mktime(&time); // преобразует структуру в секунды
}

typedef struct timeel
{
    int time;
    int index;
    struct timeel *next;
    struct timeel *prev;
} timeel;

typedef struct timelist
{
    timeel *first;
    timeel *last;
} timelist;

void push(timelist *tl, int timeval, int index)
{
    timeel *el = (timeel *)malloc(sizeof(timeel));
    el->time = timeval;
    el->index = index;
    el->prev = NULL;
    el->next = NULL;
    if (tl->first == NULL)
        tl->last = el;
    else
    {
        el->next = tl->first;
        tl->first->prev = el;
    }
    tl->first = el;
}

void pop(timelist *tl)
{
    timeel *el = tl->last;
    if (el != NULL)
    {
        if (el->prev != NULL)
        {
            tl->last = el->prev;
            tl->last->next = NULL;
        }
        else
        {
            tl->first = NULL;
            tl->last = NULL;
        }
        free(el);
    }
}

int seconds(char *periodType)
{
    if (strcmp(periodType, "sec") == 0)
        return 1;
    else if (strcmp(periodType, "min") == 0)
        return 60;
    else if (strcmp(periodType, "hour") == 0)
        return 60 * 60;
    else if (strcmp(periodType, "day") == 0)
        return 60 * 60 * 24;
    else
        return 0;
}

int period()
{
    int per = 0;
    char str[100];
    scanf("%99[^\n]", str);

    char *ch;
    int value, cc;
    bool firstword = true;

    while (true)
    {
        if (firstword)
        {
            ch = strtok(str, " ");
            firstword = false;
        }
        else
            ch = strtok(NULL, " ");
        if (ch == NULL)
            break;
        value = atoi(ch);
        if (value == 0)
            return 0;

        ch = strtok(NULL, " ");
        if (ch == NULL)
            return 0;
        cc = seconds(ch);
        if (cc == 0)
            return 0;
        per += value * cc;
    }
    return per;
}

int main()
{
    FILE *file;
    file = fopen("access_log_Jul95", "r");

    printf("Input period, ex: 1 day 12 hour 13 min: ");
    int per = period();
    if (per <= 0)
    {
        printf("ERROR!\n");
        return 1;
    }

    char *time;
    char *req;
    char *status;

    timelist timel = {NULL, NULL};
    int maxrez = 0;
    int begin;
    int end;

    int fatalreq = 0;
    bool laststr = false;
    printf("\nFailed requests:\n");
    for (int countl = 0; !laststr; countl++)
    {
        laststr = parsestring(file, &time, &req, &status);
        if (time != NULL)
        {
            int tt = parsetime(time);
            push(&timel, tt, countl);

            while (tt - timel.last->time > per)
                pop(&timel);
            int requestsInPeriod = countl - timel.last->index + 1;
            if (requestsInPeriod > maxrez)
            {
                maxrez = requestsInPeriod;
                begin = timel.last->time;
                end = tt;
            }
        }
        if (status != NULL && status[0] == '5')
        {
            printf("%s\n", req);
            fatalreq++;
        }
    }
    printf("\nCount of failed requests: %d", fatalreq);
    printf("\n");
    printf("\nTime per window with the maximum number of requests: ");
    time_t nicetime=begin;
    struct tm* nice=localtime(&nicetime); // переводим из секунд в адекватный вид
    printf("\n%s", asctime(nice));

    nicetime=end;
    nice=localtime(&nicetime);
    printf("%s\n", asctime(nice));

    fclose(file);
    return 0;
}