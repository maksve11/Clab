#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <ctype.h>

typedef unsigned __int32 u_int32_t;

const int base = 1000000000;

typedef struct
{
    u_int32_t arr[35];
    int count;
} uint1024_t;

int size(uint1024_t *x)
{
    for (int i = 34; i >= 0; i--)
    {
        if (x->arr[i] != 0)
        {
            x->count = i + 1;
            break;
        }
    }
    return x->count;
}

void scanf_value(uint1024_t *x)
{
    char *str;
    str = (char *)(malloc(315 * sizeof(char)));
    scanf("%s", str);
    if (strlen(str) > 315)
    {
        printf("\nIt's not a number\nUndefined behavior");
        return;
    }
    else
    {
        bool flag = 1;
        for (int j = 0; j < strlen(str); j++)
        {
            if (str[j] <= '9' && str[j] >= '0')
                continue;
            else
                flag = 0;
        }
        if (flag == 0)
        {
            printf("\nIt's not a number\nUndefined behavior");
            return;
        }
        else
        {
            for (int i = 0; i < 35; i++)
            {
                x->arr[i] = 0;
            }
            x->count = 0;
            for (int i = (int)strlen(str); i > 0; i -= 9)
            {
                str[i] = 0;
                x->arr[x->count] = (u_int32_t)(atoi(i >= 9 ? i + str - 9 : str));
                x->count++;
            }
        }
    }
    free(str);
}

void printf_value(uint1024_t x)
{
    int begin = size(&x) - 1;
    if (x.count == 0)
    {
        printf("0");
        return;
    }
    else if (x.arr[begin] > 0)
    {
        printf("%u", x.arr[begin]);
    }
    else if (x.arr[begin] == 0)
    {
        printf("0\n");
        return;
    }
    else
    {
        printf("Undefined Behavior\n");
        return;
    }
    for (int i = begin - 1; i >= 0; --i)
        printf("%09u", x.arr[i]);
    printf("\n");
}

uint1024_t from_uint(unsigned int x)
{
    uint1024_t k;
    for (int i = 0; i < 35; i++)
    {
        k.arr[i] = 0;
    }
    k.count = 0;
    k.arr[0] = x % base;
    k.arr[1] = x / base;
    int t = size(&k);
    return k;
    free(&k.arr);
}

uint1024_t add_op(uint1024_t x, uint1024_t y)
{
    int flagper = 0;
    for (int i = 0; i < 35; ++i)
    {
        x.arr[i] += flagper + (i >= size(&y) ? 0 : y.arr[i]);
        flagper = x.arr[i] >= base;
        if (flagper)
            x.arr[i] -= base;
    }
    return x;
    free(&x.arr);
    free(&x.count);
}

uint1024_t substr_op(uint1024_t x, uint1024_t y)
{
    int flagper = 0;
    for (int i = 0; i < 35; ++i)
    {
        x.arr[i] -= flagper + (i < size(&y) ? y.arr[i] : 0);
        flagper = y.arr[i] < 0;
        if (flagper)
            y.arr[i] += base;
    }
    return x;
}

uint1024_t mult_op(uint1024_t x, uint1024_t y)
{
    uint1024_t c;
    for (int i = 0; i < 35; i++)
    {
        c.arr[i] = 0;
    }
    c.count = 0;
    for (int i = 0; i < size(&x); ++i)
        for (int j = 0, flagg = 0; j < size(&y) || flagg; ++j)
        {
            long long tt = c.arr[i + j] + 1ll * x.arr[i] * (j < size(&y) ? y.arr[j] : 0) + flagg;
            c.arr[i + j] = (int)(tt % base);
            flagg = (int)(tt / base);
        }
    int t = size(&c);
    return c;
    free(&c.arr);
}

int main(int argc, char const *argv[])
{
    uint1024_t k, t, s, m, l, p;
    unsigned int ll;
    int len;
    // scanf("%u", &ll);
    printf("Input first number: ");
    scanf_value(&k);
    printf("Input second number: ");
    scanf_value(&t);
    // p = from_uint(ll);
    s = add_op(k, t);
    m = substr_op(k, t);
    p = mult_op(k, t);
    // s = mult_op(k, t);
    printf("Sum: ");
    printf_value(s);
    printf("Sub: ");
    printf_value(m);
    // printf("\n");
    printf("Mult: ");
    printf_value(p);
    // printf("\n");
    // printf_value(&s);
    return 0;
}